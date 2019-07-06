DIR=$(git rev-parse --show-toplevel)
DATA=$DIR/data

###############
mkdir -p $DATA/iris
cd $DATA/iris
wget -N http://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data
cd $DIR
###############

###############
mkdir -p $DATA/mnist
cd $DATA/mnist
wget -N http://yann.lecun.com/exdb/mnist/train-images-idx3-ubyte.gz
wget -N http://yann.lecun.com/exdb/mnist/train-labels-idx1-ubyte.gz
wget -N http://yann.lecun.com/exdb/mnist/t10k-images-idx3-ubyte.gz
wget -N	http://yann.lecun.com/exdb/mnist/t10k-labels-idx1-ubyte.gz
gzip -d *.gz
cd $DIR
###############