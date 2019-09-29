if [ $TRAVIS_OS_NAME = 'osx' ]; then
    conda_name="MacOSX"
elif [ $TRAVIS_OS_NAME = 'linux' ]; then
    conda_name="Linux"
elif [ $TRAVIS_OS_NAME = 'windows' ]; then
    conda_name="Windows"
fi

if [ $TRAVIS_OS_NAME != 'windows' ]; then
    curl "https://repo.continuum.io/miniconda/Miniconda3-latest-$conda_name-x86_64.sh" -o ~/miniconda.sh
    bash ~/miniconda.sh -b -p $HOME/miniconda
    export PATH="$HOME/miniconda/bin:$PATH"
else
    echo $HOME
    travis/prepare.bat
    curl "https://repo.anaconda.com/miniconda/Miniconda3-latest-Windows-x86_64.exe"
    cmd "/C Miniconda4-latest-Windows-x86_64.exe /InstallationType=JustMe /RegisterPython=0 /S /D=%UserProfile%\Miniconda3"
fi

conda config --set always_yes yes --set anaconda_upload no

conda update -q conda
conda create -n test python=$TRAVIS_PYTHON_VERSION