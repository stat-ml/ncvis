cdef extern from "../src/ncvis.hpp" namespace "ncvis":
    cdef cppclass NCVis:
        NCVis(size_t d, size_t n_threads, size_t n_neighbors, size_t M, size_t ef_construction, size_t random_seed, int max_epochs) except +
        float* fit(const float *const X, size_t N, size_t D, float a, float b, float alpha, float alpha_Q)
