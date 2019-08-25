if [ $TRAVIS_OS_NAME = 'osx' ]; then
    conda_name="MacOSX"
else
    conda_name="Linux"
fi

curl "https://repo.continuum.io/miniconda/Miniconda3-latest-$conda_name-x86_64.sh" -o ~/miniconda.sh
bash ~/miniconda.sh -b -p $HOME/miniconda
export PATH="$HOME/miniconda/bin:$PATH"

# Upload to conda only if we are pushing master
if [ $TRAVIS_BRANCH = 'master' ] && [ $TRAVIS_EVENT_TYPE = 'push' ]; then
    conda config --set anaconda_upload yes
    conda config --set conda_build.anaconda_token $CONDA_UPLOAD_TOKEN
else
    conda config --set anaconda_upload no
fi
conda config --set always_yes yes

conda update -q conda
conda create -n test python=$TRAVIS_PYTHON_VERSION
#Using source as shell has not been properly configured to use 'conda activate'.
source activate test
conda install conda-build conda-verify anaconda-client