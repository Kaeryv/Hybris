import hybris
from hybris import c_double_ptr, CFunState, CPRNG, c_double_ptr, c_int32, c_int32, CFunState, FunSig, _lhybris, CECState
import numpy as np
from ctypes import pointer

__functions_loaded__ = False
BenchmarkFunSig = FunSig(None, [c_double_ptr, c_int32, c_int32, c_double_ptr, CFunState]),
available_functions = dict()

if not __functions_loaded__:
    available_functions = { hybris.get_testcase_by_id(i).name.decode("utf-8"):  hybris.get_testcase_by_id(i) for i in range(hybris.get_num_testcases()) }
    __functions_loaded__ = True
else:
    print("Benchmark functions are loaded")


def test_function(X, name="sphere"):
    assert(len(X.shape) > 0 and len(X.shape) < 3)
    function = available_functions[name].function
    ndim, nagents = (X.shape[1], X.shape[0]) if len(X.shape) == 2 else (X.shape[0], 1)
    y = np.zeros((nagents))

    state = CFunState()
    prng = CPRNG()
    state.prng = pointer(prng)
    state.cecglobals = pointer(CECState())
    
    function(np.ascontiguousarray(X).ctypes.data_as(c_double_ptr), nagents, ndim, y.ctypes.data_as(c_double_ptr), state)
    return y

