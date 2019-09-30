call %UserProfile%\miniconda\Scripts\activate.bat %UserProfile%\miniconda

call conda activate test
call conda install anaconda-client

for /f %%i in ("conda build . --output") do set CONDA_PKG=%%i
REM Upload to Anaconda Cloud
call anaconda -t %CONDA_UPLOAD_TOKEN% upload %CONDA_PKG% --force