#include <iostream>
#include <utility>
#include <cassert>
#include <cstddef>
#include <vector>

#ifndef __SIMPLICIALSET_H__
#define __SIMPLICIALSET_H__

#define DEBUG

// Each simplix is stored in descending order, for example, [5, 2, 1]
typedef struct Simplices{
    long N; // Total number of simplices
    long M; // Number of nodes per simplex
    long* data; // N*M nodes indices 
}
Simplices;

class SimplicialSet{
public:
    // N = number of nodes
    // E = [M, 2] array of edges
    SimplicialSet() = default;
    SimplicialSet(long N, long M, const long *const E, long n_threads);
    ~SimplicialSet() = default;
    // Get simplices of the given dimensionality
    const std::vector<long>& get_simplices(long d);
    // Build d-simplices if they are not already present, d >= 2
    void build_simplices(long d);
private:
    long N_;
    // subset_[k] stores all k-simplices (k+1 nodes each) present in the set
    // each vector has ascending order
    std::vector<std::vector<long>> subsets_;
    // each vector stores all connected nodes with the index smaller 
    // than the target 
    std::vector<std::vector<long>> adjacency_;

    // Build d-dimensional simplices presuming d-1 dimensional
    void build_simplices_(long d);
};

#endif // __SIMPLICIALSET_H__