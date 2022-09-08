import sys

from numpy import uint32, uint8

sys.path.append(".")
from hybris.optim import Optimizer
from hybris import Parameter
from hybris.profiling import profile_configurations, run_optimizer_problem2

rule_maxvals = [4, 2, 4, 2, 4]

def ints2rule(ints):
    rule = uint8(0)
    rule |= (ints[0] & 0b11) << 0 
    rule |= (ints[1] & 0b1) << 2 
    rule |= (ints[2] & 0b11) << 3 
    rule |= (ints[3] & 0b1) << 5 
    rule |= (ints[4] & 0b11) << 6 
    return rule

def ints2ruleset(r1, r2, r3, p):
    r1 = ints2rule(r1)
    r2 = ints2rule(r2)
    r3 = ints2rule(r3)
    r = uint32(0)
    r |= r1
    r |= r2 << 8
    r |= r3 << 16
    r |= (p[0] & 0b11) << 24
    r |= (p[1] & 0b11) << 26
    r |= (p[2] & 0b11) << 28
    return r

def rule_from_point(point):
    point = list(map(int, point))
    return ints2ruleset(point[0:5], point[5:10], point[5:15],point[15:18])

print(rule_from_point([0]*18))

import numpy as np
meta = Optimizer(num_agents=3, num_variables=[0, 36], max_fevals=200)
from copy import deepcopy as copy
rm = copy(rule_maxvals)
rule_maxvals.extend(rm)
rule_maxvals.extend(rm)
rule_maxvals.extend([3, 3, 3])
meta.num_categories(rule_maxvals*2)
meta.reset(34)

db =np.load("./db/warmup/full.npy")
best_objective = np.inf
best_config = None
while not meta.stop():
    rules = meta.ask()
    y = []
    rules_save = []
    for rule in rules:
        rule1 = rule_from_point(rule[:18])
        rule2 = rule_from_point(rule[18:])
        rules_save.append(rule)
        scores = profile_configurations([([uint32(rule1), uint32(rule2)], [0, 4])], nruns=5, benchmark="train", max_workers=8, endresult=True, run_function=run_optimizer_problem2)
        scores_mean = scores[:, 0, :].mean(axis=1)
    
        # Compute the ranks
        ranks = np.empty_like(scores_mean)
        for i in range(len(ranks)):
            ranks[i] = np.searchsorted(db[i][:], scores_mean[i]) / len(db[i])
        y.append(np.mean(ranks))
    print(np.min(y))
    current_best = np.min(y)
    if current_best < best_objective:
        best_objective = current_best
        best_config = rules_save[np.argmin(y)]
    meta.tell(y)
import matplotlib.pyplot as plt
#plt.plot(prof)
plt.plot(meta.profile)
plt.show()
rule1 = rule_from_point(best_config[:18])
rule2 = rule_from_point(best_config[18:])
res = profile_configurations([([rule1, rule2], [0, 4])], max_workers=8, endresult=True, run_function=run_optimizer_problem2, benchmark="cec20_20d_40p")
print(res.mean(-1).mean(-1))
