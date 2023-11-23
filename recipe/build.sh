# Make sure any error below is reported as such
set -v -e

$PYTHON -m pip install --no-deps --no-build-isolation . -vvv
