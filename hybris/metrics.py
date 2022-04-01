import numpy as np

def gaussian_proximity(candidates, archive, sigma):
    ''' Gaussian penalty for proximity to archive points '''
    nc = candidates.shape[0]
    nd = candidates.shape[1]
    na = archive.shape[0]

    assert nd == archive.shape[1]

    gp = np.empty((nc,))

    for i in range(na):
        dist = np.linalg.norm(candidates - archive[i], axis=1)
        gp += np.exp(-dist**2 / sigma**2)
    
    gp /= na

    return gp

def rank_ratio(candidates, archive):
    asort = np.argsort(archive.flatten())
    sorted_y = archive.flatten()[asort]
    rank_cost = np.searchsorted(sorted_y, candidates) / sorted_y.shape[0]
    return rank_cost
