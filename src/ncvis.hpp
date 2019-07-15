#include <iostream>
#include <ostream>
#include <utility>
#include "../lib/hnswlib/hnswlib/hnswlib.h"
#include "knntable.hpp"

#ifndef NCVIS_H
#define NCVIS_H

namespace ncvis {
    typedef std::pair<size_t, size_t> Edge;

    class NCVis{
    public:
        /*! 
        @brief NCVis constructor.
        
        Constructs NCVis instance with respect to passed parameters.

        @param d Embedding dimensionality.
        @param n_threads Maximum number of threads to use.
        @param n_neighbors Number of nearest neighbors to find for each point.
        @param M <a href="https://github.com/nmslib/hnswlib/blob/master/ALGO_PARAMS.md">(hnswlib)</a> The number of bi-directional links created for every new element during construction.
        @param ef_construction <a href="https://github.com/nmslib/hnswlib/blob/master/ALGO_PARAMS.md">(hnswlib)</a> The size of the dynamic list for the nearest neighbors
        @param random_seed Each thread's random generator is initialised with (random_seed + thread_id)
        @param max_epoch Maximum number of optimization epochs.
        */
        NCVis(size_t d=2, size_t n_threads=1, size_t n_neighbors=30, size_t M = 16, size_t ef_construction = 200, size_t random_seed = 42, int max_epochs=50, int n_init_epochs=20);
        ~NCVis();
        /*!
        @brief Build embedding for points.

        Builds low-dimensional embedding for the points array of shape [N, D], where N is the number of samples and D is their dimensionality.

        @param X Pointer to the data array [N, D]. The j-th coordinate of i-th sample is assumed to be found at (X+D*i+j).
        @param N Number of samples.
        @param D Dimensionality of samples.
        @param a,b Likelihood kernel parameters from: P(x, y) = 1/(1+a*|x-y|^(2*b))
        @param alpha,alpha_Q Learning rates for the embedding and normalization constant correspondingly.

        @return Pointer to the embedding [N, d]. The j-th coordinate of i-th sample is assumed to be found at (X+d*i+j).
         */
        float* fit(const float *const X, size_t N, size_t D, float a=1., float b=1., float alpha=1., float alpha_Q=1.);
    private:
        size_t d_;
        size_t M_;
        size_t ef_construction_; 
        size_t random_seed_;
        size_t n_neighbors_;
        int max_epochs_;
        int n_init_epochs_;
        hnswlib::L2Space* l2space_;
        hnswlib::HierarchicalNSW<float>* appr_alg_;
        std::vector<Edge> edges_;

        float d_sqr(const float *const x, const float *const y);
        void buildKNN(const float *const X, size_t N, size_t D);
        KNNTable findKNN(const float *const X, size_t N, size_t D, size_t k);
        void build_edges(KNNTable& table);
    };
}

#endif //ncvis.hpp