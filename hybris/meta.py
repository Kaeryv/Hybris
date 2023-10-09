from typing import AnyStr
from .profiling import profile_configurations

from functools import partial
from . import _lhybris
import numpy as np
from .optim import ParticleSwarm

from typing import AnyStr
from .problems import get_benchmark

import os

def rank_in_db(db, scores_mean):
    ranks = np.empty_like(scores_mean)
    # ranks.shape == (len(functions), len(configs))
    for i in range(scores_mean.shape[0]):
        db[i] = np.sort(db[i])
        ranks[i] = np.searchsorted(db[i], scores_mean[i]) / len(db[i])
    return ranks
def benchmark_rule(configurations, db, profiler_args, append2db=True):
    # Evaluate the configuration
    scores = profile_configurations(configurations, endresult=True, **profiler_args)
    # scores.shape = (len(functions), len(configs), nruns)
    scores_mean = scores.mean(axis=2)
    # scores_mean.shape = (14, len(configs))
    ranks = rank_in_db(db, scores_mean)
    if append2db:
        db = np.hstack((db, scores.reshape(scores.shape[0], -1)))
    
    # Dumps all the information.
    objective = np.mean(ranks, axis=0)
    #objective.shape = (len(configs),)
    return objective, ranks, scores_mean, db

def configure_mopt_membership(mopt, mask):
    fuzzy_params_ids = [i for i, x in enumerate(mask) if int(x) == 1 ]

    # Range for center and width
    centermin = [ 0.1, 0.1, 0.1, 0.1, -15, 0.1,  0.01 ]
    centermax = [ 0.9, 1.9, 1.9, 0.9,  -1, 1.0, 1.0 ]
    widthmin =  [ 0.2, 0.2, 0.2, 0.2,   1, 0.1,  0.05 ]
    widthmax =  [ 1.0, 2.0, 2.0, 1.0,  10, 1.0,  1.0 ]

    vmin = []
    vmax = []
    for param_id in fuzzy_params_ids:
        vmin.extend([centermin[param_id], widthmin[param_id]])
        vmax.extend([centermax[param_id], widthmax[param_id]])

    mopt.vmin = vmin
    mopt.vmax = vmax


def expandw(mask, x):
    x = np.asarray(x)
    hard_boundaries = [
        (0.0, 1.0), # w
        (0.0, 2.5), # c1
        (0.0, 2.5), # c2
        (0.0, 1.0), # h
        (-16, -1.), # l
        (0.0, 1.0), # L
        (0.0, 1.0)  # K
    ]
    nrules = mask.count("1")
    wcw = x.reshape(nrules, 2)
    j = 0
    ret = []
    for i, e in enumerate(list(mask)):
        if e == "1":
            c = wcw[j][0]
            w = wcw[j][1] / 2.0
            ret.extend([ max(hard_boundaries[i][0], c - w), c, min(c+w, hard_boundaries[i][1])])
            j += 1
    return ret 
def optimize_self(mask, seed=42, num_agents=10, max_fevals=2000, db="./db/warmup/full.npy", return_all=False, profiler_args={}):
    profiler_defaults = {"benchmark": "train", "nruns": 5, "max_workers": 8 }
    profiler_defaults.update(profiler_args)
    profiler_args = profiler_defaults
    bench = get_benchmark(profiler_args["benchmark"])["problems"]
    nfunctions = len(bench)
    if isinstance(db, str):
        db = np.load(db) if os.path.isfile(db) else -np.inf * np.ones((nfunctions,1))

    nrules = sum(map(int, mask))
    nq = _lhybris.fuzz_get_num_qualities_combinations() # Terms
    no = 8 # Sign
    na = 3 # Priority of operations
    nd = (2+8) * nrules # Total dimensions
    cont_dimensions = 2 * nrules
    categ_dimensions = 8 * nrules
    opt = ParticleSwarm(num_agents=num_agents, num_variables=[cont_dimensions, categ_dimensions], max_fevals=max_fevals)
    configure_mopt_membership(opt, mask)
    opt.num_categories([nq, no, nq, no, nq, na, na, na] * nrules)
    opt.reset(seed)
    opt.weights[0,:] = 0.3


    best_configuration = np.empty(nd, dtype=float)
    all_configurations = []
    all_objectives = []
    archive_functions_error = []
    best_objective = np.inf
    pop_scores = dict()
    while not opt.stop():
        X = opt.ask()
            
        configurations = [ (x[cont_dimensions:].astype(np.int32), mask, expandw(mask, x[:cont_dimensions])) for x in X ] 
        Y_objective, Y_ranks, Y_function_error, db = benchmark_rule(configurations, db, profiler_args, append2db=True)
        for i, x in enumerate(X):
            x_hash = hash(tuple(map(lambda a: int(500*a), x)))
            if not x_hash in pop_scores.keys():
                pop_scores[x_hash] = Y_function_error[:, i][:]
            else:
                print("[Warning] Evaluating same configuration")
        opt.tell(Y_objective)
        
        # Update memories as an objective based on rank changes!
        scores_reevaluation = np.asarray([ pop_scores[hash(tuple(map(lambda a: int(500*a), x)))] for x in opt.position_memories]).T
        ranks_reevaluation = rank_in_db(db, scores_reevaluation)
        opt.aptitude_memories[:] = np.mean(ranks_reevaluation, axis=0)

        all_configurations.append(X)
        all_objectives.append(Y_objective)
        archive_functions_error.append(Y_function_error)

        iteration_best_objective_idx = np.argmin(Y_objective)
        if np.all(Y_objective[iteration_best_objective_idx] < opt.aptitude_memories):
            best_objective = Y_objective[iteration_best_objective_idx]
            print(f"New best: {best_objective}")
            best_configuration[:] = X[iteration_best_objective_idx, :]

    if not return_all:
        return opt.profile, best_configuration
    else:
        return opt.profile, best_configuration, (all_configurations, all_objectives, archive_functions_error), db
