from setuptools import setup
from setuptools.extension import Extension
from Cython.Build import cythonize
import numpy

extensions = [Extension("ncvis",
                        ["wrapper/*.pyx"],
                        extra_compile_args=["-O3", "-std=c++11"],
                        extra_link_args=['-lm'],
                        include_dirs=[numpy.get_include()])]
extensions = cythonize(extensions, language_level=3)
setup(ext_modules=extensions,
      install_requires=['wurlitzer'])