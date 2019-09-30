call %UserProfile%\miniconda\Scripts\activate.bat %UserProfile%\miniconda
call conda activate test

call conda install conda-build conda-verify
call conda info