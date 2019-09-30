import numpy as np
import ncvis

X = np.random.random((5, 3))
distances=['euclidean', 'cosine', 'correlation', 'inner_product']
for distance in distances:
    vis = ncvis.NCVis(n_neighbors=15, M=16, ef_construction=200,
    n_init_epochs=20, n_epochs=50, min_dist=0.4, n_threads=-1, distance=distance)
    Y = vis.fit_transform(X)
    all_finite = np.all(np.isfinite(Y))
    if not all_finite: 
        print("Distance:", distance)
        print("Input:")
        print(X)
        print("Output:")
        print(Y)
        exit(1)