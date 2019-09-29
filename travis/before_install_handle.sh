if [ $TRAVIS_OS_NAME = 'osx' ]; then
    conda_name="MacOSX"
    conda_ext="sh"
elif [ $TRAVIS_OS_NAME = 'linux' ]; then
    conda_name="Linux"
    conda_ext="sh"
elif [ $TRAVIS_OS_NAME = 'windows' ]; then
    conda_name="Windows"
    conda_ext="exe"
fi

curl "https://repo.anaconda.com/miniconda/Miniconda3-latest-$conda_name-x86_64.$conda_ext" -o ~/miniconda.$conda_ext
if [ $TRAVIS_OS_NAME = 'windows' ]; then
    travis/before_install.cmd
else
    travis/before_install.sh
fi