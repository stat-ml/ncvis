%UserProfile%\miniconda.exe /InstallationType=JustMe /RegisterPython=0 /S /D=%UserProfile%\miniconda
call %UserProfile%\miniconda\Scripts\activate.bat %UserProfile%\miniconda
call conda config --set always_yes yes --set anaconda_upload no
call conda update -q conda
call conda create -n test python=%TRAVIS_PYTHON_VERSION%