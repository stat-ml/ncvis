export PATH="$HOME/miniconda/bin:$PATH"
#Using source as shell has not been properly configured to use 'conda activate'.
source activate test

conda info
conda build .