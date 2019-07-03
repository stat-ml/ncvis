#%%
import sys
import os
sys.path.append("../")

import numpy as np
import ncvis
%load_ext wurlitzer

#%%
a = np.ones((2, 2))
vis = ncvis.NCVis(a)

#%%
