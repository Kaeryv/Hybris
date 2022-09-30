from hybris.optim import Optimizer
import numpy as np

def fitness_function(x):
    fb = (x[1] * 15.0 - (5.0 / (4.0 * np.pi**2)) * (x[0] * 15.0 - 5.0)**2 + (5.0 / np.pi) * (x[0] * 15.0 - 5.0) -6.0)**2 + 10.0 * (1.0 - 1.0/ (8.0 * np.pi)) * np.cos(x[0] * 15.0 - 5.0) + 10.0
    fb2 = (fb - 54.8104) / 51.9496
    f = 0.0
    if np.isclose(x[4],0.0):
        f = 20.0 * x[2] * x[3]**2 + fb2
    elif np.isclose(x[4],1.0):
        f = fb2
    elif np.isclose(x[4], 2.0):
        f = fb2
    return f


def perform_optim(seed):
    opt = Optimizer(40, [4, 1], 4000)
    opt.num_categories([3])
    opt.vmin = 0.0
    opt.vmax = 1.0
    opt.reset(seed)
    
    
    while not opt.stop():
        configs = opt.ask()
        fitness = list()
        for c in configs:
            fitness.append(fitness_function(c))
        opt.tell(fitness)
    
    return opt.profile[-1]

results = [ 
    perform_optim(seed) for seed in range(20) 
]
print(f"{np.mean(results)=}, {np.std(results)=}")
