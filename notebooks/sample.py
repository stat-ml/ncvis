#%%
import sys
import os
sys.path.append("../")

import numpy as np
import ncvis
%load_ext wurlitzer

#%%
a = np.random.uniform(0, 1, (10, 5)).astype(np.float32)
vis = ncvis.NCVis(2)
vis.fit(a)

#%%
import glob

#%%
src = glob.glob("src/*.cpp")
print(src)
src.remove('src/main.cpp')
src

#%%
