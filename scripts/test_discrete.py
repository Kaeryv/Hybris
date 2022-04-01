import sys
sys.path.append(".")
import time
import numpy as np
from hybris.optim import Optimizer

start = time.time_ns()

optim = Optimizer(num_variables=[0, 5])
optim.num_categories([3, 4, 50, 50, 50])
optim.reset(42)

iter_solution = []
while not optim.stop():
    x = optim.ask()
    cost = np.sum(x*x, axis=-1)
    optim.tell(cost)
    best = np.argmin(cost)
    iter_solution.append(optim.position[best])


stop = time.time_ns()
print(f"Done in {(stop-start)/1e6} ms.")

import matplotlib.pyplot as plt
plt.plot(optim.profile)
plt.show()
print(iter_solution)