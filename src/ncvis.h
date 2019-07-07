#include <iostream>
#include <ostream>
#include <tuple>
#include "../lib/nmslib/hnswlib/hnswlib.h"

#ifndef NCVIS_H
#define NCVIS_H

namespace ncvis {
    class NCVis{
        public:
            NCVis(size_t d, size_t n_threads=1, size_t n_neighbors=30, size_t M = 16, size_t ef_construction = 200, size_t random_seed = 42);
            ~NCVis();
            void fit(const float *const X, size_t N, size_t D);

        private:
            size_t d_;
            size_t M_;
            size_t ef_construction_; 
            size_t random_seed_;
            size_t n_neighbors_;
            hnswlib::L2Space* l2space_;
            hnswlib::HierarchicalNSW<float>* appr_alg_;

            void buildKNN(const float *const X, size_t N, size_t D);
            std::tuple<float*, size_t*> findKNN(const float *const X, size_t N, size_t D, size_t k);
    };
}

#endif
