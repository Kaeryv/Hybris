import unittest
import sys
sys.path.append(".")
import time
import numpy as np
from hybris.optim import ParticleSwarm

class TestDiscrete(unittest.TestCase):
    def test_discrete(self):
        start = time.time_ns()

        optim = ParticleSwarm(num_variables=[0, 5])
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
        self.assertLessEqual(optim.profile[-1], 1.0)
        #import matplotlib.pyplot as plt
        #plt.plot(optim.profile)
        #plt.show()
        #print(iter_solution)