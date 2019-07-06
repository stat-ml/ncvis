cdef extern from "../src/ncvis.c":
    pass

cdef extern from "../src/ncvis.h":
    cdef void add_one(double* X, int N, int d);