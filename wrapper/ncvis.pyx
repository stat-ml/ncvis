# distutils: language=c++

from wrapper cimport cncvis
import numpy as np
cimport numpy as cnp
import ctypes

cdef class NCVis:
    cdef cncvis.NCVis* c_ncvis
    cdef size_t d

    def __cinit__(self, size_t d=2, size_t n_threads=1, size_t n_neighbors=30, size_t M = 16, size_t ef_construction = 200, size_t random_seed = 42, int max_epochs=50, int n_init_epochs=20):
        self.c_ncvis = new cncvis.NCVis(d, n_threads, n_neighbors, M, ef_construction, random_seed, max_epochs, n_init_epochs)
        self.d = d

    def __dealloc__(self):
        del self.c_ncvis

    def fit(self, float[:, :] X, float a=1., float b=1., float alpha=1., float alpha_Q=1.):
        return np.asarray(<float[:X.shape[0], :self.d]>self.c_ncvis.fit(&X[0, 0], X.shape[0], X.shape[1], a, b, alpha, alpha_Q))    