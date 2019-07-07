#include <omp.h>
#include "ncvis.h"

ncvis::NCVis::NCVis(size_t d, size_t n_threads, size_t n_neighbors, size_t M, 
                    size_t ef_construction, size_t random_seed):
d_(d), M_(M), ef_construction_(ef_construction), 
random_seed_(random_seed), n_neighbors_(n_neighbors), l2space_(nullptr), appr_alg_(nullptr)
{
    omp_set_num_threads(n_threads);
    std::cout << "NCVis::NCVis()\n";
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
    for (size_t i=1; i < N; i++){
        appr_alg_->addPoint((void*)(X+i*D), i);
    }
}

std::tuple<float*, size_t*> ncvis::NCVis::findKNN(const float *const X, size_t N, size_t D, size_t k){
    float* dists  = new float[N*k];
    size_t* inds  = new size_t[N*k];

    #pragma omp parallel for
    for (size_t i=0; i < N; i++){
        // Find k+1 neighbors as one of them is the point itself 
        auto result = appr_alg_->searchKnn((const void*)(X+i*D), k+1);
        if (result.size() != k+1){
            std::cout << "[ncvis::NCVis::findKNN] Found less than k nearest neighbors, try increasing M or ef_construction.";
        } else{
            for (size_t j=0; j<k; j++) {
                auto& result_tuple = result.top();
                size_t offset = (i+1)*k-j-1;
                dists[offset] = result_tuple.first;
                inds[offset]  = result_tuple.second;
                result.pop();
            }
        }
    }

    return std::make_tuple(dists, inds);
}

void ncvis::NCVis::fit(const float *const X, size_t N, size_t D){
    if (N == 0 || D == 0){ 
        throw std::runtime_error("[ncvis::NCVis::fit] Dataset should have at least one element.");
        return;
    }

    buildKNN(X, N, D);

    float* dists;
    size_t* inds;
    // Number of neighbors can't exceed the total number of other points 
    size_t k = (n_neighbors_ < N-1)? n_neighbors_:(N-1);
    std::tie(dists, inds) = findKNN(X, N, D, k);

    // printf("============DISTANCES==========\n");
    // for (size_t i=0; i<N; i++){
    //     printf("[");
    //     for (size_t j=0; j<k; j++){
    //         printf("%f ", dists[k*i+j]);
    //     }
    //     printf("]\n");
    // }
    // printf("===============================\n");

    // printf("============NEIGHBORS==========\n");
    // for (size_t i=0; i<N; i++){
    //     printf("[");
    //     for (size_t j=0; j<k; j++){
    //         printf("%ld ", inds[k*i+j]);
    //     }
    //     printf("]\n");
    // }
    // printf("===============================\n");

    delete[] dists;
    delete[] inds;
}