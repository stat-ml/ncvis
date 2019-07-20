# distutils: language=c++

from wrapper cimport cncvis
import numpy as np
cimport numpy as cnp
import ctypes

from scipy.optimize import curve_fit
# https://github.com/lmcinnes/umap/blob/master/umap/umap_.py
def find_ab_params(spread=1., min_dist=0.1):
    """Fit a, b params for the differentiable curve used in lower
    dimensional fuzzy simplicial complex construction. We want the
    smooth curve (from a pre-defined family with simple gradient) that
    best matches an offset exponential decay.
    """
    def curve(x, a, b):
        return 1.0 / (1.0 + a * x ** (2 * b))

    xv = np.linspace(0, spread * 3, 300)
    yv = np.zeros(xv.shape)
    yv[xv < min_dist] = 1.0
    yv[xv >= min_dist] = np.exp(-(xv[xv >= min_dist] - min_dist) / spread)
    params, covar = curve_fit(curve, xv, yv)
    return params[0], params[1]

cdef class NCVis:
    cdef cncvis.NCVis* c_ncvis
    cdef size_t d

    def __cinit__(self, size_t d=2, size_t n_threads=1, size_t n_neighbors=30, size_t M = 16, size_t ef_construction = 200, size_t random_seed = 42, int n_epochs=30, int n_init_epochs=10, float spread=1., float min_dist=0.1, float alpha=1., float alpha_Q=1., object n_noise=<int>3):
        a, b = find_ab_params(spread, min_dist)
        cdef size_t[:] n_noise_arr
        if isinstance(n_noise, int):
            n_noise_arr = np.full(n_epochs, n_noise, dtype=np.uint)
        elif isinstance(n_noise, np.ndarray):
            if len(n_noise.shape) > 1:
                raise ValueError("Expected 1D n_noise array.")
            n_epochs = n_noise.shape[0]
            n_noise_arr = n_noise.astype(np.uint)
        self.c_ncvis = new cncvis.NCVis(d, n_threads, n_neighbors, M, ef_construction, random_seed, n_epochs, n_init_epochs, a, b, alpha, alpha_Q, &n_noise_arr[0])
        self.d = d

    def __dealloc__(self):
        del self.c_ncvis

    def fit(self, float[:, :] X):
        return np.asarray(<float[:X.shape[0], :self.d]>self.c_ncvis.fit(&X[0, 0], X.shape[0], X.shape[1]))