#include <iostream>
#include <ostream>
#include <utility>
#include "../lib/hnswlib/hnswlib/hnswlib.h"
#include "knntable.hpp"
#include <cstddef>

#ifndef NCVIS_H
#define NCVIS_H

namespace ncvis {
    typedef std::pair<long, long> Edge;
    enum Distance
    {
        squared_L2,
        inner_product,
        cosine_similarity,
        correlation
    };

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
        @param max_epoch Number of optimization epochs.
        @param n_init_epochs Number of initialization epochs. 
        @param a,b Likelihood kernel parameters from: P(x, y) = 1/(1+a*|x-y|^(2*b))
        @param alpha,alpha_Q Learning rates for the embedding and normalization constant correspondingly.
        @param n_noise Number of noise samples per data sample for each iteration. An array of size [n_epochs]; will be initialized to 3 noise samples per data sample for each epoch if not provided.
        @param dist Distance to use for nearest neighbors search.
        */
        NCVis(long d=2, long n_threads=1, long n_neighbors=30, long M = 16, long ef_construction = 200, long random_seed = 42, int n_epochs=50, int n_init_epochs=20, float a=1., float b=1., float alpha=1., float alpha_Q=1., long* n_noise=nullptr, ncvis::Distance dist=ncvis::Distance::squared_L2);
        ~NCVis();
        /*!
        @brief Build embedding for points.

        Builds low-dimensional embedding for the points array of shape [N, D], where N is the number of samples and D is their dimensionality.

        @param X Pointer to the data array [N, D]. The j-th coordinate of i-th sample is assumed to be found at (X+D*i+j).
        @param N Number of samples.
        @param D Dimensionality of samples.
        @param Y Pointer to the embedding [N, d]. The j-th coordinate of i-th sample is assumed to be found at (X+d*i+j).
        */
        void fit_transform(const float *const X, long N, long D, float* Y);
    
    private:
        long d_;
        long M_;
        long ef_construction_; 
        long random_seed_;
        long n_neighbors_;
        int n_epochs_;
        int n_init_epochs_;
        float a_;
        float b_;
        float alpha_;
        float alpha_Q_;
        long* n_noise_;

        hnswlib::SpaceInterface<float>* space_;
        hnswlib::HierarchicalNSW<float>* appr_alg_;
        Distance dist_;

        void preprocess(const float *const x, long D, ncvis::Distance dist, float* out);
        float d_sqr(const float *const x, const float *const y);
        void buildKNN(const float *const X, long N, long D);
        KNNTable findKNN(const float *const X, long N, long D, long k);
        std::vector<Edge> build_edges(KNNTable& table);
        void init_embedding(long N, float* Y, float alpha, std::vector<ncvis::Edge>& edges);
        void optimize(long N, float* Y, float& Q, std::vector<ncvis::Edge>& edges);
    };
}

#endif //ncvis.hpp