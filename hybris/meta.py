from typing import AnyStr
from .profiling import profile_configurations

from functools import partial
from . import _lhybris
import numpy as np
from .optim import ParticleSwarm, RuleSet, OptimizerFactory

from typing import AnyStr
from .problems import Benchmark
from types import SimpleNamespace

from tqdm import tqdm

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

def optimize_self(mask, seed=42, optimize_membership=False, db="./db/warmup/full.npy", return_all=False, profiler_args={}, metaopt_args={}):
    profiler_defaults = {"benchmark": "train", "nruns": 5, "max_workers": 8 }
    profiler_defaults.update(profiler_args)
    profiler_args = profiler_defaults
    del profiler_defaults
    
    metaopt_defaults = {"num_agents": 10, "max_fevals": 2000}
    metaopt_defaults.update(metaopt_args)
    metaopt_args = metaopt_defaults
    del metaopt_defaults

    bench = Benchmark.get(profiler_args["benchmark"]).problems
    nfunctions = len(bench)
    if isinstance(db, str):
        db = np.load(db) if os.path.isfile(db) else -np.inf * np.ones((nfunctions,1))

    nrules = sum(map(int, mask))
    nq = _lhybris.fuzz_get_num_qualities_combinations() # Terms
    no = 8 # Sign
    na = 3 # Priority of operations
    
    cont_dimensions = 2 * nrules if optimize_membership else 0
    categ_dimensions = 8 * nrules
    opt = OptimizerFactory.create(**metaopt_args, num_variables=[cont_dimensions, categ_dimensions])
    if optimize_membership:
        configure_mopt_membership(opt, mask)
    opt.num_categories([nq, no, nq, no, nq, na, na, na] * nrules)
    opt.reset(seed)
    opt.weights[0,:] = 0.3


    best_configuration = None
    best_configuration_objective = np.inf
    all_configurations = []
    all_objectives = []
    archive_functions_error = []
    pop_scores = dict()
    iteration = 0

    with tqdm(total=opt.max_iterations) as pbar:
        while not opt.stop():
            X = opt.ask()
                
            configurations = [ RuleSet(mask, x, nrules, optimize_membership) for x in X ] 
            Y_objective, Y_ranks, Y_function_error, db = benchmark_rule(configurations, db, profiler_args, append2db=True)
            for i, x in enumerate(X):
                x_hash = hash(tuple(map(lambda a: int(500*a), x)))
                if not x_hash in pop_scores.keys():
                    pop_scores[x_hash] = Y_function_error[:, i][:]
                else:
                    print("[Warning] Evaluating same configuration")

            # Update memories as an objective based on rank changes!
            scores_reevaluation = np.asarray([ pop_scores[hash(tuple(map(lambda a: int(500*a), x)))] for x in opt.position_memories]).T
            ranks_reevaluation = rank_in_db(db, scores_reevaluation)
            opt.aptitude_memories[:] = np.mean(ranks_reevaluation, axis=0)

            opt.tell(Y_objective)
            
            all_configurations.append(X)
            all_objectives.append(Y_objective)
            archive_functions_error.append(Y_function_error)

            iteration_best_objective_idx = np.argmin(Y_objective)
            #print(f"Iteration {iteration} over {metaopt_args["max_fevals"]/metaopt_args["num_agents"]}:", end="")
            if Y_objective[iteration_best_objective_idx] < best_configuration_objective:
                best_configuration_objective = Y_objective[iteration_best_objective_idx]
            #    print(f"New best: {best_configuration_objective}", end="")
                best_configuration = X[iteration_best_objective_idx, :]
            #print(".")
            iteration += 1
            pbar.update(1)
        

    if not return_all:
        return opt.profile, RuleSet(mask, best_configuration, nrules, optimize_membership)
    else:
        return opt.profile, RuleSet(mask, best_configuration, nrules, optimize_membership), (all_configurations, all_objectives, archive_functions_error), db

def optimize_gbrt(mask, seed=42, num_agents=10, max_fevals=2000, optimize_membership=False, db="./db/warmup/full.npy", return_all=False, profiler_args={}):
    raise NotImplementedError
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
    
    cont_dimensions = 2 * nrules if optimize_membership else 0
    categ_dimensions = 8 * nrules
    opt = ParticleSwarm(num_agents=num_agents, num_variables=[cont_dimensions, categ_dimensions], max_fevals=max_fevals)
    if optimize_membership:
        configure_mopt_membership(opt, mask)
    opt.num_categories([nq, no, nq, no, nq, na, na, na] * nrules)
    opt.reset(seed)
    opt.weights[0,:] = 0.3


    best_configuration = None
    best_configuration_objective = np.inf
    all_configurations = []
    all_objectives = []
    archive_functions_error = []
    pop_scores = dict()
    iteration = 0
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
        print(f"Iteration {iteration} over {max_fevals/num_agents}:", end="")
        if Y_objective[iteration_best_objective_idx] < best_configuration_objective:
            best_configuration_objective = Y_objective[iteration_best_objective_idx]
            print(f"New best: {best_configuration_objective}", end="")
            best_configuration = X[iteration_best_objective_idx, :]
        print(".")
        iteration += 1
        

    if not return_all:
        return opt.profile, best_configuration
    else:
        return opt.profile, best_configuration, (all_configurations, all_objectives, archive_functions_error), db
