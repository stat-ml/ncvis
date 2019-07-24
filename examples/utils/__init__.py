import numpy as np
import matplotlib.pyplot as plt

def scatter_classes(xs, y, ax, marker='o', silhouette_coefficient=False):
    if silhouette_coefficient:
        from sklearn.metrics import silhouette_samples
        scores = silhouette_samples(xs, y)
        labels = np.unique(y)
        n_labels = labels.shape[0]
        macro_scores = np.empty(n_labels)
        for i in range(n_labels):
            macro_scores[i] = np.mean(scores[y == labels[i]])
        score = np.mean(macro_scores)
        std_score = np.std(macro_scores)

        props = dict(boxstyle='round', facecolor='grey', alpha=0.1, linewidth=0)
        info = 'Silhouette Coefficient = {:.3f}±{:.3f}'
    
    n_dims = xs.shape[-1]
    for k in np.unique(y):
        npoints = np.count_nonzero(y==k)
        x1 = x2 = None

        class_mask = (y==k)
        if n_dims == 2:
            x1 = xs[class_mask, 0]
            x2 = xs[class_mask, 1]
        elif n_dims == 1:
            x1 = xs[class_mask, 0]
            x2 = np.random.uniform(0, 1, npoints)
        ax.scatter(x1, x2, marker=marker, label="{}".format(k))
        
    if len(np.unique(y)) <= 10:
        ax.legend(loc='upper right')
    if silhouette_coefficient:
        ax.text(0.01, 0.01, info.format(score, std_score), fontsize=14, bbox=props, transform=ax.transAxes)