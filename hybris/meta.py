from typing import AnyStr
from .profiling import profile_configurations

from functools import partial
from . import _lhybris
import numpy as np
from .optim import Optimizer

from typing import AnyStr
from .problems import get_benchmark

import os

def rank_in_db(db, scores_mean):
    ranks = np.empty_like(scores_mean)
    # ranks.shape = (14, len(configs))
    for i in range(scores_mean.shape[0]):
        ranks[i] = np.searchsorted(db[i], scores_mean[i]) / len(db[i])
    return ranks
def benchmark_rule(configurations, nruns, benchmark, max_workers, db, append2db=True, optimizer_args_update={}):
    # Evaluate the configuration
    scores = profile_configurations(configurations, nruns=nruns, benchmark=benchmark, endresult=True, max_workers=max_workers, optimizer_args_update=optimizer_args_update)
    # scores.shape = (14, len(configs), nruns)
    scores_mean = scores.mean(axis=2)
    # scores_mean.shape = (14, len(configs))
    ranks = rank_in_db(db, scores_mean)
    if append2db:
        db = np.hstack((db, scores.reshape(scores.shape[0], -1)))
    
    # Dumps all the information.
    objective = np.mean(ranks, axis=0)
    #objective.shape = (len(configs),)
    return objective, ranks, scores_mean, db

def optimize_self(mask, seed=42, num_agents=10, max_fevals=2000, db="./db/warmup/full.npy", return_all=False, profiler_args={}):
    profiler_defaults = {"benchmark": "train", "nruns": 5, "max_workers": 8 }
    profiler_defaults.update(profiler_args)
    profiler_args = profiler_defaults
    hard_boundaries = [
        (0.0, 1.0), # w
        (0.0, 2.5), # c1
        (0.0, 2.5), # c2
        (0.0, 1.0), # h
        (-16, 0.0), # l
        (0.1, 1.0), # L
        (0.0, 1.0)  # K
    ]
    bench = get_benchmark(profiler_args["benchmark"])["problems"]
    nfunctions = len(bench)
    if isinstance(db, str):
        if os.path.isfile(db):
            db = np.load(db)
        else:
            print("Creating new db")
            db = np.zeros((nfunctions,1))
    nrules = sum(map(int, mask))
    nq = _lhybris.fuzz_get_num_qualities_combinations() # Terms
    no = 8 # Sign
    na = 3 # Priority of operations
    nd = 10 * nrules # Total dimensions
    cont_dimensions = 2 * nrules
    categ_dimensions = 8 * nrules
    opt = Optimizer(num_agents=num_agents, num_variables=[cont_dimensions, categ_dimensions], max_fevals=max_fevals)
    opt.num_categories([nq, no, nq, no, nq, na, na, na] * nrules)
    opt.reset(seed)

    best_configuration = np.empty(nd, dtype=int)
    all_configurations = []
    all_objectives = []
    all_funcvals = []
    best_objective = np.inf
    pop_scores = dict()
    while not opt.stop():
        X = opt.ask()
        objective = []
        def expandw(x):
            wcw = x[:cont_dimensions].reshape(nrules, 2)
            j = 0
            ret = []
            for i, e in enumerate(list(mask)):
                if e == "1":
                    c = wcw[j][0]
                    w = wcw[j][1] / 2.0
                    ret.extend([ max(hard_boundaries[i][0], c - w), c, min(c+w, hard_boundaries[i][1])])
                    j += 1
            return ret 
            
        configurations = [ (x[cont_dimensions:].astype(np.int32), mask, expandw(x)) for x in X ] 
        Y_objective, Y_ranks, Y_scores, db = benchmark_rule(configurations, db=db, **profiler_args)

        for i, x in enumerate(X):
            pop_scores[hash(tuple(x))] = Y_scores[:, i][:, np.newaxis]
        opt.tell(Y_objective)
        
        # Update memories as an objective based on rank changes!
        for i, (x, y) in enumerate(zip(opt.position_memories, opt.aptitude_memories)):
            opt.aptitude_memories[i] = np.mean(rank_in_db(pop_scores[hash(tuple(x))], db))

        all_configurations.append(X)
        all_objectives.append(Y_objective)
        all_funcvals.append(Y_scores)

        iteration_best_objective_idx = np.argmin(Y_objective)
        if Y_objective[iteration_best_objective_idx] < best_objective:
            best_objective = Y_objective[iteration_best_objective_idx]
            print(f"New best: {best_objective}")
            best_configuration[:] = X[iteration_best_objective_idx, :]

    if not return_all:
        return opt.profile, best_configuration
    else:
        return opt.profile, best_configuration, (all_configurations, all_objectives, all_funcvals), db
