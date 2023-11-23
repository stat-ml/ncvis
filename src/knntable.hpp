#ifndef KNNTABLE_H
#define KNNTABLE_H

#include <cstddef>
#include <vector>

namespace ncvis {
class KNNTable {
   public:
    KNNTable(long N, long k, float reserve_ratio = 1.);
    ~KNNTable();
    void symmetrize();
    long size();

    std::vector<std::vector<long> > inds;  // Vectors for easy resize handling
    std::vector<std::vector<float> > dists;

   private:
    long N_;
    long k_;
    float reserve_ratio_;
};
}  // namespace ncvis

#endif  // knntable.hpp
