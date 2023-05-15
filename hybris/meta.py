from typing import AnyStr
from .profiling import profile_configurations

from functools import partial
from . import _lhybris
import numpy as np
from .optim import Optimizer

from typing import AnyStr

def benchmark_rule(X, mask: AnyStr, nruns, benchmark, max_workers, db):
    X = list(map(int, X))

    # Evaluate the configuration
    scores = profile_configurations([(X, mask)], nruns=nruns, benchmark=benchmark, endresult=True, max_workers=max_workers)
    scores_mean = scores[:, 0, :].mean(axis=1)
    
    # Compute the ranks
    ranks = np.empty_like(scores_mean)
    for i in range(len(ranks)):
        ranks[i] = np.searchsorted(db[i][:], scores_mean[i]) / len(db[i])
    
    # Dumps all the information.

    objective = np.mean(ranks)
    return objective, ranks, scores_mean

def optimize_self(mask, seed=42, num_agents=10, max_fevals=2000, benchmark="train", db="./db/warmup/full.npy", return_all=False):
    if isinstance(db, str):
        db = np.load(db)
    nrules = sum(map(int, mask))
    nq = _lhybris.fuzz_get_num_qualities_combinations()
    no = 8
    na = 3
    nd = 8 * nrules
    opt = Optimizer(num_agents=num_agents, num_variables=[0, 8*nrules], max_fevals=max_fevals)
    opt.num_categories([nq, no, nq, no, nq, na, na, na] * nrules)
    opt.reset(seed)

    best_configuration = np.empty(nd, dtype=int)
    all_configurations = []
    all_objectives = []
    all_funcvals = []
    best_objective = np.inf
    while not opt.stop():
        X = opt.ask()
        objective = []
        for x in X:    
            Y_objective, Y_ranks, Y_scores = benchmark_rule(x, mask, 5, benchmark=benchmark, max_workers=8, db=db)
            objective.append(Y_objective)
        opt.tell(objective)

        all_configurations.append(X)
        all_objectives.append(objective)
        all_funcvals.append(Y_objective)

        iteration_best_objective_idx = np.argmin(objective)
        if objective[iteration_best_objective_idx] < best_objective:
            best_objective = objective[iteration_best_objective_idx]
            best_configuration[:] = X[iteration_best_objective_idx, :]

    if not return_all:
        return opt.profile, best_configuration
    else:
        return opt.profile, best_configuration, (all_configurations, all_objectives, all_funcvals)
