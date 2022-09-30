# Hybris-Python

## Installation

```
$ pip install hybris-py
```


## Getting started

### Optimizing any function

An optimization of function Sphere can be conducted as follows

```python
def objective_function(X):
    return np.mean(np.power(X, 2), axis=-1)

from hybris.optim import Optimizer
opt = Optimizer(20, [10, 0], max_fevals=200)
opt.vmin = -5.0, opt.vmax = 5.0
opt.reset(456349)

while not opt.stop():
    decision_variables = opt.ask()
    objective_values = objective_function(decision_variables)
    opt.tell(objective_values)

# Show the resulting profile
import matplotlib.pyplot as plt
plt.semilogy(opt.profile)
plt.savefig("Profile.png")
```

### Optimizing the optimizer

To do meta-optimization, any categorical optimizer can be used. We provide a simplified way to do so in the `meta` module.
```python
from hybris.meta import optimize_self
prof = optimize_self("1001000", 43)
import matplotlib.pyplot as plt
plt.plot(prof)
plt.show()
```


