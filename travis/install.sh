export PATH="$HOME/miniconda/bin:$PATH"
source activate test

conda info
conda build . 