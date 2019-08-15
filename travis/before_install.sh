if [ $TRAVIS_OS_NAME = 'osx' ]; then
    brew update
    brew install wget
    conda_name="MacOSX"
else
    conda_name="Linux"
fi

wget https://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh -O ~/miniconda.sh
bash ~/miniconda.sh -b -p $HOME/miniconda
export PATH="$HOME/miniconda/bin:$PATH"

conda update --yes conda
conda create --yes -n test python=$TRAVIS_PYTHON_VERSION
conda activate test
conda install conda-build