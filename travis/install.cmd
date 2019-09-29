call %UserProfile%\miniconda\Scripts\activate.bat %UserProfile%\miniconda

conda activate test
conda install conda-build conda-verify
conda info