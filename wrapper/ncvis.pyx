# distutils: language=c++
# distutils: sources = src/ncvis.cpp

from wrapper cimport cncvis

cdef class NCVis:
    cdef cncvis.NCVis c_ncvis;