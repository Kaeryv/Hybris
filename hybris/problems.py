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


def get_benchmark(name: str):
    import yaml
    with open("db/benchmarks.yml") as f:
        data = yaml.safe_load(f)
    
    return data[name]
