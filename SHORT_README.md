[![Conda](https://anaconda.org/alartum/ncvis/badges/version.svg)](https://anaconda.org/alartum/ncvis)
[![PyPI](https://img.shields.io/pypi/v/ncvis.svg)](https://pypi.python.org/pypi/ncvis/)
[![GitHub](https://img.shields.io/github/license/alartum/ncvis.svg)](https://github.com/alartum/ncvis/blob/master/LICENSE)
[![Build Status](https://dev.azure.com/stat-ml/ncvis/_apis/build/status/stat-ml.ncvis?branchName=master)](https://dev.azure.com/stat-ml/ncvis/_build/latest?definitionId=1&branchName=master)

# ncvis

**NCVis** is an efficient solution for data visualization and dimensionality reduction. It uses [HNSW](https://github.com/nmslib/hnswlib) to quickly construct the nearest neighbors graph and a parallel (batched) approach to build its embedding. Efficient random sampling is achieved via [PCGRandom](https://github.com/imneme/pcg-cpp). Detailed application examples can be found [here](https://github.com/alartum/ncvis-examples).

# Using

```python
import ncvis

vis = ncvis.NCVis()
Y = vis.fit_transform(X)
```

More detailed examples can be found [here](https://github.com/alartum/ncvis-examples).

# Installation

## Conda [recommended]

You do not need to setup the environment if using *conda*, all dependencies are installed automatically. 
```bash
$ conda install alartum::ncvis 
```

## Pip [not recommended]

**Important**: be sure to have a compiler with *OpenMP* support. *GCC* has it by default, which is not the case with *clang*. You may need to install *llvm-openmp* library beforehand.  

1. Install **numpy** and **cython** packages (compile-time dependencies):
    ```bash
    $ pip install numpy cython
    ```
2. Install **ncvis** package:
    ```bash
    $ pip install ncvis
    ```

## From source [not recommended]

**Important**: be sure to have *OpenMP* available.

First of all, download the *pcg-cpp* and *hnswlib* libraries:
```bash
$ make libs
``` 
### Python Wrapper 

If *conda* environment is used, it replaces library search paths. To prevent compilation errors, you either need to use compilers provided by *conda* or switch to *pip*  and system compilers. 

* Conda
    ```bash
    $ conda install conda-build numpy cython scipy
    $ conda install -c conda-forge cxx-compiler c-compiler
    $ conda-develop -bc .
    ``` 

* Pip
    ```bash
    $ pip install numpy cython
    $ make wrapper
    ```

You can then use *pytest* to run some basic checks
```bash
$ pytest -v recipe/test.py
```


### C++ Binary

* Release
    ```bash
    $ make ncvis
    ```

* Debug
    ```bash
    $ make debug
    ```