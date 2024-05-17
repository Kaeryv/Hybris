import sys
sys.path.append(".")
import numpy as np
import matplotlib.pyplot as plt
from hybris.problems import get_benchmark
from hybris.profiling import profile_configurations
from hybris.meta import optimize_self
mask="1001100"
benchmark_name = "harrison_2016"
#benchmark_name = "train"
optimizer_args_update = {"max_fevals": 2000}
prof, bc, others, db = optimize_self(mask, seed=4323, 
    profiler_args = {"benchmark": benchmark_name, "max_workers":8, "optimizer_args_update": optimizer_args_update}, 
    db=f"db/{benchmark_name}_2016_1.npy",
    optimize_membership=True,
    max_fevals=2*4000, num_agents=40, return_all=True)

np.save(f"db/{benchmark_name}_1.npy", db)
plt.plot(prof)

from hybris.meta import expandw
ncont = mask.count("1")*2 
res = profile_configurations(
    [
        ([], "0000000", None ),
        ((bc[ncont:]).astype(int), mask, expandw(mask, bc[:ncont]), )
     ], benchmark =  benchmark_name, optimizer_args_update=optimizer_args_update)

print(res.shape)

fig, axs = plt.subplots(4, 5, figsize=(12,10), dpi=200)
bench = get_benchmark(benchmark_name)["problems"]
axs = axs.flatten()
print(res.shape)
for i, (ax, name) in enumerate(zip(axs[:19], bench)):
    ax.plot(res[i, 0].mean(0), label="PSO")
    ax.plot(res[i, 1].mean(0), label="FPSO")
    ax.legend()
    ax.text(0.1, 0.9, f"{name['function']}:{round(res[i,0].mean(0)[-1], 2)}/{round(res[i,1].mean(0)[-1], 2)}", transform=ax.transAxes)

fig.savefig("./performance.png")