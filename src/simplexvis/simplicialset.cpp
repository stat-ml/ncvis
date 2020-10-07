#include <omp.h>
#include "simplicialset.hpp"
#include <cstdlib>
#include <cstring>
#include <algorithm>

SimplicialSet::SimplicialSet(long N, long M, const long* E, long n_threads):
N_(N), subsets_(2), adjacency_(N)
{
    omp_set_num_threads(n_threads);
    // Initialize 0-simplices
    auto& nodes = subsets_[0];
    nodes.resize(N);

    // Initialize 1-simplices
    auto& edges = subsets_[1];
    edges.resize(2*M);
    
    #pragma omp parallel
    {
    #pragma omp for nowait
    for (long i=0; i < N; ++i){
        nodes[i] = i;
        std::cout << omp_get_thread_num() << std::endl;
    }

    // Order each simplex
    #pragma omp for
    for (long i=0; i < M; ++i){
        auto pos = 2*i;
        std::copy_n(E+pos, 2, edges.begin()+pos);
        auto& to = edges[pos];
        auto& from = edges[pos+1]; 
        if (from < to) {std::swap(from, to); }

        adjacency_[from].emplace_back(to);
    }

    std::cout << "Adjacency" << std::endl;
    #pragma omp for
    for(long i=0; i < adjacency_.size(); ++i){
        auto& conn = adjacency_[i];
        std::sort(conn.begin(), conn.end());
        std::cout << "[" << i << "] ";
        for(auto e: conn){
            std::cout << e << ", ";
        }
        std::cout << std::endl;
    }
    }
}

const std::vector<long>& SimplicialSet::get_simplices(long d) 
{
    return subsets_[d];
}

