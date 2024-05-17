import sys
sys.path.append(".")
import numpy as np
import matplotlib.pyplot as plt
from hybris.optim import ParticleSwarm
from hybris.meta import expandw
from hybris.problems import get_benchmark
from hybris.functions import available_functions, test_function
from hybris.profiling import profile_configurations


bench = get_benchmark("harrison_2016")["problems"]
problems = [ available_functions[fn["function"]] for fn in bench ]
for prob, pd in zip(problems, bench):
    prob.lower = pd["lower"]
    prob.upper = pd["upper"]
root = "db/mopts/"
mask = "1000000"
problem_id  = 1
max_fevals = 40000
#config = np.load(f"{root}/mopt_{mask}_442.npz")
config = np.load(f"mopt_1000000_54321.npz")
baseline_profile = profile_configurations([
    ([], "0000000", []),
    (config["rules"], mask, expandw(mask, config["memberships"]))
], benchmark="harrison_2016", optimizer_args_update={"max_fevals": max_fevals, "num_variables": [50,0], "num_agents": 40})


opt = ParticleSwarm(40, [50, 0], max_fevals=max_fevals)
opt.set_memberships(mask, expandw(mask, config["memberships"]))
opt.disable_all_rules()
opt.set_rules_fromlist(mask, config["rules"])
prob = bench[problem_id]
opt.vmin[:] = prob["lower"]
opt.vmax[:] = prob["upper"]
opt.reset(1)
weights = list()
while not opt.stop():
    x = opt.ask()
    y = test_function(x, name=prob["function"])
    opt.tell(y)
    weights.append(opt.weights.copy())
weights = np.asarray(weights)

fig, axs = plt.subplots(2)
axs[0].plot(np.mean(weights[:, 0], axis=(1)))
axs[1].plot(baseline_profile[problem_id,0].mean(0))
axs[1].plot(baseline_profile[problem_id,1].mean(0))
axs[1].plot(opt.profile)
plt.show()