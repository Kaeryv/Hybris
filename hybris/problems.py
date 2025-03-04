import hybris
import copy
from enum import IntEnum
from types import SimpleNamespace
from functools import partial
from .functions import test_function

class ProblemTag(IntEnum):
    TRAINING    = 1 << 0
    VALIDATION  = 1 << 1
    SURFACEABLE = 1 << 2

class ProblemSet:
    @classmethod
    def list_matching_tag(cls, tag: int):
        count = cls.num_matching_tag(tag)
        assert count > 0, "No functions match tag."
        cases = hybris.get_filtered_testcases(tag)
        return [ cases[i] for i in range(count) ]

    @classmethod
    def list_all(cls):
        tag = 0b1111111111
        return cls.list_matching_tag(tag)

    @staticmethod
    def num_matching_tag(tag: int) -> int:
        return hybris.get_num_filtered_testcases(tag)


class Benchmark():
    @classmethod
    def get(cls, name: str):
        import yaml
        with open("db/benchmarks.yml") as f:
            data = yaml.safe_load(f)
        
        return cls(data[name])

    def __init__(self, input_dict):
        self.db = SimpleNamespace(**input_dict)
    
    def problem(self, fname):
        prob = list(filter(lambda p: p['function'] == fname, self.problems))
        if len(prob) == 0 or len(prob) > 1:
            raise LookupError
        return SimpleNamespace(**prob[0])

    @property
    def function_names(self):
        return (p['function'] for p in self.problems)
    
    @property
    def problems(self):
        return self.db.problems
    
    @property
    def optimizer_args(self):
        return self.db.optimizer_args
    
    def optimizer_set_bounds(self, fname, opt):
        opt.vmin = self.problem(fname).lower
        opt.vmax = self.problem(fname).upper

    def function(self, fname):
        return partial(test_function, name=fname)
