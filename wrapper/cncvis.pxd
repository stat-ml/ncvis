cdef extern from "../src/ncvis.h":
    cdef cppclass NCVis:
        NCVis() except +