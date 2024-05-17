import sys
sys.path.append(".")

from hybris.profiling import profile_configurations
from hybris.functions import available_functions

iw = [0.7298, 1.49618, 1.49618, 0.0, -16, 0.6, 3]
res  = profile_configurations([([], "0000000", None)], nruns=5, benchmark="train")
print(res.shape)

import matplotlib.pyplot as plt
fig, axs = plt.subplots(2,7)
axs = axs.flatten()
for i in range(res.shape[0]):
    axs[i].semilogy(res[i, 0].mean(0))
    #axs[i].set_title(available_functions[i])
    print(res[i, 0].mean(0)[-1])
plt.show()
