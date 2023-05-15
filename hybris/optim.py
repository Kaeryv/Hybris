import hybris
from numpy.ctypeslib import as_array
from ctypes import c_uint32, c_void_p
from typing import List

class Optimizer():
    def __init__(self, num_agents=40, num_variables=[10, 0], max_fevals=4000) -> None:
        self.num_dimensions = sum(num_variables)
        self.max_iterations = max_fevals // num_agents
        self.num_agents = num_agents
        self.num_variables = num_variables
        self.handle = hybris.registry_create(
            num_agents, self.num_dimensions, num_variables[1], self.max_iterations)

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
        #self.handle.contents.lower_bound = self.vmin
        #self.handle.contents.upper_bound = self.vmax
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

    def get_rule(self, index: int) -> int:
        return self.handle.contents.controllers[index]

    @property
    def profile(self):
        return as_array(self.handle.contents.profile, shape=(self.iteration,)).copy()
    @property
    def weights(self):
        return as_array(self.handle.contents.weights, shape=(hybris.Parameter.NUM_WEIGHTS, self.num_agents ))
    
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
    def aptitude_memories(self):
        return as_array(self.handle.contents.mem_aptitude, shape=(self.num_agents, ))
    @property
    def aptitude(self):
        return as_array(self.handle.contents.aptitude, shape=(self.num_agents, ))

