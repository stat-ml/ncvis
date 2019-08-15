if [ $TRAVIS_OS_NAME = 'osx' ]; then
    conda_name="MacOSX"
else
    conda_name="Linux"
fi

curl "https://repo.continuum.io/miniconda/Miniconda3-latest-$conda_name-x86_64.sh" -o ~/miniconda.sh
bash ~/miniconda.sh -b -p $HOME/miniconda
export PATH="$HOME/miniconda/bin:$PATH"

conda update --yes conda
conda create --yes -n test python=$TRAVIS_PYTHON_VERSION
source activate test
conda install --yes conda-build