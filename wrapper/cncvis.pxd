cdef extern from "../src/ncvis.cpp":
    pass

cdef extern from "../src/ncvis.h":
    cdef cppclass NCVis:
        NCVis() except +
        void add_one(double* X, int N, int D);