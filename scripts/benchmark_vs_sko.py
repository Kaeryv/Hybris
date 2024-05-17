import sys
sys.path.append(".")
max_fevals = 40000
ndims      = 2
popsize    = 40

from sko.PSO import PSO
import yaml
import numpy as np
from functools import partial
from hybris.functions import test_function
from copy import deepcopy
bm = yaml.safe_load(open("db/benchmarks.yml", "r"))
from hybris.optim import Optimizer

# Get the results for sko

sko_profiles = []
for tc in bm["mybenchmark"]:
    lb = tc["lower"] * np.ones(ndims)
    ub = tc["upper"] * np.ones(ndims)
    pso = PSO(func=partial(test_function, name=tc["function"]), n_dim=ndims, pop=popsize, max_iter=max_fevals//popsize, lb=lb, ub=ub, w=0.7, c1=1.4, c2=1.4)
    pso.run()
    sko_profiles.append(deepcopy(pso.gbest_y_hist))

# For hybris
hybris_profiles = []

for tc in bm["mybenchmark"]:
    lb = tc["lower"] * np.ones(ndims)
    ub = tc["upper"] * np.ones(ndims)
    psoh = Optimizer(popsize, [ndims, 0], max_fevals=max_fevals)
    psoh.vmin = lb
    psoh.vmax = ub
    psoh.reset(np.random.randint(0, 9999999))
    while not psoh.stop():
        x = psoh.ask()
        y = test_function(x, name=tc["function"])
        psoh.tell(y)
    hybris_profiles.append(psoh.profile)

import matplotlib.pyplot as plt
fig, axs = plt.subplots(3,2)
axs = axs.flat
for a, b, ax, t in zip(hybris_profiles, sko_profiles, axs, bm["mybenchmark"]):
    ax.plot(a, label="hybris")
    ax.plot(b, label="sko")
    ax.set_title(t["function"])
plt.legend()
plt.savefig("bench1.png")

if ndims == 2:
    x = np.linspace(-500, 500, 64)
    X, Y = np.meshgrid(x, x)
    XY = np.vstack((X.flat, Y.flat)).T
    y = test_function(XY, name="schwefel")
    fig, ax = plt.subplots()
    ax.matshow(y.reshape(64,64), extent=[-500, 500, -500, 500], origin="lower")
    ax.scatter(psoh.position[:,0], psoh.position[:,1])
    ax.scatter(pso.gbest_x[0], pso.gbest_x[1])
    plt.savefig("population_2d.png")

