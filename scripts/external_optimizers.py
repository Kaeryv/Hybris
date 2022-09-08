from argparse import ArgumentParser

# First and foremost

parser = ArgumentParser()
parser.add_argument("-m", "--method", required=True, type=str)
parser.add_argument("-n", "--nruns", required=True, type=int)
parser.add_argument("-o", "--output", required=True, type=str)
parser.add_argument("-b", "--benchmark", required=True, type=str)
args = parser.parse_args()

from hybris.problems import ProblemClass, ProblemSet, CECBench, benchmarks
from hybris.backend import CFunState
from hybris.tools import PRNG, to_c
import numpy as np
from functools import partial


# Function state when not using Hybris Optimizer
state = CFunState()
state.cecglobals = CECBench.new_state()
state.prng = PRNG(32)

bench = benchmarks[args.benchmark]
problems = list(ProblemSet.matching_filter(bench["funset"]))
dimensions = bench["optimizer_args"]["num_dimensions"]
max_fevals = bench["optimizer_args"]["max_fevals"]
print(f"Working on {bench['title']}")

gprofile = []
# Wrapper function to adapt our interface to any lib.
def func_wrapper(X, func=None):
    global gprofile
    f = np.empty(1)
    func(to_c(X), 1 , X.shape[0], to_c(f), state)
    gprofile.append(f[0])
    return f[0]

if args.method == "GA":
    import geneticalgorithm.geneticalgorithm as ga
    popsize = 100
    max_iters = max_fevals // popsize

    algorithm_param = {'max_num_iteration': max_iters,
                       'population_size': popsize,
                       'mutation_probability':0.1,
                       'elit_ratio': 0.01,
                       'crossover_probability': 0.5,
                       'parents_portion': 0.3,
                       'crossover_type':'uniform',
                       'max_iteration_without_improv':None}
    
    profiles = np.empty((len(problems), args.nruns, max_iters+1))
    for fi, f in enumerate(problems):
        print(f"Processing problem: {f.name}")
        bounds = np.array([[f.lower, f.upper]]*dimensions)
        function=partial(func_wrapper, func=f.func)
        for i in range(args.nruns):
            model = ga(function=function, dimension=dimensions, variable_type='real', variable_boundaries=bounds,
                       progress_bar=False, convergence_curve=False, algorithm_parameters=algorithm_param)
            model.run()
            profiles[fi, i, :] = np.asarray(model.report)
    np.save(f"{args.output}.npy", profiles)


#
# Differential evolution
# https://docs.scipy.org/doc/scipy/reference/generated/scipy.optimize.differential_evolution.html
#
 
elif args.method == "DE":
    import copy
    from scipy.optimize import differential_evolution
    popsize = 20
    profiles = []
    for fi, f in enumerate(problems):
        profiles.append([])
        print(f"Processing problem: {f.name}")
        bounds = [(f.lower, f.upper)]*dimensions
        for si in range(args.nruns):
            # fevals = (maxiter + 1) * popsize * len(x)
            maxtiter = int(max_fevals / dimensions / popsize) - 1
            result = differential_evolution(partial(func_wrapper,func=f.func), bounds, maxiter=maxtiter, popsize=popsize, polish=False, tol=0.0)
            #profiles[fi, i, :] = np.asarray(model.report)
            profiles[fi].append(copy.copy(gprofile))
            gprofile.clear()
    profiles = np.asarray(profiles).reshape(len(problems), args.nruns, -1)
    np.save(f"{args.output}.npy", profiles)
