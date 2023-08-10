from typing import AnyStr
from .profiling import profile_configurations

from functools import partial
from . import _lhybris
import numpy as np
from .optim import Optimizer

from typing import AnyStr

def rank_in_db(db, scores_mean):
    ranks = np.empty_like(scores_mean)
    # ranks.shape = (14, len(configs))
    for i in range(scores_mean.shape[0]):
        ranks[i] = np.searchsorted(db[i], scores_mean[i]) / len(db[i])
    return ranks
def benchmark_rule(configurations, nruns, benchmark, max_workers, db, append2db=True, initial_weights=None):
    print(f"Benchmarking {len(configurations)} rules.")
    # Evaluate the configuration
    scores = profile_configurations(configurations, nruns=nruns, benchmark=benchmark, endresult=True, max_workers=max_workers, initial_weights=initial_weights)
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

def optimize_self(mask, seed=42, num_agents=10, max_fevals=2000, benchmark="train", db="./db/warmup/full.npy", return_all=False, max_workers=8, initial_weights=None):
    if isinstance(db, str):
        db = np.load(db)
    nrules = sum(map(int, mask))
    nq = _lhybris.fuzz_get_num_qualities_combinations()
    no = 8
    na = 3
    nd = 10 * nrules
    opt = Optimizer(num_agents=num_agents, num_variables=[2*nrules, 8*nrules], max_fevals=max_fevals)
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
            wcw = x[:2*nrules]
            weights = [  max(0, wcw[0] - wcw[1]/2.), wcw[0], min(wcw[0] + wcw[1]/2., 1.0)]
            return weights
        configurations = [ (x[2*nrules:].astype(np.int32), mask, expandw(x)) for x in X ] 
        Y_objective, Y_ranks, Y_scores, db = benchmark_rule(configurations, 5, benchmark=benchmark, max_workers=max_workers, db=db, initial_weights=initial_weights)
        for x in X:
            pop_scores[hash(tuple(x))] = Y_scores
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
