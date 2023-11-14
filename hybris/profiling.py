from hybris.optim import ParticleSwarm
from hybris.problems import get_benchmark, ProblemSet
import logging
from itertools import product
from concurrent.futures import ThreadPoolExecutor
from functools import partial
import tqdm
import numpy as np
from hybris.functions import available_functions
from hybris.problems import get_benchmark

def profiler_kernel(prob_conf_seed, opt_args, endresult=False):
    (_, problem), (_, (rules, mask, weights)), seed = prob_conf_seed

    opt = ParticleSwarm(**opt_args)
    opt.vmin = problem.lower
    opt.vmax = problem.upper
    opt.disable_all_rules()
    opt.set_rules_fromlist(mask, rules)
    if weights:
        opt.set_memberships(mask, weights)
    opt.minimize_problem(problem, seed)
    if endresult is True:
        return opt.profile[-1]
    else:
        return opt.profile

def profile_configurations(configurations, nruns=5, benchmark="train", max_workers=6, endresult=False, show_progressbar=False, optimizer_args_update={}):
    '''
        optimizer_args_update: 
    '''
    assert len(configurations) > 0, "Supply at least one configuration."

    logging.info(f"Starting to profile configurations with {max_workers} workers.")

    bench = get_benchmark(benchmark)["problems"]
    optimizer_args = get_benchmark(benchmark)["optimizer_args"]
    optimizer_args.update(optimizer_args_update)
    
    problems = [ available_functions[fn["function"]] for fn in bench ]
    for prob, pd in zip(problems, bench):
        prob.lower = pd["lower"]
        prob.upper = pd["upper"]

    fstar = np.asarray([ available_functions[fn["soluce"]] if "soluce" in fn else 0.0 for fn in bench ])


    # We use enumerate to safely bind ids to each variable value
    prob_conf_seed = list(product(
            enumerate(problems), 
            enumerate(configurations), 
            range(nruns)))

    with ThreadPoolExecutor(max_workers=max_workers) as pool:
        results = pool.map(partial(profiler_kernel, 
                                   opt_args=optimizer_args, 
                                   endresult=endresult), 
                           prob_conf_seed)
        if show_progressbar:
            results = tqdm.tqdm(results, total=len(prob_conf_seed))
        
        results = list(results)
        if not endresult:
            return np.asarray(results).reshape((len(problems), len(configurations), nruns, -1)) - fstar[:, np.newaxis, np.newaxis, np.newaxis]
        else:
            return np.asarray(results).reshape((len(problems), len(configurations), nruns)) - fstar[:, np.newaxis, np.newaxis]
