from hybris.optim import Optimizer
from hybris.problems import get_benchmark, ProblemSet
import logging
from itertools import product
from concurrent.futures import ThreadPoolExecutor
from functools import partial
import tqdm
import numpy as np

def profiler_kernel(prob_conf_seed, opt_args, initial_weights=None, endresult=False):
    if len(prob_conf_seed[1][1]) == 2:
        (_, problem), (_, (rules, mask)), seed = prob_conf_seed
    elif len(prob_conf_seed[1][1]) == 3:
        (_, problem), (_, (rules, mask, weights)), seed = prob_conf_seed
    else:
        print("Error: ", len(prob_conf_seed[1][1]))
    opt = Optimizer(**opt_args)
    opt.disable_all_rules()
    if initial_weights:
        opt.initial_weights = initial_weights
    opt.set_rules_fromlist(mask, rules)
    if len(prob_conf_seed[1][1]) == 3:
        opt.set_memberships(mask, weights)
    opt.minimize_problem(problem, seed)
    if endresult is True:
        return opt.profile[-1]
    else:
        return opt.profile

def profile_configurations(
    configurations, 
    nruns=5, benchmark="train", max_workers=6, endresult=False, progress=False, initial_weights=None):
    assert len(configurations) > 0, "Supply at least one configuration."

    logging.info(f"Starting to profile configurations with {max_workers} workers.")

    bench = get_benchmark(benchmark)
    problems = list(ProblemSet.list_matching_tag(bench["funset"]))
    fstar = np.asarray(bench["soluces"])
    opt_args = bench["optimizer_args"]

    # We use enumerate to safely bind ids to each variable value
    prob_conf_seed = list(product(
            enumerate(problems), 
            enumerate(configurations), 
            range(nruns)))

    with ThreadPoolExecutor(max_workers=max_workers) as pool:
        results = pool.map(partial(profiler_kernel, 
                                   opt_args=opt_args, 
                                   initial_weights=initial_weights,
                                   endresult=endresult), 
                           prob_conf_seed)
        if progress:
            results = tqdm.tqdm(results, total=len(prob_conf_seed))
        
        results = list(results)
        if not endresult:
            return np.asarray(results).reshape((len(problems), len(configurations), nruns, -1)) - fstar[:, np.newaxis, np.newaxis, np.newaxis]
        else:
            return np.asarray(results).reshape((len(problems), len(configurations), nruns)) - fstar[:, np.newaxis, np.newaxis]
