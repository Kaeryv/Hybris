import sys
sys.path.append(".")
import time
import numpy as np
from hybris.optim import ParticleSwarm
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

start = time.time_ns()
optim = ParticleSwarm(num_variables=[2, 0])
optim.vmin = -1.0
optim.vmax = 1.0
optim.reset(42)

fig = plt.figure()
line, = plt.plot([-1, 1], [-1, 1], 'r.')
positions = list()
def iteration_function(frame):
    if optim.stop():
        return
    x = optim.ask()
    optim.tell(np.sum(x*x, axis=-1))
    positions.append(x.copy())
    line.set_data((optim.position[:, 0], optim.position[:, 1]))


anim = FuncAnimation(fig, iteration_function, frames=optim.max_iterations, interval=20)
anim.save('animation.gif', writer='imagemagick', fps=24)
stop = time.time_ns()
print(f"Done in {(stop-start)/1e6} ms.")
positions = np.asarray(positions)
plt.close()
# plt.figure()
# for i in range(40):
#     plt.plot(positions[:, i, 0])
# plt.show()

# plt.figure()
# plt.plot(optim.profile)
# plt.show()