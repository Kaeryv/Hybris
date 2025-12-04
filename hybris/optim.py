import hybris
from numpy.ctypeslib import as_array
from ctypes import c_uint32
from typing import List
from abc import ABC, abstractmethod
import numpy as np
import json

class OptimizerFactory():
    @staticmethod
    def create(type, **kwargs):
        if type == "pso":
            return ParticleSwarm(**kwargs)
        else:
            raise NotImplementedError

class Optimizer(ABC):
    @abstractmethod
    def reset(self, seed):
        pass
    @abstractmethod
    def stop(self):
        pass
    @abstractmethod
    def vmin(self):
        pass
    @abstractmethod
    def vmax(self):
        pass

def expandw(mask, x):
    if len(x) == 0:
        return None
    x = np.asarray(x)
    hard_boundaries = [
        (0.0, 1.0), # w
        (0.0, 2.5), # c1
        (0.0, 2.5), # c2
        (0.0, 1.0), # h
        (-16, -1.), # l
        (0.0, 1.0), # L
        (0.0, 1.0)  # K
    ]
    nrules = mask.count("1")
    wcw = x.reshape(nrules, 2)
    j = 0
    ret = []
    for i, e in enumerate(list(mask)):
        if e == "1":
            c = wcw[j][0]
            w = wcw[j][1] / 2.0
            ret.extend([ max(hard_boundaries[i][0], c - w), c, min(c+w, hard_boundaries[i][1])])
            j += 1
    return ret

class RuleSet():
    def __init__(self, mask, raw, count=0, type=0):
        self.raw = np.array(raw)
        self.count = sum(map(int, mask))
        cont_dimensions = 2 * count if type == 1 else 0

        self.controllers = self.raw[cont_dimensions:].astype(int)
        self.mask = mask
        self.weights = np.array(expandw(mask, self.raw[:cont_dimensions])) if type == 1 else None
        self.type = type

    def save(self, filename, more={}):
        with open(filename, "w") as f:
            json.dump({
                "raw": self.raw.tolist(),
                "mask": self.mask,
                "type": self.type,
                "weights": self.weights.tolist(),
                "controllers": self.controllers.tolist() if  self.controllers is not None else None,
                "count": self.count,
                "more": more
            }, f)
    
    @classmethod
    def load(cls, filename):
        with open(filename, "r") as f:
            f = json.load(f)
            self = cls(f["mask"], f["raw"], f["count"], f["type"])

        return self, f["more"]


def setup_opt_control(opt, ruleset: RuleSet = None):
    if ruleset is None:
        return
    opt.disable_all_rules()
    opt.set_rules_fromlist(ruleset.mask, ruleset.controllers)
    if ruleset.weights is not None:
        opt.set_memberships(ruleset.mask, ruleset.weights)


class ParticleSwarm(Optimizer):
    def __init__(self, num_agents=40, num_variables=[10, 0], max_fevals=4000, initial_weights=None) -> None:
        self.num_dimensions = sum(num_variables)
        self.max_iterations = max_fevals // num_agents
        self.num_agents = num_agents
        self.num_variables = num_variables
        self.handle = hybris.registry_create(
            num_agents, self.num_dimensions, num_variables[1], self.max_iterations)

        if initial_weights:
            self.initial_weights = initial_weights
        
        self._stop = True
        self.iteration = 0
    
    @property
    def iteration(self):
        return self.handle.contents.cur_iteration
    @iteration.setter
    def iteration(self, value):
        self.handle.contents.cur_iteration = value
    @property
    def vmin(self):
        return as_array(self.handle.contents.lower_bound, shape=(self.num_variables[0],)) 
    @property
    def vmax(self):
        return as_array(self.handle.contents.upper_bound, shape=(self.num_variables[0],)) 
    @vmin.setter
    def vmin(self, value):
        a = as_array(self.handle.contents.lower_bound, shape=(self.num_variables[0],))
        a[:] = value
    @vmax.setter
    def vmax(self, value):
        a = as_array(self.handle.contents.upper_bound, shape=(self.num_variables[0],))
        a[:] = value

    def __del__(self):
        hybris.registry_free(self.handle)
    
    def num_categories(self, vars):
        assert len(vars) == self.num_variables[1], "Expecting a value for each categorical variable."
        for i, v in enumerate(vars):
            hybris.reg_set_num_categories(self.handle, i, v)
    
    def reset(self, seed):
        hybris.registry_init(self.handle, seed)
        self.initialized = True
        self.iteration = 0
        self._stop = False

    def minimize_problem(self, problem, seed):
        hybris.reg_minimize_problem(self.handle, problem, seed)

    def stop(self):
        return self._stop

    def ask(self):
        hybris.reg_update_population(self.handle)
        return as_array(self.handle.contents.position, shape=(self.num_agents, self.num_dimensions)).copy()

    def tell(self, aptitude):
        array_view = as_array(self.handle.contents.aptitude, shape=(self.num_agents,))
        array_view[:] = aptitude
        hybris.reg_process_fitness(self.handle)

        if self.iteration >= self.max_iterations:
            self._stop = True
    
    def disable_rule(self, index):
        self.handle.contents.controllers[index] = hybris.NO_CONTROL_RULE

    def disable_all_rules(self):
        for i in range(hybris.Parameter.NUM_WEIGHTS):
            self.disable_rule(i)

    def set_rule(self, index: int, rule: int):
        self.handle.contents.controllers[index] = c_uint32(rule)

    def set_rule_fromlist(self, index: hybris.Parameter, rule: List[int]):
        """
        :param index: The identifier of the parameter controlled by the rule.
        :param rule: The rule as a set of integer permutation indexes
        """
        assert len(rule) == 8, "A rule is expressed by 8 integers here."
        r = hybris.rule_from_points(hybris.CController(*rule))
        self.set_rule(index, r)

    def set_rules_fromlist(self, mask, rules):
        """
        :param mask: Bitfield represented by a string or a list, for example "0000001"
        :param rules: For each 1 in the bitfield, the associated rule
        """
        mask = mask if isinstance(mask, str) else "".join([str(e) for e in mask])
        assert len(mask) == hybris.Parameter.NUM_WEIGHTS, f"Lenght of mask is incorrect: {len(mask)}"
        j = 0
        for i, e in enumerate(mask):
            if e == '1':
                rule = rules[j*8:j*8+8]
                self.set_rule_fromlist(i, rule)
                j += 1

    def set_memberships(self, mask, weights):
        """
        :param mask: Bitfield represented by a string or a list, for example "0000001"
        :param weights: For each 1 in the bitfield, the associated rule
        """
        mask = mask if isinstance(mask, str) else "".join([str(e) for e in mask])
        assert len(mask) == hybris.Parameter.NUM_WEIGHTS, f"Lenght of mask is incorrect: {len(mask)}"
        j = 0
        for i, e in enumerate(mask):
            if e == '1':
                w = weights[j*3:j*3+3]
                self.handle.contents.controllers_membership[i][0] = w[0]
                self.handle.contents.controllers_membership[i][1] = w[1]
                self.handle.contents.controllers_membership[i][2] = w[2]
                j += 1


    def get_rule(self, index: int) -> int:
        return self.handle.contents.controllers[index]

    @property
    def profile(self):
        return as_array(self.handle.contents.profile, shape=(self.iteration,)).copy()
    @property
    def weights(self):
        return as_array(self.handle.contents.weights, shape=(hybris.Parameter.NUM_WEIGHTS, self.num_agents ))
    @property
    def initial_weights(self):
        return as_array(self.handle.contents.initial_weights, shape=(hybris.Parameter.NUM_WEIGHTS))
    @initial_weights.setter
    def initial_weights(self, v):
        a = as_array(self.handle.contents.initial_weights, shape=(hybris.Parameter.NUM_WEIGHTS))
        a[:] = v
    
    @property
    def probes(self):
        return as_array(self.handle.contents.fuzzy_props, shape=( self.num_agents, 3,))
    
    @property
    def position(self):
        return as_array(self.handle.contents.position, shape=(self.num_agents, self.num_dimensions))
    @property
    def position_memories(self):
        return as_array(self.handle.contents.mem_position, shape=(self.num_agents, self.num_dimensions))

    @property
    def speed(self):
        return as_array(self.handle.contents.speed, shape=(self.num_agents, self.num_dimensions))
    @property
    def aptitude_memories(self):
        return as_array(self.handle.contents.mem_aptitude, shape=(self.num_agents, ))
    @property
    def aptitude(self):
        return as_array(self.handle.contents.aptitude, shape=(self.num_agents, ))

