#Push to PyPI only once on master
if [ $TRAVIS_BRANCH = 'master' ] && [ $TRAVIS_EVENT_TYPE = 'push' ] && [ $TRAVIS_OS_NAME = 'linux' ]; then
    export PATH="$HOME/miniconda/bin:$PATH"
    #Using source as shell has not been properly configured to use 'conda activate'.
    source activate test

    conda install numpy cython anaconda-client twine

    #Upload to Anaconda Cloud
    CONDA_PKG=$(conda build . --output)
    anaconda -t $CONDA_UPLOAD_TOKEN upload $CONDA_PKG --force
    
    #Upload source distribution to PyPI only once (if we are on Linux)
    if [ $TRAVIS_OS_NAME = 'linux' ]; then
        #Install C++ libraries
        git clone https://github.com/nmslib/hnswlib.git lib/hnswlib
        git clone https://github.com/imneme/pcg-cpp lib/pcg-cpp 
        python setup.py sdist
        twine upload -u '__token__' -p $PYPI_UPLOAD_TOKEN dist/*
    fi
fi