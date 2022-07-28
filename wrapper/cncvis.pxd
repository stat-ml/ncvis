from libc.stdint cimport int32_t, int64_t

cdef extern from "../src/ncvis.hpp" namespace "ncvis":
    cdef enum Distance:
        squared_L2,
        inner_product,
        cosine_similarity,
        correlation

cdef extern from "../src/ncvis.hpp" namespace "ncvis":
    cdef cppclass NCVis:
        NCVis(int64_t d, int64_t n_threads, int64_t n_neighbors, int64_t M, int64_t ef_construction, int64_t random_seed, int32_t n_epochs, int32_t n_init_epochs, float a, float b, float alpha, float alpha_Q, int64_t* n_noise, Distance dist) except +
        void fit_transform(const float *const X, int64_t N, int64_t D, float* Y)
