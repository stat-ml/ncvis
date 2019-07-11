#include <omp.h>
#include <random>
#include <cmath>
#include "ncvis.hpp"
#include "../lib/pcg-cpp/include/pcg_random.hpp"

ncvis::NCVis::NCVis(size_t d, size_t n_threads, size_t n_neighbors, size_t M, 
                    size_t ef_construction, size_t random_seed, int max_epochs):
d_(d), M_(M), ef_construction_(ef_construction), 
random_seed_(random_seed), n_neighbors_(n_neighbors), max_epochs_(max_epochs),l2space_(nullptr), appr_alg_(nullptr)
{
    omp_set_num_threads(n_threads);
}

ncvis::NCVis::~NCVis(){
    delete l2space_;
    delete appr_alg_;
}

void ncvis::NCVis::buildKNN(const float *const X, size_t N, size_t D){
    delete l2space_;
    delete appr_alg_;  
    l2space_ = new hnswlib::L2Space(D);
    appr_alg_ = new hnswlib::HierarchicalNSW<float>(l2space_, N, M_, ef_construction_, random_seed_);

    // Perform initialisation without messing with mutexes 
    appr_alg_->addPoint((void*)X, (size_t) 0);
    #pragma omp parallel for
    for (size_t i=1; i < N; ++i){
        appr_alg_->addPoint((void*)(X+i*D), i);
    }
}

ncvis::KNNTable ncvis::NCVis::findKNN(const float *const X, size_t N, size_t D, size_t k){
    KNNTable table(N, k);

    #pragma omp parallel for
    for (size_t i=0; i < N; ++i){
        // Find k+1 neighbors as one of them is the point itself 
        auto result = appr_alg_->searchKnn((const void*)(X+i*D), k+1);
        if (result.size() != k+1){
            std::cout << "[ncvis::NCVis::findKNN] Found less than k nearest neighbors, try increasing M or ef_construction.";
        } else{
            for (size_t j=0; j<k; ++j) {
                auto& result_tuple = result.top();
                table.dists[i].push_back(result_tuple.first);
                table.inds[i].push_back(result_tuple.second);
                result.pop();
            }
        }
    }

    return table;
}

void ncvis::NCVis::build_edges(ncvis::KNNTable& table){
    size_t n_edges = 0;
    for (const auto& i : table.inds){
        n_edges += i.size();
    }
    edges_.reserve(n_edges);

    for (size_t i = 0; i < table.inds.size(); ++i){
        for (const auto& j : table.inds[i]){
            edges_.emplace_back(i, j);
        }
    }
}

float ncvis::NCVis::d_sqr(const float *const x, const float *const y){
    float dist_sqr = 0;
    for (size_t i = 0; i < d_; ++i){
        dist_sqr += (x[i]-y[i])*(x[i]-y[i]);
    }

    return dist_sqr;
}

float* ncvis::NCVis::fit(const float *const X, size_t N, size_t D, float a, float b, float alpha, float alpha_Q){
    if (N == 0 || D == 0){ 
        throw std::runtime_error("[ncvis::NCVis::fit] Dataset should have at least one element.");
        return nullptr;
    }

    buildKNN(X, N, D);

    // Number of neighbors can't exceed the total number of other points 
    size_t k = (n_neighbors_ < N-1)? n_neighbors_:(N-1);
    KNNTable table = findKNN(X, N, D, k);

    table.symmetrize();
    build_edges(table);
    // printf("Edges size: %ld\n", edges_.size());
    // printf("==============EDGES============\n");
    // for (const auto& i : edges_){
    //     printf("(%ld, %ld)\n", i.first, i.second); 
    // }
    // printf("===============================\n");

    float* Y = new float[N*d_];
    size_t n_noise = 3;
    // Likelihood parameters
    // Normalization
    float Q=0.;
    #pragma omp parallel
    {
        pcg64 pcg(random_seed_+omp_get_thread_num());
        std::uniform_real_distribution<float> gen_Y(0, 1);
        #pragma omp for
        for (size_t i = 0; i < N*d_; ++i){
            Y[i] = gen_Y(pcg);
        }

        std::uniform_int_distribution<size_t> gen_ind(0, N-1);
        for (int epoch = 0; epoch < max_epochs_; ++epoch){
            // Hogwild: lock-free parameters reading and writing
            #pragma omp for
            for (size_t i = 0; i < edges_.size(); ++i){
                // printf("[%d] (%ld, %ld)\n", epoch, edges_[i].first, edges_[i].second);
                size_t id = edges_[i].first;
                std::vector<size_t> other;
                other.reserve(n_noise+1);
                other.push_back(edges_[i].second);
                // Generate noise samples
                for (size_t j = 1; j < n_noise+1; ++j){
                    size_t id_other = gen_ind(pcg);
                    if (id_other == id){
                        --j;
                        continue;
                    }
                    other.push_back(id_other);
                    // printf("%ld\n", id_other);
                }

                std::vector<float> d2, Ph, w;
                d2.reserve(n_noise+1);
                Ph.reserve(n_noise+1);
                w.reserve(n_noise+1);
                for (const auto& j : other){
                    float d2_tmp = d_sqr(Y+id*d_, Y+j*d_);
                    d2.push_back(d2_tmp);
                    float Ph_tmp = 1/(1+a*std::pow(d2_tmp, b)); 
                    Ph.push_back(Ph_tmp);
                    w.push_back(Ph_tmp/(n_noise*std::exp(Q)));
                }
                w[0] = 1/(1+w[0]);
                float dQ = w[0];
                for (size_t j = 1; j < n_noise+1; ++j){
                    w[j] = -1/(1+1/w[j]);
                    dQ += w[j];
                }
                // Non-blocking write
                Q -= dQ*alpha_Q;
                // printf("[%d]{%d} Q = %f\n", epoch, omp_get_thread_num(), Q);
                std::vector<float> dx(d_);
                for (size_t j = 0; j < n_noise; ++j){
                    w[j] = 2*w[j]*Ph[j]*a*b*std::pow(d2[j], b-1);

                    // Also non-blocking write
                    for (size_t k = 0; k < d_; ++k){
                        float dx_k = Y[other[j]*d_+k] - Y[id*d_+k];
                        dx_k = dx_k * w[j] * alpha;
                        if (dx_k > 4.){
                            dx_k = 4.;
                        } else if (dx_k < -4.){
                            dx_k = -4.;
                        }
                        Y[id*d_+k] += dx_k;
                        Y[other[j]*d_+k] -= dx_k;
                    }
                }
            }
        }
        
    }

    // printf("============DISTANCES==========\n");
    // for (size_t i=0; i<N; ++i){
    //     printf("[");
    //     for (size_t j=0; j<table.dists[i].size(); ++j){
    //         printf("%f ", table.dists[i][j]);
    //     }
    //     printf("]\n");
    // }
    // printf("===============================\n");

    printf("============NEIGHBORS==========\n");
    for (size_t i=0; i<N; ++i){
        printf("[");
        for (size_t j=0; j<table.inds[i].size(); ++j){
            printf("%ld ", table.inds[i][j]);
        }
        printf("]\n");
    }
    printf("===============================\n");

    return Y;
}