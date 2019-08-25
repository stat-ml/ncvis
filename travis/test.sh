export PATH="$HOME/miniconda/bin:$PATH"
#Using source as shell has not been properly configured to use 'conda activate'.
source activate test

CONDA_PKG=$(conda build . --output)
conda build -t $CONDA_PKG