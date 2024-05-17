#!/usr/bin/env python
# -*- coding: utf-8 -*-

import numpy as np
import cocoex
import cocopp
from hybris.optim import Optimizer
from sko.PSO import PSO
from math import ceil
from argparse import ArgumentParser
parser = ArgumentParser()
parser.add_argument("-b", type=int)
args = parser.parse_args()
def fmin(FUN, DIM, maxfunevals):
    # Set algorithm parameters
    popsize = 40
    c1 = 1.4944
    c2 = 1.4944
    w = 0.792
    xbound = 5
    vbound = 5
    # Allocate memory and initialize
    xmin = -xbound * np.ones((popsize,DIM))
    xmax = xbound * np.ones((popsize,DIM))
    vmin = -vbound * np.ones((popsize,DIM))
    vmax = vbound * np.ones((popsize,DIM))
    x = 2 * xbound * np.random.rand(popsize,DIM) - xbound;
    v = 2 * vbound * np.random.rand(popsize,DIM) - vbound;
    pbest = x.copy()
    # update pbest and gbest
    cost_p = np.asarray([problem(xx) for xx in pbest])
    cost, index = np.min(cost_p), np.argmin(cost_p)
    gbest = pbest[index,:].copy()
    maxfunevals = min(1e5 * DIM, maxfunevals);
    maxiterations = ceil(maxfunevals/popsize);
    for it in range(2, maxiterations):
        # Update inertia weight
        w = 0.9 - 0.8*(it-2)/(maxiterations-2);

        # Update velocity
        v = w*v + c1*np.random.rand(popsize,DIM)*(pbest-x) + c2*np.random.rand(popsize,DIM)* (gbest-x)

        # Clamp veloctiy
        s = v < vmin
        v = (1-s).astype(float) * v + s * vmin
        b = v > vmax
        v = (1-b).astype(float) * v + b * vmax

        # Update position
        x += v

        # Clamp position - Absorbing boundaries
        # Set x to the boundary
        s = x < xmin # bool
        x = (1-s).astype(float)*x + s.astype(float) * xmin
        b = x > xmax
        x = (1-b).astype(float)*x + b.astype(float) * xmax

        # Clamp position - Absorbing boundaries
        # Set v to zero
        b = np.logical_or(s, b)
        v = (1-b)*v + b*np.zeros((popsize,DIM));
        # Update pbest and gbest if necessary
        cost_x = np.asarray([problem(xx) for xx in x])
        s = cost_x < cost_p
        cost_p[s] = cost_x[s]
        pbest[s, :] = x[s]
        cost, index = np.min(cost_p), np.argmin(cost_p)
        gbest = pbest[index,:]

batches = 8
current_batch = args.b if args.b else 1
output_folder = "kamel"
if batches > 1:
    output_folder += "_batch%03dof%d" % (current_batch, batches)

# budget = 100D
# nagents = min(10D, 40)
budget_multiplier = int(1e5) # times dimension, increase to 10, 100, ...
suite = cocoex.Suite("bbob", "year: 2009", "")
observer = cocoex.Observer("bbob", "result_folder: " + output_folder)
from tqdm import tqdm
for batch_counter, problem in enumerate(tqdm(suite)):
    if problem.dimension == 40:
        continue
    if batch_counter % batches != current_batch % batches:
        continue
    problem.observe_with(observer)  # generate the data for cocopp post-processing
    problem(np.zeros(problem.dimension))  # making algorithms more comparable
    propose_x0 = problem.initial_solution_proposal  # callable, all zeros in first call
    budget = problem.dimension * budget_multiplier
    fmin(problem,  problem.dimension, budget.dimension)
    


