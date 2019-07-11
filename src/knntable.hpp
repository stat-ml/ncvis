#include <vector>

#ifndef KNNTABLE_H
#define KNNTABLE_H

namespace ncvis{
    class KNNTable{
    public:
        KNNTable(size_t N, size_t k, float reserve_ratio=1.);
        ~KNNTable();
        void symmetrize();
        size_t size();
        
        std::vector< std::vector<size_t> > inds; // Vectors for easy resize handling
        std::vector< std::vector<float> > dists;
    private:
        size_t N_;
        size_t k_;
        float reserve_ratio_;
    };
}

#endif // knntable.hpp