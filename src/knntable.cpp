#include "knntable.hpp"
#include <stdio.h>
#include <iostream>
#include <algorithm>
// #include <iterator>
#include <omp.h>

ncvis::KNNTable::KNNTable(size_t N, size_t k, float reserve_ratio):
inds(N),
dists(N),
N_(N), k_(k), reserve_ratio_(reserve_ratio)
{
    for (size_t i = 0; i < N_; ++i){
        // Preserve memory for extra edges from symmetrization
        size_t size = (size_t)(k_*(1+reserve_ratio_));
        inds[i].reserve(size);
        dists[i].reserve(size);
    }
}

ncvis::KNNTable::~KNNTable(){
    
}

void ncvis::KNNTable::symmetrize(){
    std::vector<omp_lock_t> locks(N_);
    std::vector< std::vector<size_t> > inds_add(N_);
    std::vector< std::vector<float> > dists_add(N_);
    #pragma omp parallel
    {
    #pragma omp for
    for (size_t i = 0; i < N_; ++i){
        omp_init_lock(&locks[i]);
        // Preallocate memory for new edges
        size_t size = (size_t)(k_*reserve_ratio_);
        inds_add[i].reserve(size);
        dists_add[i].reserve(size);
    }
    
    // Collect incoming edges
    #pragma omp for
    for (size_t i = 0; i < N_; ++i){
        for (size_t j = 0; j < k_; ++j){
            size_t edge_to = inds[i][j];
            omp_set_lock(&locks[edge_to]);
            inds_add[edge_to].push_back(i);
            dists_add[edge_to].push_back(dists[i][j]);
            omp_unset_lock(&locks[edge_to]);
        }
    }
    
    // Clear locks
    #pragma omp for
    for (size_t i = 0; i < N_; ++i){
        omp_destroy_lock(&locks[i]);
    }

    // Merge, remove duplicates and sort by distance
    #pragma omp for
    for (size_t i = 0; i < N_; ++i){
        // std::cout << "i = " << i << std::endl;
        // std::copy(
        // inds_add[i].begin(),
        // inds_add[i].end(),
        // std::ostream_iterator<size_t>(std::cout, " ")
        // );
        // std::cout << std::endl;

        // Add new edges to the list
        inds[i].insert(inds[i].end(), inds_add[i].begin(), inds_add[i].end());
        dists[i].insert(dists[i].end(), dists_add[i].begin(), dists_add[i].end());
        
        std::vector<size_t> idx(inds[i].size());
        for (size_t j = 0; j < idx.size(); ++j){
            idx[j] = j;
        }       

        std::vector<size_t> inds_tmp((const std::vector<size_t>) inds[i]);
        std::vector<float> dists_tmp((const std::vector<float>) dists[i]);

        // Get indices that sort the array by distance
        const auto& key = dists[i];
        std::sort(idx.begin(), idx.end(), 
                  [&key](size_t i1, size_t i2){return key[i1] < key[i2];});

        size_t k = 0;
        for (size_t j = 0; j < idx.size(); ++j){
            // Add edge only if it is unique
            if (j == 0 || inds[i][idx[j]] != inds[i][idx[j-1]]){
                inds[i][k] = inds_tmp[idx[j]];
                dists[i][k] = dists_tmp[idx[j]];
                ++k;
            }
        }
        // Remove extra elements
        inds[i].erase(inds[i].begin()+k, inds[i].end());
        dists[i].erase(dists[i].begin()+k, dists[i].end());
    }
    }
}

size_t ncvis::KNNTable::size(){
    if (dists.size() != inds.size()){
        std::cerr << "[ncvis::KNNTable::size] Indices and distances sizes differ.";
        return std::min(dists.size(), inds.size());
    }
    return dists.size();
}