from argparse import ArgumentParser
parser = ArgumentParser()
parser.add_argument("-compute", action="store_true")
parser.add_argument("-graph", action="store_true")
args = parser.parse_args()


import sys
sys.path.append(".")
from hybris.tools import prototypes_sets
from hybris import Parameter
from hybris.meta import optimize_self
import numpy as np
import pickle

masks, parameter_pairs = prototypes_sets(Parameter.NUM_WEIGHTS, 2, return_associations=True)

db1 = np.load("./db/warmup/full.npy")

if args.compute:
    for i, (mask, comb) in enumerate(zip(masks, parameter_pairs)):
        print(f"metaoptimizing {mask}")
        prof, config, more_data = optimize_self(mask, 43, benchmark="train", db=db1, max_fevals=500, num_agents=3, return_all=True)
        with open(f"./db/results/metaoptim_{i}.pkl", 'wb') as f:
            pickle.dump({"profile": prof, "best_config": config, "all": more_data}, f)


if args.graph:
    from hybris.profiling import profile_configurations

    import matplotlib.pyplot as plt
    import matplotlib
    font = {'family' : 'normal',
        'weight' : 'normal',
        'size'   : 18}

    matplotlib.rc('font', **font)
    from mpl_toolkits.axes_grid1 import make_axes_locatable
    pso = profile_configurations([([], "0000000")], nruns=5, benchmark="train", max_workers=8)[:, 0, :, -1]
    pso = pso.mean(-1)
    db = np.load("./db/warmup/full.npy")
    pso_mean_rank = np.mean([ np.searchsorted(db[i], pso[i]) / len(db[i]) for i in range(db.shape[0])])
    

    A = np.zeros((Parameter.NUM_WEIGHTS, Parameter.NUM_WEIGHTS))
    for i, (mask, comb) in enumerate(zip(masks, parameter_pairs)):
        data = pickle.load(open(f"./db/results/metaoptim_{i}.pkl", "rb"))["profile"][-1]
        A[comb[0], comb[1]] = data / pso_mean_rank
    
    fig, axs = plt.subplots(figsize=(8, 6))
    h = axs.matshow(A.T, cmap="Greys")
    names = ["", "$\omega$", "$c_1$", "$c_2$", "$h$", "$l$", "$u$", "$K$"]
    axs.set_xticklabels(names), axs.set_yticklabels(names)

    cax = make_axes_locatable(axs).append_axes('right', size='5%', pad=0.05)
    cb = fig.colorbar(h, cax=cax, orientation='vertical')
    cb.set_label("Mean meta-objective relative to PSO$\quad$")
    plt.subplots_adjust(left=0)
    axs.xaxis.tick_bottom()
    axs.set_xlabel("Parameter I")
    axs.set_ylabel("Parameter II")
    x_step = np.array([ (i-0.5, i+0.5) for i in range(7) ]).flatten()
    y_step = np.array([ (i-0.5, i-0.5) for i in range(7) ]).flatten()
    axs.plot(x_step, y_step, 'k')
    plt.savefig("figures/MetaOptimcross.pdf", transparent=True)

