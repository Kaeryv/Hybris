import sys
sys.path.append(".")
from hybris.optim import Optimizer
from hybris.functions import test_function, available_functions
import numpy as np
from hybris.meta import rank_in_db


w1 = np.random.randn(3, 8)
w2 = np.random.randn(8, 3)
def perform_optimization(w1, w2, fun):
    opt = Optimizer(40, [20, 0], max_fevals=2000)
    opt.vmin = -5
    opt.vmax =  5
    opt.reset(42)
    while not opt.stop():
        x = opt.ask()
        y = test_function(x, fun)
        opt.tell(y)
        for i in range(40):
            opt.weights[0:3, i] = w1 @ np.tanh(w2 @ opt.probes[i])
    print(str(opt.profile[-1]))
    return opt.profile[-1]

import cma
#mopt = cma.CMAEvolutionStrategy(48 * [0], 0.5)
mopt = Optimizer(20, [48,0], max_fevals=20000)
mopt.vmin = -2.5
mopt.vmax =  2.5
mopt.reset(43432)
mopt.weights[0,:] = 0.4
bests = list()
fevals = 0
best_w  = None
db = np.load("db/warmup/new.npy")

def benchmark_controller(p, db):
    loss = np.zeros((len(available_functions)))
    for i, fun in  enumerate(available_functions):
        loss[i] = perform_optimization(p[0:24].reshape(3,8), p[24:].reshape(8,3), fun)  

    ranks = rank_in_db(loss, db)
    return np.mean(ranks)

while not mopt.stop():
    params = mopt.ask()
    perf = [benchmark_controller(p, db) for p in params]
    fevals+=len(params)
    #mopt.tell(params, np.ascontiguousarray(perf))
    mopt.tell(np.ascontiguousarray(perf))
    bests.append(np.min(perf))
    if np.min(perf) <= np.min(bests):
        best_w = params[np.argmin(perf)]
    if fevals >= 10000:
        break

import matplotlib.pyplot as plt
np.savez("weights.npz", w1=best_w[0:24].reshape(3,8), w2=best_w[24:].reshape(8,3))
#plt.semilogy(mopt.profile)
plt.semilogy(bests)
plt.savefig("pso.png")
plt.show()
