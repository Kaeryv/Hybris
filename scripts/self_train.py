import numpy as np
import sys
sys.path.append(".")
from hybris.meta import optimize_self
import hybris
#initial_weights = [0.7298, 1.49618, 1.49618, 0.0, -16, 0.6, 19 ]
prof, conf, etc, new_db = optimize_self("1000000", 4235, num_agents=20, max_workers=6, return_all=True, db="db/warmup/new.npy", max_fevals=300, )
#                                        initial_weights=initial_weights)
np.save("db/warmup/new.npy", new_db)
import matplotlib.pyplot as plt
plt.plot(prof)
plt.show()
print(conf)
