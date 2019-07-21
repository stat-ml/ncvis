from setuptools import setup, find_packages
from setuptools.extension import Extension
import glob

try:
    from Cython.Build import cythonize
    import numpy
except ImportError:
    print("numpy and/or cython are not installed:")
    print("pip install numpy cython")
    exit(1)

__version__ = "0.0.0"
exec(open('wrapper/_version.py').read())
DISTNAME = 'ncvis'
VERSION = __version__
DESCRIPTION = 'Noise contrastive data visualization'
with open('README.md', 'r') as f:
    LONG_DESCRIPTION = f.read()
MAINTAINER = 'Aleksandr Artemenkov'
MAINTAINER_EMAIL = 'alartum@gmail.com'
URL = 'https://github.com/alartum/ncvis'
LICENSE = 'MIT'

#Add all sources except main
src = glob.glob('src/*.cpp')

extensions = [Extension("ncvis",
                        ["wrapper/*.pyx",
                        *src],
                        extra_compile_args=["-O3", "-std=c++11", "-fopenmp", "-fpic", "-ffast-math"],
                        libraries=['m', "gomp"],
                        include_dirs=[numpy.get_include()])]
extensions = cythonize(extensions, language_level=3)

metadata = dict(  name=DISTNAME,
                  maintainer=MAINTAINER,
                  maintainer_email=MAINTAINER_EMAIL,
                  description=DESCRIPTION,
                  license=LICENSE,
                  url=URL,
                  version=VERSION,
                  long_description=LONG_DESCRIPTION,
                  long_description_content_type="text/markdown",
                  classifiers=['Intended Audience :: Science/Research',
                              'Intended Audience :: Developers',
                              'License :: OSI Approved :: MIT License',
                              'Programming Language :: C++',
                              'Programming Language :: Python :: 3',
                              'Operating System :: OS Independent',
                              ],
                  install_requires=['scipy'],
                  python_requires=">=3")

setup(ext_modules=extensions,
      **metadata)