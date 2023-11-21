# Make sure any error below is reported as such
set -v -e

$PYTHON setup.py install --no-deps
