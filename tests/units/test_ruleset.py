import unittest

import sys
sys.path.append(".")
from hybris.optim import RuleSet
import numpy as np

class TestRuleset(unittest.TestCase):
    def test_save_and_load(self):
        filename="test_rs.json"
        rs = RuleSet("1000000", np.array([0.0,1.0, 1,3,4,5,6,7,8,9]), 1, 1)
        rs.save(filename)
        rs_loaded = RuleSet.load(filename)
        self.assertEqual(rs.count, rs_loaded.count)
        _ = [self.assertEqual(x, y) for x, y in zip(rs.controllers, rs_loaded.controllers)]
        _ = [self.assertEqual(x, y) for x, y in zip(rs.weights, rs_loaded.weights)]
