cdef extern from "../src/ncvis.cpp":
    pass

cdef extern from "../src/ncvis.h" namespace "ncvis":
    cdef cppclass NCVis:
        NCVis(size_t) except +
        void add_one(double* X, int N, int D);
