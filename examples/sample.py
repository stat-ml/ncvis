#%%
import sys
import os
# sys.path.append("../")

import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import ncvis
import umap
# To redirect stdout (C++ outputs) to notebook cells
%load_ext wurlitzer

#%%
import os
import struct

def load_mnist(path, kind='train'):
    """Load MNIST data from `path`"""
    labels_path = os.path.join(path, '%s-labels-idx1-ubyte' % kind)
    images_path = os.path.join(path, '%s-images-idx3-ubyte' % kind)
        
    with open(labels_path, 'rb') as lbpath:
        magic, n = struct.unpack('>II', lbpath.read(8))
        labels = np.fromfile(lbpath, dtype=np.uint8)

    with open(images_path, 'rb') as imgpath:
        magic, num, rows, cols = struct.unpack(">IIII", imgpath.read(16))
        images = np.fromfile(imgpath, dtype=np.uint8).reshape(len(labels), rows*cols)
 
    return images, labels

#%%
def plot_silhouette(xs, y, ax, marker='o'):
    from sklearn.metrics import silhouette_samples
    scores = silhouette_samples(xs, y)
    labels = np.unique(y)
    n_labels = labels.shape[0]
    macro_scores = np.empty(n_labels)
    for i in range(n_labels):
        macro_scores[i] = np.mean(scores[y == labels[i]])
    score = np.mean(macro_scores)
    std_score = np.std(macro_scores)

    props = dict(boxstyle='round', facecolor='grey', alpha=0.1, linewidth=0)
    info = 'Silhouette Coefficient = {:.3f}±{:.3f}'
    
    n_dims = xs.shape[-1]
    for k in np.unique(y):
        npoints = np.count_nonzero(y==k)
        x1 = x2 = None

        class_mask = (y==k)
        if n_dims == 2:
            x1 = xs[class_mask, 0]
            x2 = xs[class_mask, 1]
        elif n_dims == 1:
            x1 = xs[class_mask, 0]
            x2 = np.random.uniform(0, 1, npoints)
        ax.scatter(x1, x2, marker=marker, label="{}".format(k))
        
    ax.legend(loc='upper right')
    ax.text(0.01, 0.01, info.format(score, std_score), fontsize=14, bbox=props, 
            transform=ax.transAxes)

#%%
from sklearn.model_selection import train_test_split

X_mnist, y_mnist = load_mnist('examples/data/mnist/', kind='train')
X_mnist = X_mnist/255

X_small, _, y_small, _ = train_test_split(X_mnist, y_mnist, test_size=None, train_size=1e-1, stratify=y_mnist, random_state=42)

iris = pd.read_csv("examples/data/iris/iris.data", header=None, names=["sepal length", "sepal width", "petal length", "petal width", "class"])
X_iris = iris.iloc[:, :4].values
y_iris = iris["class"].astype("category").cat.codes.values

data = {'iris': (X_iris, y_iris),
        'mnist6k': (X_small, y_small),
        'mnist60k': (X_mnist, y_mnist),}

#%%
%%time
# X, y = data['iris']
X, y = data['mnist6k']

vis = ncvis.NCVis()
Y = vis.fit(X)

#%%
fig, ax = plt.subplots(1, 1)
plot_silhouette(Y, y, ax, marker='.')
plt.show()

#%%
%%time
X, y = data['mnist6k']
embedding = umap.UMAP(n_neighbors=15,
                      min_dist=0.5,
                      init='spectral',
#                       metric='correlation',
                      metric='euclidean',
                      negative_sample_rate=5,
                      n_epochs=30).fit_transform(X)
#%%
fig, ax = plt.subplots(1, 1)
plot_silhouette(embedding, y, ax, marker='.')
plt.show()

#%%
print(embedding.shape)

#%%
%load_ext wurlitzer
import os
import subprocess
from locale import getpreferredencoding

func = 'call'
popenargs = ('python', '/home/alartum/git/ncvis/setup.py', 'build_ext', '--inplace')
kwargs = {}
codec = getpreferredencoding() or 'utf-8'

if 'env' not in kwargs:
    kwargs = kwargs.copy()
    env_copy = os.environ.copy()
    kwargs.update({'env': env_copy})
kwargs['env'] = {str(key): str(value) for key, value in kwargs['env'].items()}
_args = []
import sys
print(sys.path)
print(kwargs['env']['PATH'])
kwargs['env']['PATH'] = '/home/alartum/miniconda3/bin:/home/alartum/miniconda3/envs/ncvis-test/bin:/home/alartum/miniconda3/condabin:/home/alartum/.local/bin:/usr/local/bin:/home/alartum/.vscode-server/bin/2213894ea0415ee8c85c5eea0d0ff81ecc191529/bin:/home/alartum/.local/bin:/usr/local/bin:/home/alartum/.vscode-server/bin/2213894ea0415ee8c85c5eea0d0ff81ecc191529/bin:/usr/local/sbin:/usr/local/bin:/usr/bin:/opt/cuda/bin:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl'
if 'stdin' not in kwargs:
    kwargs['stdin'] = subprocess.PIPE
for arg in popenargs:
    # arguments to subprocess need to be bytestrings
    if sys.version_info.major < 3 and hasattr(arg, 'encode'):
        arg = arg.encode(codec)
    elif sys.version_info.major >= 3 and hasattr(arg, 'decode'):
        arg = arg.decode(codec)
    _args.append(str(arg))

stats = kwargs.get('stats')
if 'stats' in kwargs:
    del kwargs['stats']

rewrite_stdout_env = kwargs.pop('rewrite_stdout_env', None)
if rewrite_stdout_env:
    kwargs['stdout'] = _setup_rewrite_pipe(rewrite_stdout_env)

out = None
if stats is not None:
    proc = PopenWrapper(_args, **kwargs)
    if func == 'output':
        out = proc.out.read()

    if proc.returncode != 0:
        raise subprocess.CalledProcessError(proc.returncode, _args)

    stats.update({'elapsed': proc.elapsed,
                'disk': proc.disk,
                'processes': proc.processes,
                'cpu_user': proc.cpu_user,
                'cpu_sys': proc.cpu_sys,
                'rss': proc.rss,
                'vms': proc.vms})
else:
    if func == 'call':
        _args = ['python', '-cimport sys; print(sys.executable, sys.path)']
        p = subprocess.run(_args, **kwargs)
        print(p)
    else:
        if 'stdout' in kwargs:
            del kwargs['stdout']
        out = subprocess.check_output(_args, **kwargs)

#%%

import subprocess

subprocess.run(["python", '1'])

#%%
