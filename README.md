[![PyPI](https://img.shields.io/pypi/v/ncvis.svg)](https://pypi.python.org/pypi/ncvis/)
![GitHub](https://img.shields.io/github/license/alartum/ncvis.svg)
# ncvis

**NCVis** is an efficient solution for data visualization. It uses [HNSW](https://github.com/nmslib/hnswlib) for fast nearest neighbors graph construction and a parallel approach for building the graph embedding.

# Installation

Install **numpy** and **cython** packages (compile-time dependencies):
```bash
$ pip install numpy cython
```

Install **ncvis** package:
```bash
$ pip install ncvis
```

# Using

```python
import ncvis

vis = ncvis.NCVis()
Y = vis.fit(X)
```

A more detailed example can be found [here](https://nbviewer.jupyter.org/github/alartum/ncvis/blob/master/examples/benchmark.ipynb).

# Experiments

Datasets can be dowloaded by using the *download.sh* script:
```bash
$ bash examples/data/download.sh <dataset>
```
Replace *\<dataset\>* with corresponding entry from the table. You can also download all of them at once:
```bash
$ bash examples/data/download.sh
```

 The datasets can be then accessed by using interfaces from the *data* Python module. Make sure that these packages are installed:
 ```bash
 $ pip install -r examples/requirements-pip.txt
 ```
 or 
  ```bash
 $ conda install --file examples/requirements-conda.txt
 ```

|Dataset| \<dataset\> | Dataset Class|
|-------|:-----------:|:------:|
|[MNIST](http://yann.lecun.com/exdb/mnist/)|mnist| MNIST|
|[Fashion MNIST](https://github.com/zalandoresearch/fashion-mnist)|fmnist| FMNIST|
|[Iris](https://archive.ics.uci.edu/ml/datasets/Iris)|iris|Iris|
|[Handwritten Digits](https://archive.ics.uci.edu/ml/datasets/optical+recognition+of+handwritten+digits)|pendigits|PenDigits|
|[COIL-20](http://www.cs.columbia.edu/CAVE/software/softlib/coil-20.php)|coil20|COIL20|
|[COIL-100](http://www1.cs.columbia.edu/CAVE/software/softlib/coil-100.php)|coil100|COIL100|
|[Mouse scRNA-seq](https://hemberg-lab.github.io/scRNA.seq.datasets/mouse/brain/)|scrna|ScRNA|
|[Statlog (Shuttle)](https://archive.ics.uci.edu/ml/datasets/Statlog+(Shuttle))|shuttle|Shuttle|
|[Flow Cytometry](https://flowrepository.org/id/FR-FCM-ZZ36)|flow|*not yet*|
|[GoogleNews](https://code.google.com/archive/p/word2vec/)|news|*not yet*|

Each dataset can be used in the following way:

|Sample Code | Action |
|-----|--------|
|```d = data.MNIST()```| Load the dataset.|
|```ds.X```| Get the samples as numpy array of shape *(n_samples, n_dimensions)*. If samples have more than one dimension they are all flattened.|
|```ds.y```| Get the labels of the samples.|
|```len(ds)```| Get total number of samples.|
|```ds[0]```| Get 0-th pair *(sample, label)* from the dataset.|
|```ds.shape```| Get the original shape of the samples. For example, it equals to *(28, 28)* for MNIST. |