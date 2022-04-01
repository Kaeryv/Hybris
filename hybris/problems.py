import hybris
import copy
from enum import IntEnum

class ProblemTag(IntEnum):
    TRAINING    = 1 << 0
    VALIDATION  = 1 << 1
    SURFACEABLE = 1 << 2

class ProblemSet:
    @classmethod
    def list_matching_tag(cls, tag: int):
        count = cls.num_matching_tag(tag)
        cases = hybris.get_filtered_testcases(tag)
        return [ cases[i] for i in range(count) ]

    @classmethod
    def list_all(cls):
        tag = 0b1111111111
        return cls.list_matching_tag(tag)

    @staticmethod
    def num_matching_tag(tag: int) -> int:
        return hybris.get_num_filtered_testcases(tag)


def get_benchmark(name: str):
    import json
    with open("db/benchmarks.json") as f:
        data = json.load(f)
    
    return data[name]

import numpy as np
from hybris import testfunction
def get_doe(problem, num_dimensions, num_items):
    x = problem.lower + (problem.upper - problem.lower) * np.random.rand(num_items, num_dimensions)
    y = testfunction(problem.function, x)
    return x, y