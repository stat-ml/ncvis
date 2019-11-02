export PATH="$HOME/miniconda/bin:$PATH"
#Using source as shell has not been properly configured to use 'conda activate'.
source activate test

conda install anaconda-client

#Upload to Anaconda Cloud
CONDA_PKG=$(conda build . --output)
anaconda -t conda_token.txt upload $CONDA_PKG --force

#Upload source distribution to PyPI only once (if we are on Linux)
if [ $TRAVIS_OS_NAME = 'linux' ] && [ $CONDA_PYTHON_VERSION='3.7' ]; then
    conda install numpy cython twine
    # Submodules are updated be default
    python setup.py sdist
    twine upload -u '__token__' -p $PYPI_UPLOAD_TOKEN dist/*
fi