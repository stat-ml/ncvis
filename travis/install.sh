export PATH="$HOME/miniconda/bin:$PATH"
#Using source as shell has not been properly configured to use 'conda activate'.
source activate test

conda info
conda build .
#Push to PyPI only once on master
if [ $TRAVIS_BRANCH = 'master' ] && [ $TRAVIS_EVENT_TYPE = 'push' ] && [ $TRAVIS_OS_NAME = 'linux']; then
    python setup.py sdist
    twine upload -u '__token__' -p $PYPI_UPLOAD_TOKEN dist/*
fi