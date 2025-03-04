import unittest
from numpy.testing import assert_allclose
import hybris
import numpy as np
from hybris.functions import test_function
class TestBackend(unittest.TestCase):

    def test_functions(self):
        x = np.zeros((5,5))
        y = test_function(x, "xinsheyang1")
        print(y)

    def test_maps(self):
        x = np.linspace(0, 10, 64)
        X, Y = np.meshgrid(x, x)
        XY = np.vstack((X.flat, Y.flat)).T
        y = test_function(XY, "schwefel")
        #import matplotlib.pyplot as plt
        #plt.matshow(y.reshape(64,64))
        #plt.savefig("map.png")

