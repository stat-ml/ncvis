# distutils: language=c

from wrapper cimport cncvis
import numpy as np

def add_one(double[:, ::1] X):
    cncvis.add_one(&X[0, 0], X.shape[0], X.shape[1])