from setuptools import setup
from setuptools.extension import Extension
from Cython.Build import cythonize
import numpy
import glob

#Add all sources except main
src = glob.glob('src/*.cpp')
src.remove('src/main.cpp')

extensions = [Extension("ncvis",
                        ["wrapper/*.pyx",
                        *src],
                        extra_compile_args=["-O3", "-std=c++11", "-fopenmp", "-fpic", "-ffast-math"],
                        extra_link_args=['-lm', "-lgomp"],
                        include_dirs=[numpy.get_include()])]
extensions = cythonize(extensions, language_level=3)
setup(ext_modules=extensions,
      install_requires=['wurlitzer'])