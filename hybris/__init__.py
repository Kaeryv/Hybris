import sys
from pathlib import Path
import os
import platform


# Load the precompiled C code
from ctypes import CDLL
try:
    from .chybris import __file__ as FILENAME
    HYBRIS_BINARY_PATH = FILENAME
except:
    print("Falling-back to local Hybris shared lib.")
    HYBRIS_DEVLIB_PATH=f"./bin/{platform.system()}/libhybris.so"

    if "HYBRIS_BINARY_PATH" in os.environ:
        HYBRIS_BINARY_PATH = os.environ["HYBRIS_BINARY_PATH"]
    elif os.path.isfile(HYBRIS_DEVLIB_PATH):
        HYBRIS_BINARY_PATH = HYBRIS_DEVLIB_PATH
    else:
        _this_mod = sys.modules[__name__]
        HYBRIS_BINARY_PATH = Path(_this_mod.__file__).parent / "lib/libhybris.so"


print("Using Hybris Binary from {}".format(HYBRIS_BINARY_PATH))
_lhybris = CDLL(str(HYBRIS_BINARY_PATH))

from enum import IntEnum
class Parameter(IntEnum):
  WINERTIA = 0
  WCONFIDENCE = 1
  WSOCIAL = 2
  WHYBRIDATION = 3
  WLOWER_SPEED_LIMIT = 4
  WUPPER_SPEED_LIMIT = 5
  WFRIENDS_CAPACITY = 6
  NUM_WEIGHTS = 7

NO_CONTROL_RULE = 1 << 31

from ctypes import (
    Structure, POINTER, CFUNCTYPE, pointer,
    c_uint64, c_uint32, c_int32, c_double, c_float,
    c_char_p, c_void_p)

c_double_ptr = POINTER(c_double)
c_int32_ptr = POINTER(c_int32)
c_uint32_ptr = POINTER(c_uint32)

class CPRNG(Structure):
    _fields_ = [
        ("state", c_uint64),
        ("inc",   c_uint64),
    ]

# Cec functions from GECCO came with a state and files
# The state was isolated but kept 'as is' for reproducibility.
class _CECState(Structure):
    _fields_ = [
        ("OShift",      c_double_ptr),
        ("M",           c_double_ptr),
        ("y",           c_double_ptr),
        ("z",           c_double_ptr),
        ("ini_flag",    c_int32     ),
        ("n_flag",      c_int32     ),
        ("func_flag",   c_int32     ),
        ("SS",          c_int32_ptr ),
    ]

class CECState(_CECState):
    def __init__(self):
        super().__init__()
        _lhybris.cecbench_state_init(pointer(self))


class CController(Structure):
    _fields_ = [ 
        ("iq1", c_int32),
        ("io1", c_int32),
        ("iq2", c_int32),
        ("io2", c_int32),
        ("iq3", c_int32),
        ("ip1", c_int32),
        ("ip2", c_int32),
        ("ip3", c_int32),
    ]

''' Container for problem state '''
class CFunState(Structure):
    _fields_ = [
        ("prng",       POINTER(CPRNG)),
        ("cecglobals", POINTER(CECState)),
    ]

''' Definition of a C problem aptitude function interface '''
CTestFunction = CFUNCTYPE(None, POINTER(c_double), c_int32, c_int32, POINTER(c_double), CFunState)
    
class CTestCase(Structure):
    _fields_ = [ 
        ("name", c_char_p),
        ("function", CTestFunction),
        ("lower", c_double),
        ("upper", c_double),
        ("flags", c_int32),
    ]

''' The main structure containing the optimizer state 
    Be careful when modifying as the position of these fields matters!
'''
class CRegistry(Structure):
    _fields_ = [
        ("position",          c_double_ptr),
        ("mem_position",      c_double_ptr),
        ("prev_position",     c_double_ptr),

        ("speed",             c_double_ptr),
        ("prev_speed",        c_double_ptr),

        ("aptitude",          c_double_ptr),
        ("mem_aptitude",      c_double_ptr),
        ("prev_aptitude",     c_double_ptr),

        ("friends",           c_int32_ptr),
        ("best_friends",      c_int32_ptr),
        ("friends_count",     c_int32_ptr),
        
        ("rank",              c_int32_ptr),
        ("argsort",           c_int32_ptr),
        ("prev_rank",         c_int32_ptr),

        ("cat_var_nvals",     c_int32_ptr),
        ("cat_var_valprob",   c_double_ptr),
        
        ("weights",           c_double_ptr),
        ("fuzzy_props",       c_double_ptr),
        ("fuzzy_states",      c_double_ptr),
        
        ("profile",           c_double_ptr),

        ("initial_weights",   c_double*Parameter.NUM_WEIGHTS),

        ("controllers",       c_uint32*Parameter.NUM_WEIGHTS),
        ("num_agents",        c_uint32),
        ("num_dimensions",    c_uint32),
        ("num_discrete_dimensions",    c_uint32),
        
        ("siwr",              c_int32),
        ("si",                c_int32),

        ("cur_iteration",     c_int32),
        ("max_iterations",    c_int32),

        ("lower_bound",       c_double_ptr),
        ("upper_bound",       c_double_ptr),
        
        ("prng",              POINTER(CPRNG)),
    ]

CRegistryPtr = POINTER(CRegistry)
CTestCasePtr = POINTER(CTestCase)

from collections import namedtuple
FunSig = namedtuple("FunSig", "restype agstypes")
C_INTERFACE = {
    "get_filtered_testcases":              FunSig(CTestCasePtr, [c_int32]),
    "get_num_filtered_testcases":          FunSig(c_int32,      [c_int32]),
    "get_testcase_by_id":                  FunSig(CTestCase,    [c_int32]),
    "fuzz_get_num_qualities_combinations": FunSig(c_uint32,     []),
    "registry_create":                     FunSig(CRegistryPtr, [c_int32, c_int32, c_int32, c_int32]),
    "registry_init":                       FunSig(None ,        [CRegistryPtr, c_uint32]),
    "reg_process_fitness":                 FunSig(None,         [CRegistryPtr]),
    "reg_update_population":               FunSig(None,         [CRegistryPtr]),
    "registry_free":                       FunSig(None,         [CRegistryPtr]),
    "rule_from_points":                    FunSig(c_int32,      [CController]),
    "pcg32_random_seed":                   FunSig(None,         [POINTER(CPRNG), c_uint64]),
    "set_range_weight":                    FunSig(None,         [c_int32, c_double, c_double, c_double]),
    "set_default_weight":                  FunSig(None,         [c_int32, c_double]),
    "reg_set_num_categories":              FunSig(None,         [CRegistryPtr, c_int32, c_int32]),
    "reg_minimize_problem":                FunSig(None,         [CRegistryPtr, CTestCase, c_uint32]),
    "sphere":                              FunSig(None,         [c_double_ptr, c_int32, c_int32, c_double_ptr, CFunState]),
    "sphere":                              FunSig(None,         [c_double_ptr, c_int32, c_int32, c_double_ptr, CFunState]),
    "ackley":                              FunSig(None,         [c_double_ptr, c_int32, c_int32, c_double_ptr, CFunState]),
    "rastrigin":                           FunSig(None,         [c_double_ptr, c_int32, c_int32, c_double_ptr, CFunState]),
    "rosenbrock":                          FunSig(None,         [c_double_ptr, c_int32, c_int32, c_double_ptr, CFunState]),
    "stiblinskitank":                      FunSig(None,         [c_double_ptr, c_int32, c_int32, c_double_ptr, CFunState]),
    "schwefel":                            FunSig(None,         [c_double_ptr, c_int32, c_int32, c_double_ptr, CFunState]),
    "chungreynolds":                       FunSig(None,         [c_double_ptr, c_int32, c_int32, c_double_ptr, CFunState]),
    "alpine":                              FunSig(None,         [c_double_ptr, c_int32, c_int32, c_double_ptr, CFunState]),
    "alpine2":                             FunSig(None,         [c_double_ptr, c_int32, c_int32, c_double_ptr, CFunState]),
    "griewank":                            FunSig(None,         [c_double_ptr, c_int32, c_int32, c_double_ptr, CFunState]),
    "quing":                               FunSig(None,         [c_double_ptr, c_int32, c_int32, c_double_ptr, CFunState]),
    "salomon":                             FunSig(None,         [c_double_ptr, c_int32, c_int32, c_double_ptr, CFunState]),
    "happycat":                            FunSig(None,         [c_double_ptr, c_int32, c_int32, c_double_ptr, CFunState]),
    "xinsheyang1":                         FunSig(None,         [c_double_ptr, c_int32, c_int32, c_double_ptr, CFunState]),
    "xinsheyang2":                         FunSig(None,         [c_double_ptr, c_int32, c_int32, c_double_ptr, CFunState]),
    "bentcigar":                           FunSig(None,         [c_double_ptr, c_int32, c_int32, c_double_ptr, CFunState])
}

if _lhybris.cec_bench_present is True:
    C_INTERFACE.update({
        "cecbench_state_init":                 FunSig(None,         [POINTER(CECState)]),
        "cecbench_state_free":                 FunSig(None,         [POINTER(CECState)]),
        })

''' A wrapper to implement verification/debugging of C API calls '''
def caller(c_fun, *args):
    assert len(args) == len(c_fun.agstypes), f"Wrong number of arguments in call to {c_fun}"
    c_args = [ agstype(arg) if agstype is c_double else arg for arg, agstype in zip(args,c_fun.agstypes)]
    return c_fun(*c_args)

def init_callables():
    from functools import partial
    current_module = __import__(__name__)
    for fname in C_INTERFACE:
        restype, agstypes = C_INTERFACE[fname]
        getattr(_lhybris, fname).restype = restype
        getattr(_lhybris, fname).agstypes = agstypes
        partial_caller = partial(caller, getattr(_lhybris, fname))
        setattr(current_module, fname, partial_caller)
        
init_callables()

import numpy as np
def testfunction(function, X):
    N, D = X.shape
    f = np.empty(N)
    prng = CPRNG()
    cec = CECState()
    state = CFunState(pointer(prng), pointer(cec))
    function(np.ascontiguousarray(X).ctypes.data_as(POINTER(c_double)), N, D, f.ctypes.data_as(POINTER(c_double)), state)
    return f
