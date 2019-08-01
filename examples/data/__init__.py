import os
import struct
import numpy as np
import pandas as pd

from abc import ABC, abstractmethod
class Dataset(ABC):
    """
    Abstract data interface class. 
    """
    @abstractmethod
    def __init__(self):
        """
        Must define:
            self.X     -- numpy array of data samples; 
            self.y     -- numpy array of labels;
            self.names -- dictionary of names for each value of label 
            self.shape -- shape of the raw data
        """
        super().__init__()
        self.X = None
        self.y = None
        self.names = {}
        self.shape = None

    def __getitem__(self, id):
        """
        Returns:
        X, y  -- data sample and label by given index
        """
        return self.X[id], self.y[id]

    def X(self):
        """
        Returns:
        X  -- data samples
        """
        return self.X

    def y(self):
        """
        Returns:
        y  -- data labels
        """
        return self.y

    def __len__(self):
        """
        Returns:
        n -- number of samples in the dataset
        """
        if self.X.shape[0] != self.y.shape[0]:
            raise RuntimeError("Data samples and labels sizes differ {} and {}, but must be the same".format(self.X.shape[0], self.y.shape[0]))
        return self.X.shape[0]

from multiprocessing import Process, Pool, Queue
import time
import progressbar
class LargePool:
    """
    Multiprocessing with progressbar.
    """
    def __init__(self, tasks, worker_class, worker_args=(), worker_kwargs={}, message='Loading '):
        self.tasks = tasks
        self.worker_class = worker_class
        self.worker_args = worker_args
        self.worker_kwargs = worker_kwargs
        self.message = message
        
    def run(self, processes=None, progress=True, delay=0.2):
        tasks = Queue()
        size = len(self.tasks)
        results = Queue(maxsize=size)
        
        def init():
            worker = self.worker_class(*self.worker_args, **self.worker_kwargs)
            while True:
                t = tasks.get()
                results.put(worker(t))
        
        def load_queue():
            for t in self.tasks:
                tasks.put(t)
        p = Process(target=load_queue)
        p.start()
          
        pool = Pool(processes=processes, initializer=init)
        if progress:
            with progressbar.ProgressBar(max_value=size, prefix=self.message) as bar:
                while not results.full():
                    bar.update(results.qsize())
                    time.sleep(delay)
        
        res = [results.get() for i in range(size)]
        
        p.terminate()
        pool.terminate()
        return [r for r in res if r is not None]
    
class Worker(ABC):
    @abstractmethod
    def __init__(self):
        super().__init__()
        pass
    @abstractmethod
    def __call__(self, task):
        pass

def load_mnist_raw(path, kind):
    """
    Load image/labels data packed as http://yann.lecun.com/exdb/mnist/.
    
    Arguments:
    path   -- path to the loaded file
    kind   -- kind of the file contents:
              'l' = labels
              'i' = images
            
    Returns:
    data   -- loaded data as numpy array
    """
    with open(path, 'rb') as f:
        if kind == 'l':
            magic, n = struct.unpack('>ii', f.read(8))
            data = np.fromfile(f, dtype=np.uint8)
        elif kind == 'i':
            magic, num, rows, cols = struct.unpack(">iiii", f.read(16))
            data = np.fromfile(f, dtype=np.uint8).reshape(num, rows*cols)
        else:
            raise RuntimeError("Unsupported file contents kind: '{}'".format(kind))

    return data

def load_mnist_like(folder='mnist'):
    """
    Load MNIST(F-MNIST) dataset.

    Returns:
    X, y -- data points and labels
    """
    train = {'i': 'data/{}/train-images-idx3-ubyte'.format(folder),
             'l': 'data/{}/train-labels-idx1-ubyte'.format(folder)}
    test  = {'i': 'data/{}/t10k-images-idx3-ubyte'.format(folder),
             'l': 'data/{}/t10k-labels-idx1-ubyte'.format(folder)}
    files = [train, test]
    
    storage = {'i': None,
               'l': None}
    for f in files:
        for kind in storage:
            arr = load_mnist_raw(f[kind], kind)
            if storage[kind] is None:
                storage[kind] = arr
            else:
                storage[kind] = np.concatenate((storage[kind], arr))

    return storage['i'], storage['l'] 

class MNIST(Dataset):
    """
    MNIST Dataset
    Alias: mnist
    http://yann.lecun.com/exdb/mnist/
    """
    def __init__(self):
        super().__init__()
        self.X, self.y = load_mnist_like('mnist')
        self.names = {k:str(k) for k in range(self.y.max())}
        self.shape = (28, 28)

class FMNIST(Dataset):
    """
    Fashion MNIST Dataset
    Alias: fmnist
    https://github.com/zalandoresearch/fashion-mnist
    """
    def __init__(self):
        super().__init__()
        self.X, self.y = load_mnist_like('fmnist')
        self.names = {
            0: "T-shirt/top",
            1: "Trouser",
            2: "Pullover",
            3: "Dress",
            4: "Coat",
            5: "Sandal",
            6: "Shirt",
            7: "Sneaker",
            8: "Bag",
            9: "Ankle boot"
        }
        self.shape = (28, 28)

class Iris(Dataset):
    """
    Iris Dataset
    https://archive.ics.uci.edu/ml/datasets/Iris
    """
    def __init__(self):
        super().__init__()        
        df = pd.read_csv("data/iris/iris.data", header=None)
        self.X = df.iloc[:, :-1].values
        classes = df.iloc[:, -1].astype("category").cat
        self.y = classes.codes.values
        self.names = dict(enumerate(classes.categories))
        for k in self.names:
            self.names[k] = self.names[k].rsplit('-', 1)[1].title()
        self.shape = (self.X.shape[1], )

import re
import imageio
class CoilLoader(Worker):
    def __init__(self, path):
        super().__init__()
        self.pattern = re.compile(r'obj(\d+)__(\d+).png')
        self.path = path
    
    def __call__(self, file):
        match = self.pattern.match(file)
        if match:
            obj = match.group(1)
            res = imageio.imread(os.path.join(self.path, file)).ravel(), int(obj)-1
            return res

def load_coil_like(path):
    """
    Load COIL-20(COIL-100) dataset.

    Returns:
    X, y -- data points and labels
    """
    for _, _, f in os.walk(path):
        fs = f
        break

    p = LargePool(fs, CoilLoader, (path,))
    res = p.run()

    X = []
    y = []
    for r in res:
        X.append(r[0])
        y.append(r[1])

    return np.stack(X), np.stack(y)

class COIL20(Dataset):
    """
    COIL-20 Dataset
    Alias: coil20
    http://www.cs.columbia.edu/CAVE/software/softlib/coil-20.php
    """
    def __init__(self):
        super().__init__()        

        self.X, self.y = load_coil_like('data/coil20/coil-20-proc')
        self.names = {k:'Object ' + str(k) for k in range(self.y.max())}
        self.shape = (128, 128)

class COIL100(Dataset):
    """
    COIL-100 Dataset
    Alias: coil100
    http://www1.cs.columbia.edu/CAVE/software/softlib/coil-100.php
    """
    def __init__(self):
        super().__init__()        

        self.X, self.y = load_coil_like('data/coil100/coil-100')
        self.names = {k:'Object ' + str(k) for k in range(self.y.max())}
        self.shape = (128, 128, 3)

class PenDigits(Dataset):
    """
    Pen Digits Dataset
    Alias: pendigits
    https://archive.ics.uci.edu/ml/datasets/optical+recognition+of+handwritten+digits
    """
    def __init__(self):
        super().__init__()
        files = ["data/pendigits/optdigits.tes",
                 "data/pendigits/optdigits.tra"]

        loaded = [None]*2
        for f in files:
            df = pd.read_csv(f, header=None)
            for i in range(2):
                if i == 0:
                    new = df.iloc[:, :-1].values
                else:
                    new = df.iloc[:, -1].values
                if loaded[i] is None:
                    loaded[i] = new
                else:
                    loaded[i] = np.concatenate((loaded[i], new))
        self.X, self.y = loaded
            
        self.names = {k:str(k) for k in range(self.y.max())}
        self.shape = (8, 8)

from io import StringIO
class CsvLoader(Worker):
    def __init__(self, sep='\t'):
        super().__init__()
        self.sep = sep
        
    def __call__(self, text):
        csv = StringIO(text)
        return pd.read_csv(csv, sep=self.sep, header=None, engine='c')
    
class CsvReader:
    def __init__(self, path, nlines, chunksize=1024):
        super().__init__()
        self.nlines = nlines
        self.chunksize = chunksize
        self.path = path
    
    def __iter__(self):
        nlines = 0
        nread = 0
        text = ''
        with open(self.path) as f:
            f.readline()
            for line in f:
                if nlines == self.nlines:
                    break
                nlines += 1
                nread += 1
                text += line + '\n'
                if nread == self.chunksize:
                    yield text
                    nread = 0
                    text = ''
            yield text
    
    def __len__(self):
        return (self.nlines+self.chunksize-1)//self.chunksize

class ScRNA(Dataset):
    """
    Mouse scRNA-seq Dataset
    Alias: scrna
    https://hemberg-lab.github.io/scRNA.seq.datasets/mouse/brain/
    """
    def __init__(self):
        super().__init__()

        # Load labels
        df = pd.read_csv('data/scrna/GSE93374_cell_metadata.txt', sep='\t')
        classes = df.iloc[:, 6].astype('category').cat
        name_to_class = dict(zip(df.iloc[:, 0], classes.codes.values))
        self.names = dict(enumerate(classes.categories))
        df = pd.read_csv('data/scrna/GSE93374_Merged_all_020816_DGE.txt', sep='\t', nrows=1)
        ind_to_name = df.columns.values
        self.y = np.empty(len(ind_to_name), dtype=np.int)
        for i in range(len(ind_to_name)):
            self.y[i] = name_to_class[ind_to_name[i]]

        # Load the data itself
        path = 'data/scrna/GSE93374_Merged_all_020816_DGE.txt'
        nlines = 26774
        reader = CsvReader(path, nlines=nlines, chunksize=1024)
        p = LargePool(reader, CsvLoader, ('\t',))
        df = pd.concat(p.run())
        self.X = df.iloc[:, 1:].values.T

        self.shape = (nlines,)

class Shuttle(Dataset):
    """
    Statlog (Shuttle) Dataset
    Alias: shuttle
    https://archive.ics.uci.edu/ml/datasets/Statlog+(Shuttle)
    """
    def __init__(self, drop_time=True):
        super().__init__()

        base = "data/shuttle/shuttle."
        exts = ["trn", "tst"]

        vals = {'X': None, 
                'y': None}
        for ext in exts:
            df = pd.read_csv(base+ext, sep=' ').values
            new = {'X': df[:, 1:-1] if drop_time else df[:, :-1],
                   'y': df[:,  -1]}
            for k in new:
                if vals[k] is None:
                    vals[k] = new[k]
                else:
                    vals[k] = np.concatenate((vals[k], new[k]))

        self.X, self.y = vals['X'], vals['y']
        self.names = {
                    1: 'Rad Flow', 
                    2: 'Fpv Close', 
                    3: 'Fpv Open', 
                    4: 'High', 
                    5: 'Bypass', 
                    6: 'Bpv Close', 
                    7: 'Bpv Open'
                }
        self.shape = (self.X.shape[1],)

