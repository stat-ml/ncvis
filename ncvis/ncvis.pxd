cdef extern from "src/ncvis.cpp":
    pass

cdef extern from "src/ncvis.h":
    cdef cppclass NCVis:
        NCVis() except +