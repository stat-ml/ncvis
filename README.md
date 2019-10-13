[![Conda](https://anaconda.org/alartum/ncvis/badges/version.svg)](https://anaconda.org/alartum/ncvis)
[![PyPI](https://img.shields.io/pypi/v/ncvis.svg)](https://pypi.python.org/pypi/ncvis/)
[![GitHub](https://img.shields.io/github/license/alartum/ncvis.svg)](https://github.com/alartum/ncvis/blob/master/LICENSE)
[![Build Status](https://travis-ci.com/alartum/ncvis.svg?branch=master)](https://travis-ci.com/alartum/ncvis)
# ncvis

**NCVis** is an efficient solution for data visualization. It uses [HNSW](https://github.com/nmslib/hnswlib) for fast nearest neighbors graph construction and a parallel approach for building the graph embedding.

# Installation

## Conda [recommended]

You do not need to setup the environment if using *conda*, all dependencies are installed automatically. 
```bash
$ conda install -c alartum ncvis 
```

## Pip

**Important**: be sure to have a compiler with *OpenMP* support. *GCC* has it by default, wich is not the case with *clang*. You may need to install *llvm-openmp* library beforehand.  

1. Install **numpy** and **cython** packages (compile-time dependencies):
    ```bash
    $ pip install numpy cython
    ```
2. Install **ncvis** package:
    ```bash
    $ pip install ncvis
    ```

## From source

1. Install **numpy** and **cython** packages (compile-time dependencies):
    ```bash
    $ pip install numpy cython
    ```
2. Use Makefile, it will call *pip* for you
    ```bash
    $ make wrapper
    ```
# Using

```python
import ncvis

vis = ncvis.NCVis()
Y = vis.fit_transform(X)
```

A more detailed example can be found [here](https://github.com/alartum/ncvis-examples).