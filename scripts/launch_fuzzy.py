import sys
sys.path.append(".")
from hybris.optim import Optimizer
from hybris.functions import test_function
import numpy as np
#rule = list(map(int, "28  3  6  4 32  1  2  1 29  4  0  0 48  0  0  0 19  2 41  7  0  2  1  1".split()))
#rule = list(map(int, "13  7  1  4 29  0  0  0 40  0 32  2 48  2  0  0 48  6 44  7 44  1  1  2".split()))
rule = list(map(int, "40  3 42  4  6  2  1  0 24  4 25  1 21  1  1  2 18  5 19  1 19  0  1  0".split()))

opt = Optimizer(40, [50, 0], max_fevals=40000)
opt.reset(42)
opt.set_rules_fromlist("1110000", rule)
wc12 = np.zeros((1000, 3, 40))
i = 0
while not opt.stop():
    x = opt.ask()
    y = test_function(x, name="rastrigin")
    opt.tell(y)
    wc12[i] = opt.weights[0:3, :]
    i+=1


# Poli and Broomhead 2007 convergence
omega = np.linspace(-1, 1, 200)
c1plc2 = 24 * (1-omega**2) / (7-5*omega)

import matplotlib.pyplot as plt
fig, axs = plt.subplots(1, 2)
axs[0].semilogy(opt.profile)
color = np.repeat(np.arange(1000).reshape(-1, 1), 40, axis=1)
axs[1].scatter(wc12[:, 1].flatten() + wc12[:, 2].flatten(), wc12[:, 0].flatten(), c=color, alpha=0.4)
axs[1].plot(c1plc2, omega)
plt.show()


