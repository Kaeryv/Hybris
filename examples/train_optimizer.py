import sys
sys.path.append(".")
import numpy as np
from hybris.meta import optimize_self

# Usage
# python examples/train_optimizer.py harrison_2016 1000 40 30 8 4242 64 

benchmark_name = sys.argv[1]

# Target problem characteristics
# Target Use NE evals for ND problem using NA agents in swarm...
NE, NA, ND = int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4])
mthreads = int(sys.argv[5])
seed = int(sys.argv[6])
mask=f"{int(sys.argv[7]):07b}"
output_filename = f"rs_{benchmark_name}_{NE}_{NA}_{ND}_{seed}_{mask}.json"

mNA = 40
mNE = 12000
dbname = "./warmup.npy"
opti_args = {
                "max_fevals": NE,
                "num_variables": [ND,0], 
                "num_agents": NA ,
                "initial_weights": [0.7298, 1.49618, 1.49618, 0.0, -16, 0.6, 1.0 ],
            }#[0.679,1.49618,1.49618,0.0,-16,0.6,1.0] on cluster
metaprofile, best_configuration, optidata, db = optimize_self(
    mask, seed=seed, optimize_membership=True, db=dbname, return_all=True, 
    profiler_args={
        "benchmark": benchmark_name,
        "optimizer_args_update": opti_args,
        "max_workers": mthreads
    }, metaopt_args = { # Arguments of the optimizer of the optimizer
        "type": "pso",
        "initial_weights": [0.7298, 1.49618, 1.49618, 0.0, -16, 0.6, 0.5 ],
        "max_fevals": mNE,
        "num_agents": mNA
    })
    
np.save(dbname, db)
best_configuration.save(output_filename, more=opti_args)
