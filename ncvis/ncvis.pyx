# distutils: language = c++

from ncvis cimport NCVis

cdef class PyNCVis:
    cdef NCVis c_ncvis;