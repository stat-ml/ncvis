call %UserProfile%\miniconda\Scripts\activate.bat %UserProfile%\miniconda

call conda activate test
call conda install anaconda-client

call conda build . --output
REM Upload to Anaconda Cloud
REM CONDA_PKG=$(conda build . --output)
REM anaconda -t $CONDA_UPLOAD_TOKEN upload $CONDA_PKG --force