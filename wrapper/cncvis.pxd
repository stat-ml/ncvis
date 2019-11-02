cdef extern from "../src/ncvis.hpp" namespace "ncvis":
    cdef enum Distance:
        squared_L2,
        inner_product,
        cosine_similarity,
        correlation

cdef extern from "../src/ncvis.hpp" namespace "ncvis":
    cdef cppclass NCVis:
        NCVis(long d, long n_threads, long n_neighbors, long M, long ef_construction, long random_seed, int n_epochs, int n_init_epochs, float a, float b, float alpha, float alpha_Q, long* n_noise, Distance dist) except +
        void fit_transform(const float *const X, long N, long D, float* Y)
