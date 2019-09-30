bash ~/miniconda.sh -b -p $HOME/miniconda
export PATH="$HOME/miniconda/bin:$PATH"
conda config --set always_yes yes --set anaconda_upload no
conda update -q conda
conda create -n test python=$1