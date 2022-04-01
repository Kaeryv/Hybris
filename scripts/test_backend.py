import sys
sys.path.append(".")
import hybris
from ctypes import pointer


reg = hybris.registry_create(40, 8, 0, 1000)
reg.contents.lower_bound = -1.0
reg.contents.upper_bound =  1.0
hybris.registry_init(reg, 45)
hybris.reg_update_population(reg)
hybris.reg_process_fitness(reg)