import sys
sys.path.append(".")
import time
import numpy as np
from hybris.optim import Optimizer

start = time.time_ns()
optim = Optimizer()
optim.reset(42)

while not optim.stop():
    x = optim.ask()
    optim.tell(np.sum(x*x, axis=-1))

stop = time.time_ns()
print(f"Done in {(stop-start)/1e6} ms.")

#import matplotlib.pyplot as plt
#plt.plot(optim.profile)
#plt.show()
