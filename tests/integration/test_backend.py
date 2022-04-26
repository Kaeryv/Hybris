import unittest

import sys
sys.path.append(".")
import hybris
from ctypes import pointer
from numpy.testing import assert_allclose


class TestBackend(unittest.TestCase):
    def test_init(self):
        reg = hybris.registry_create(40, 8, 0, 1000)
        reg.contents.lower_bound = -1.0
        reg.contents.upper_bound =  1.0
        hybris.registry_init(reg, 45)
        self.assertEqual(reg.contents.num_agents, 40)
        self.assertEqual(reg.contents.num_dimensions, 8)
        for i in range(hybris.Parameter.NUM_WEIGHTS):
            self.assertEqual(reg.contents.controllers[i], hybris.NO_CONTROL_RULE)

        print(reg.contents.position[0])
        hybris.reg_update_population(reg)
        hybris.reg_process_fitness(reg)

