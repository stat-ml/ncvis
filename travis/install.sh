export PATH="$HOME/miniconda/bin:$PATH"
#Using source as shell has not been properly configured to use 'conda activate'.
source activate test

conda info
conda build .
#Push to PyPI only once on master
if [ $TRAVIS_BRANCH = 'master' ] && [ $TRAVIS_EVENT_TYPE = 'push' ] && [ $TRAVIS_OS_NAME = 'linux' ]; then
    #Install conda dependencies
    conda install numpy cython twine
    #Install C++ libraries
    git clone https://github.com/nmslib/hnswlib.git $(LibDir)hnswlib
	git clone https://github.com/imneme/pcg-cpp $(LibDir)pcg-cpp 
    python setup.py sdist
    twine upload -u '__token__' -p $PYPI_UPLOAD_TOKEN dist/*
fi