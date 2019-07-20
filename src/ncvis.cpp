#include <omp.h>
#include <random>
#include <cmath>
#include "ncvis.hpp"
#include "../lib/pcg-cpp/include/pcg_random.hpp"
#include <chrono>

ncvis::NCVis::NCVis(size_t d, size_t n_threads, size_t n_neighbors, size_t M, 
                    size_t ef_construction, size_t random_seed, int n_epochs, 
                    int n_init_epochs, float a, float b, float alpha, float alpha_Q, size_t* n_noise):
d_(d), M_(M), ef_construction_(ef_construction), 
random_seed_(random_seed), n_neighbors_(n_neighbors), n_epochs_(n_epochs), n_init_epochs_(n_init_epochs), a_(a), b_(b), alpha_(alpha), alpha_Q_(alpha_Q), l2space_(nullptr), appr_alg_(nullptr)
{
    omp_set_num_threads(n_threads);
    n_noise_ = new size_t[n_epochs];
    size_t default_noise = 3;
    for (int i=0; i<n_epochs; ++i){
        n_noise_[i] = (n_noise == nullptr)? default_noise:n_noise[i];
    }
}

ncvis::NCVis::~NCVis(){
    delete l2space_;
    delete appr_alg_;
    delete[] n_noise_;
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

    #pragma omp parallel for
    for (const auto& i : table.inds){
        #pragma omp atomic
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

void ncvis::NCVis::init_embedding(size_t N, float*& Y, float alpha){
    // For temporary values 
    float* Y_swap = new float[N*d_];
    float* mean = new float[d_];
    float* sigma = new float[d_];

    #pragma omp parallel
    {
    pcg64 pcg(random_seed_+omp_get_thread_num());
    std::uniform_real_distribution<float> gen_Y(0, 1);

    #pragma omp for
    for (size_t i = 0; i < N*d_; ++i){
        Y[i] = gen_Y(pcg);
    }

    // Initialize layout
    for (int init_epoch = 0; init_epoch < n_init_epochs_; ++init_epoch){
        #pragma omp single
        {
        float* tmp = Y_swap;
        Y_swap = Y;
        Y = tmp;
        }
        #pragma omp for
        for (size_t i = 0; i < N*d_; ++i){
            Y[i] = 0;
        }
        #pragma omp for
        for (size_t i = 0; i < edges_.size(); ++i){
            size_t id = edges_[i].first;
            size_t other_id = edges_[i].second;
            
            for (size_t k = 0; k < d_; ++k){
                // float dx_k = Y[other_id*d_+k] - Y[id*d_+k];
                // dx_k = dx_k * init_alpha;
                // printf("dx[%ld] = %f\n", k, dx_k);
                // Y[id*d_+k] += dx_k;
                // Y[other_id*d_+k] -= dx_k;
                Y[id*d_+k] += alpha * Y_swap[other_id*d_+k];
            }
        }

        #pragma omp single
        for (size_t k = 0; k < d_; ++k){
            mean[k] = 0;
            sigma[k] = 0;
        }

        #pragma omp for
        for (size_t i = 0; i < N; ++i){
            for (size_t k = 0; k < d_; ++k){
                #pragma omp atomic
                mean[k] += Y[i*d_+k];
            }
        }

        #pragma omp single
        for (size_t k = 0; k < d_; ++k){
            mean[k] /= N;
        }

        #pragma omp for
        for (size_t i = 0; i < N; ++i){
            for (size_t k = 0; k < d_; ++k){
                #pragma omp atomic
                sigma[k] += (Y[i*d_+k] - mean[k])*(Y[i*d_+k] - mean[k]);
            }
        }

        #pragma omp single
        for (size_t k = 0; k < d_; ++k){
            sigma[k] /= N;
            sigma[k] = sqrtf(sigma[k]);
        }

        #pragma omp for
        for (size_t i = 0; i < N; ++i){
            for (size_t k = 0; k < d_; ++k){
                Y[i*d_+k] = (Y[i*d_+k] - mean[k])/sigma[k];
            }
        }
    }
    }
    
    delete[] Y_swap;
    delete[] mean;
    delete[] sigma;
}

void ncvis::NCVis::optimize(size_t N, float* Y, float& Q){
    float Q_cum=0.;
    #pragma omp parallel
    {
    pcg64 pcg(random_seed_+omp_get_thread_num());
    // Build layout
    std::uniform_int_distribution<size_t> gen_ind(0, N-1);

    for (int epoch = 0; epoch < n_epochs_; ++epoch){
        // Hogwild: lock-free parameters reading and writing
        float step = alpha_*(1-(((float)epoch)/n_epochs_)*(((float)epoch)/n_epochs_));
        float Q_copy = Q;
        size_t cur_noise = n_noise_[epoch];
        Q_cum = 0;
        #pragma omp for
        for (size_t i = 0; i < edges_.size(); ++i){
            // printf("[%d] (%ld, %ld)\n", epoch, edges_[i].first, edges_[i].second);
            size_t id = edges_[i].first;
            for (size_t j = 0; j < cur_noise+1; ++j){
                size_t other_id;
                if (j == 0){
                    other_id = edges_[i].second; 
                } else{
                    do{
                        other_id = gen_ind(pcg);
                    } while (other_id == id);
                }

                float d2 = d_sqr(Y+id*d_, Y+other_id*d_);
                float Ph = 1/(1+a_*powf(d2, b_));
                float w = 1.;
                if (cur_noise != 0){
                    w = Ph/(cur_noise*expf(Q_copy));
                    // w = Ph/(cur_noise*expf(Q));
                    if (j == 0){
                        w = 1/(1+w);    
                    } else {
                        w = -1/(1+1/w);
                    }
                    // Non-blocking write
                    // #pragma omp critical
                    // {
                    // printf("[%d:%d] Ph = %f\n", epoch, omp_get_thread_num(), Ph);
                    // }
                    Q_copy -= w*alpha_Q_;
                    // Q -= w*alpha_Q;
                    w = 2*w*Ph*a_*b_*powf(d2, b_-1);
                }
                // #pragma omp critical
                // {
                // printf("[%d:%d] Q = %f\n", epoch, omp_get_thread_num(), Q);
                // }
                // Also non-blocking write
                for (size_t k = 0; k < d_; ++k){
                    float dx_k = Y[other_id*d_+k] - Y[id*d_+k];
                    dx_k = dx_k * w * step;
                    if (dx_k > 4.){
                        dx_k = 4.;
                    } else if (dx_k < -4.){
                        dx_k = -4.;
                    }
                    Y[id*d_+k] += dx_k;
                    Y[other_id*d_+k] -= dx_k;
                }
            }  
        }
        #pragma omp critical
        Q_cum += Q_copy;
        #pragma omp single
        Q = Q_cum/omp_get_num_threads();
    }
    }
}

float* ncvis::NCVis::fit(const float *const X, size_t N, size_t D){
    if (N == 0 || D == 0){ 
        throw std::runtime_error("[ncvis::NCVis::fit] Dataset should have at least one element.");
        return nullptr;
    }
    #if defined(DEBUG)
        auto t1 = std::chrono::high_resolution_clock::now();
        buildKNN(X, N, D);
        auto t2 = std::chrono::high_resolution_clock::now();
        std::cout << "buildKNN: "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
                  << " ms\n";
    #else 
        buildKNN(X, N, D);
    #endif
    // Number of neighbors can't exceed the total number of other points 
    size_t k = (n_neighbors_ < N-1)? n_neighbors_:(N-1);
    k = (k > 0)? k:1;
    #if defined(DEBUG)
        t1 = std::chrono::high_resolution_clock::now();
        KNNTable table = findKNN(X, N, D, k);
        t2 = std::chrono::high_resolution_clock::now();
        std::cout << "findKNN: "
                << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
                << " ms\n";
        t1 = std::chrono::high_resolution_clock::now();
        table.symmetrize();
        t2 = std::chrono::high_resolution_clock::now();
        std::cout << "symmetrize: "
                << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
                << " ms\n";
        t1 = std::chrono::high_resolution_clock::now();
        build_edges(table);
        t2 = std::chrono::high_resolution_clock::now();
        std::cout << "build_edges: "
                << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
                << " ms\n";
    #else
        KNNTable table = findKNN(X, N, D, k);
        table.symmetrize();
        build_edges(table);
    #endif
  
    // printf("Edges size: %ld\n", edges_.size());
    // printf("==============EDGES============\n");
    // for (const auto& i : edges_){
    //     printf("(%ld, %ld)\n", i.first, i.second); 
    // }
    // printf("===============================\n");
    // Likelihood parameters
    float* Y = new float[N*d_];
    // Normalization
    float Q=0.;
    float init_alpha = 1./k;
    
    #if defined(DEBUG)
        t1 = std::chrono::high_resolution_clock::now();
    #endif

    init_embedding(N, Y, init_alpha);
    optimize(N, Y, Q);
    
    #if defined(DEBUG)
        t2 = std::chrono::high_resolution_clock::now();
        std::cout << "main cycle: "
                << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
                << " ms\n";
    #endif
    // printf("============DISTANCES==========\n");
    // for (size_t i=0; i<N; ++i){
    //     printf("[");
    //     for (size_t j=0; j<table.dists[i].size(); ++j){
    //         printf("%f ", table.dists[i][j]);
    //     }
    //     printf("]\n");
    // }
    // printf("===============================\n");

    // printf("============NEIGHBORS==========\n");
    // for (size_t i=0; i<N; ++i){
    //     printf("[");
    //     for (size_t j=0; j<table.inds[i].size(); ++j){
    //         printf("%ld ", table.inds[i][j]);
    //     }
    //     printf("]\n");
    // }
    // printf("===============================\n");

    return Y;
}