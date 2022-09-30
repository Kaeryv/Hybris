from hybris.optim import Optimizer
from hybris.problems import get_benchmark, ProblemSet
import logging
from itertools import product
from concurrent.futures import ThreadPoolExecutor
from functools import partial
import tqdm
import numpy as np

def run_optimizer_problem(prob_conf_seed, opt_args, endresult=False):
    (_, problem), (_, (rules, mask)), seed = prob_conf_seed
    opt = Optimizer(**opt_args)
    opt.disable_all_rules()
    opt.set_rules_fromlist(mask, rules)
    opt.minimize_problem(problem, seed)
    if endresult is True:
        return opt.profile[-1]
    else:
        return opt.profile

def run_optimizer_problem2(prob_conf_seed, opt_args, endresult=False):
    (_, problem), (_, (rules, mask)), seed = prob_conf_seed
    opt = Optimizer(**opt_args)
    opt.disable_all_rules()
    for r, m in zip(rules, mask):
        opt.set_rule(m, r)
    opt.minimize_problem(problem, seed)
    if endresult is True:
        return opt.profile[-1]
    else:
        return opt.profile

def profile_configurations(
    configurations, 
    nruns=5, benchmark="train", max_workers=6, endresult=False, progress=False,
    run_function=run_optimizer_problem):
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
        results = pool.map(partial(run_function, 
                                   opt_args=opt_args, 
                                   endresult=endresult), 
                           prob_conf_seed)
        if progress:
            results = tqdm.tqdm(results, total=len(prob_conf_seed))
        
        results = list(results)
        if not endresult:
            return np.asarray(results).reshape((len(problems), len(configurations), nruns, -1)) - fstar[:, np.newaxis, np.newaxis, np.newaxis]
        else:
            return np.asarray(results).reshape((len(problems), len(configurations), nruns)) - fstar[:, np.newaxis, np.newaxis]
