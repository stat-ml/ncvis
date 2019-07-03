# distutils: language=c++

from wrapper cimport cncvis
import numpy as np
from cpython.mem cimport PyMem_Malloc, PyMem_Free

cdef class NCVis:
    cdef cncvis.NCVis c_ncvis;

    def __init__(self, double[:, ::1] X):
        self.c_ncvis.add_one(&X[0, 0], X.shape[0], X.shape[1])