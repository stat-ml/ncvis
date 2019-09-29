%UserProfile%\miniconda.exe /InstallationType=JustMe /RegisterPython=0 /S /D=%UserProfile%\miniconda
call %UserProfile%\miniconda\Scripts\activate.bat %UserProfile%\miniconda
conda config --set always_yes yes --set anaconda_upload no

conda update -q conda
conda create -n test python=$TRAVIS_PYTHON_VERSION