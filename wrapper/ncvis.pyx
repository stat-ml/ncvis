# distutils: language=c++

from wrapper cimport cncvis
import numpy as np
cimport numpy as cnp
import ctypes

cdef class NCVis:
    cdef cncvis.NCVis* c_ncvis;

    def __cinit__(self, size_t d):
        self.c_ncvis = new cncvis.NCVis(d)
        cdef cnp.ndarray[double, ndim=2, mode='c'] X = np.zeros((2, 2))
        print("Before", X)
        self.c_ncvis.add_one(&X[0, 0], X.shape[0], X.shape[1])
        print("After", X)

    def __dealloc__(self):
        del self.c_ncvis