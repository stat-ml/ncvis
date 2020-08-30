#include <iostream>
#include <utility>
#include <cassert>
#include <cstddef>
#include <vector>

#ifndef SIMPLEXVIS_H
#define SIMPLEXVIS_H

#define DEBUG

namespace simplexvis {
    typedef struct Edge{
        long first;
        long second;
        float weight;
    }
    Edge;

    class SimplexVis{
    public:
        /*! 
        @brief SimplexVis constructor.
        
        Constructs SimplexVis instance with respect to passed parameters.

        @param d Embedding dimensionality.
        @param n_threads Maximum number of threads to use.
        @param max_clique Maximum clique size to consider. 
        @param random_seed Each thread's random generator is initialised with (random_seed + thread_id)
        @param max_epoch Number of optimization epochs.
        @param n_init_epochs Number of initialization epochs. 
        @param a,b Likelihood kernel parameters from: P(x, y) = 1/(1+a*|x-y|^(2*b))
        @param alpha,alpha_Q Learning rates for the embedding and normalization constant correspondingly.
        @param n_noise Number of noise samples per data sample for each iteration. An array of size [n_epochs]; will be initialized to 3 noise samples per data sample for each epoch if not provided.
        */
        SimplexVis(long d=2, long n_threads=1, long max_clique_size=2, long random_seed=42, int n_epochs=50, int n_init_epochs=20, float a=1., float b=1., float alpha=1., float alpha_Q=1., long* n_noise=nullptr);
        ~SimplexVis();
        /*!
        @brief Build graph embedding.

        Builds low-dimensional embedding for the graph with edges [M, 2], where E is the number of edges and each row is a pair of nodes' indices. Indices are supposed to be in range [0..N].

        @param E Pointer to the edges array [M, 2].
        @param W Pointer to the weights array [M].
        @param N Number of nodes.
        @param Y Pointer to the embedding [N, d]. The j-th coordinate of i-th sample is assumed to be found at (Y+d*i+j).
        */
        void fit_transform(long N, long M, const long *const E, const float *const W, float* Y);
    
    private:
        long d_;
        long random_seed_;
        long max_clique_size_;
        int n_epochs_;
        int n_init_epochs_;
        float a_;
        float b_;
        float alpha_;
        float alpha_Q_;
        long* n_noise_;

        void init_embedding(long N, long M, const long *const E, const float *const W, float* Y);
        void optimize(long N, long M, const long *const E, const float *const W, float* Y);
    };
}

#endif //simplexvis.hpp