import hybris
from hybris import c_double_ptr, CFunState, CPRNG
import numpy as np
from ctypes import pointer

available_functions = [
    "sphere",
    "ackley",
    "rastrigin",
    "rosenbrock",
    "stiblinskitank",
    "schwefel",
    "chungreynolds",
    "alpine",
    "alpine2",
    "griewank",
    "quing",
    "salomon",
    "happycat",
    "xinsheyang1",
    "xinsheyang2",
    "bentcigar"
]

def test_function(X, name="sphere"):
    assert(len(X.shape) > 0 and len(X.shape) < 3)
    function = getattr(hybris, name)
    ndim, nagents = (X.shape[1], X.shape[0]) if len(X.shape) == 2 else (X.shape[0], 1)
    y = np.zeros((nagents))

    state = CFunState()
    prng = CPRNG()
    state.prng = pointer(prng)
    
    function(np.ascontiguousarray(X).ctypes.data_as(c_double_ptr), nagents, ndim, y.ctypes.data_as(c_double_ptr), state)
    return y
    
