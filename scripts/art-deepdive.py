import sys
sys.path.append(".")
from argparse import ArgumentParser

from hybris.optim import ParticleSwarm
from hybris.problems import Benchmark
from hybris.functions import available_functions, test_function
import numpy as np
from hybris.optim import RuleSet



import numpy as np
import matplotlib.pyplot as pl
import scipy.stats as st

# engine = np.load("db/mopts/mopt_1110010_4242.npz")
#engine = np.load("db/mopts/mopt_1110001_4242.npz")
engine = np.load("db/mopts/mopt_1110000_4242.npz")
#engine = np.load("db/mopts/mopt_1000000_4242.npz")
max_fevals = 40000
NA = 40
benchmark = "harrison_2016"
ND = 40
seeds = 1
problem_id = 2


bench = Benchmark.get(benchmark).problems
problems = [ available_functions[fn["function"]] for fn in bench ]
for prob, pd in zip(problems, bench):
    prob.lower = pd["lower"]
    prob.upper = pd["upper"]

def bench_riwpso(prob, seed):
    opt = ParticleSwarm(NA, [ND, 0], max_fevals=max_fevals)
    opt.disable_all_rules()
    raw = engine["memberships"]
    raw.extend(engine["rules"])
    mask = str(engine["mask"])
    rs = RuleSet(mask, raw, type=1)
    #opt.set_rules_fromlist(mask, engine["rules"])
    #opt.set_memberships(mask, expandw(mask, engine["memberships"]))
    opt.vmin[:] = prob["lower"]
    opt.vmax[:] = prob["upper"]
    opt.reset(int(432*seed**1.5+3213215))
    weights = list()
    aptitudes = list()
    positions = list()
    probes=list()
    while not opt.stop():
        x = opt.ask()
        y = test_function(x, name=prob["function"])
        opt.tell(y)
        weights.append(opt.weights.copy())
        aptitudes.append(opt.aptitude.copy())
        positions.append(opt.position.copy())
        probes.append(opt.probes.copy())
    weights = np.asarray(weights)
    aptitudes = np.asarray(aptitudes)
    positions = np.asarray(positions)
    probes = np.asarray(probes)
    return weights, aptitudes, positions, probes

if False:
    weights = list()
    aptitudes = list()
    positions = list()
    probes = list()
    for seed in range(seeds):
        w, a, p, pp = bench_riwpso(bench[problem_id], seed)
        weights.append(w)
        aptitudes.append(a)
        positions.append(p)
        probes.append(pp)

    weights = np.asarray(weights).reshape(seeds, int(max_fevals/NA), 7, NA)
    aptitudes = np.asarray(aptitudes).reshape(seeds, int(max_fevals/NA), NA)
    positions = np.asarray(positions).reshape(seeds, int(max_fevals/NA), NA, ND)
    probes = np.asarray(probes).reshape(seeds, int(max_fevals/NA), NA, 3)
    np.savez_compressed("db/one-omega-c1c2.npz", weights=weights, aptitudes=aptitudes, positions=positions, probes=probes)
from mpl_toolkits.axes_grid1 import make_axes_locatable

def graph(axs, file, title, first=False, last=False):
    data = np.load(file)
    probes = data["probes"]
    apts = data["aptitudes"]
    weights = data["weights"]
    skip = 15
    time = np.repeat(np.arange(max_fevals/NA)[::skip], NA) / max_fevals * NA
    px = probes[0, ::skip, :, 0].flatten()
    im = axs[1].scatter(px,probes[0, ::skip, :, 1].flatten(), c=time, marker=".", cmap="summer")
    axs[1].axis([-1, 1, -12, 0 ])
    axs[1].set_xlabel("Improvement probe $\phi$")
    #ax2b = axs[2].twinx()
    if first:
        axs[1].set_yticks([-2,-6,-10], labels=["$10^{-2}$", "$10^{-6}$", "$10^{-8}$"])
        axs[0].set_yticks([0,0.5,1.0])
        axs[2].set_ylabel("Difference")
        axs[1].set_ylabel("Dist. to best friend probe $\delta$")
        axs[0].set_ylabel("$\omega$")
    else:
        axs[1].set_yticks([-2,-6,-10], labels=[])
        axs[0].set_yticks([0, 0.5, 1], labels=[])
        axs[2].set_yticks([0,0.5, 1], labels=[])
    #ax2b.set_yticks([0,1, 2], labels=[])
    axs[0].set_title(title + f"\n[{apts.min():0.2e}]")
    axs[0].set_xlabel("$c_1+c_2$")

    axs[1].axhline(-1.5, c="grey", alpha=0.5)
    axs[1].axhline(-2, c="grey", alpha=0.5)
    axs[1].axhline(-5, c="grey", alpha=0.5)
    axs[1].set_ylim(-13, 0)
    
    divider = make_axes_locatable(axs[2])
    cax = divider.append_axes('bottom', size='10%', pad=0.0)
    fig.colorbar(im, cax=cax, orientation='horizontal')
    axs[2].set_xticks([])
    cax.set_xticks([0, 0.5, 1])
    cax.set_xticklabels(["0", "1/2", "1"])
    cax.set_xlabel("Optim. progress")

    l1, = axs[2].plot(weights[0, :, 0].mean(axis=1)-0.7298, color="black")
    l2, = axs[2].plot(weights[0, :, 1].mean(axis=1)-1.49618, color="grey")
    l3, = axs[2].plot(weights[0, :, 2].mean(axis=1)-1.49618, color="lightgrey")
    axs[2].set_ylim(-0.5, 0.5)
    # axs[2].set_xlabel("Iteration")


    zoney = np.linspace(-1, 1, 256)
    zonex = (24*(1-zoney**2))/(7-5*zoney)
    axs[0].fill_betweenx(zoney, 0, zonex, color="k", alpha=0.2, facecolor="#83a6aaff")
    axs[0].plot(zonex, zoney, color="k")
    scaty = weights[:,::skip,0].flatten()
    scatx = weights[:,::skip,1].flatten() + weights[:,::skip,2].flatten()
    axs[0].scatter(scatx, scaty, marker=".", c=time, cmap="summer")
    axs[0].axis([0, 4, 0, 1])
    if first==True:
        axs[2].legend([l1,l2,l3], ["$\Delta\omega$", "$\Delta c_1$", "$\Delta c_2$"], bbox_to_anchor=[0.5,0.5], ncols=1, fancybox=False)

import matplotlib.pyplot as plt
fig, axs = plt.subplots(3,3, figsize=(6.6,6), gridspec_kw={"height_ratios": [0.25, 0.45, 0.25]}, dpi=300)
graph(axs[:,0], "db/deepdive/one-pso.npz", "Constant PSO", first=True)
graph(axs[:,1], "db/deepdive/one-omega.npz", "$\omega$ Control")
graph(axs[:,2], "db/deepdive/one-omega-c1c2.npz", "$\omega,c_1,c_2$ Control", last=True)
plt.text(0.01, 0.80, "a.", fontsize=12,  transform=fig.transFigure, weight="bold")
plt.text(0.01, 0.48, "b.", fontsize=12, transform=fig.transFigure, weight="bold")
plt.text(0.01, 0.18, "c.", fontsize=12, transform=fig.transFigure, weight="bold")
plt.text(0.25, 0.96, "1.", fontsize=12, transform=fig.transFigure, weight="bold")
plt.text(0.50, 0.96, "2.", fontsize=12, transform=fig.transFigure, weight="bold")
plt.text(0.75, 0.96, "3.", fontsize=12, transform=fig.transFigure, weight="bold")

plt.subplots_adjust(left=0.15, wspace=0.1, hspace=0.4)
#plt.tight_layout()
fig.savefig("figs/deep-dive.png")
fig.savefig("figs/deep-dive.pdf")
exit()

zoney = np.linspace(-1, 1, 256)
zonex = (24*(1-zoney**2))/(7-5*zoney)
ax1.plot(zonex, zoney, color="k")
scaty = weights[:,::skip,0].flatten()
scatx = weights[:,::skip,1].flatten() + weights[:,::skip,2].flatten()
ax1.scatter(scatx, scaty, alpha=0.1, marker=".", c=time)
ax1.axis([0, 4, 0, 1])
#scat = weights[prob]
#scat = np.swapaxes(scat, 0, 2)
#ax1c = ax1.inset_axes([0,0, 0.5, 0.5])
# kdeplot2d(ax1c, scatx, scaty, expandw(str(engine["mask"]), engine["memberships"]))
# ax1.set_aspect(2)
# xdiff = np.sqrt(np.sum(np.diff(positions[prob].mean(0), axis=0)**2, axis=-1))
# xdiffmax = np.sqrt(ND * (bench[prob]["upper"]-bench[prob]["lower"])**2)
# wdiff = np.abs(np.diff(weights[prob].mean(0), axis=0))[:, 0]
# l1, = ax2.semilogy(xdiff.mean(-1)/xdiffmax, label="Average particle movement")
# l2, = ax2.semilogy(10**weights[prob].mean(0)[:, 4].mean(-1), label="Inertia Value", color="C3")
# #wdiff = wdiff.mean(-1)
# ax2.axhline(1)
# ax3 = ax2.twinx()
# #ax3.plot(wdiff, label="Average parameter movement")
# l3, = ax3.plot(weights[prob].mean(0)[:, 0].mean(-1), label="Inertia Value", color="C1")
# l4, = ax3.plot(weights[prob].mean(0)[:, 1].mean(-1), label="$c_1$ Value", color="C2")
# l5, = ax3.plot(weights[prob].mean(0)[:, 6].mean(-1), label="Connectivity Value", color="C4")
# plt.legend([l1,l2,l3,l4,l5], ["$\\bar{\\Delta(t)}$", "$l$", "$\\omega$", "$c_1$", "$k$"], ncols=5, bbox_to_anchor=(0.6, 1.2))
# #np.save(f"db/tables/{benchmark}_{ND}_{max_fevals}_{'stability'}.npy", perfs)
