import sys
sys.path.append(".")
from hybris.optim import Optimizer
import numpy as np
from hybris import _lhybris

a = np.load("./db/warmup/full.npy")
b = np.load("./db/warmup/cec20_20d_40p.npy")
b = np.vstack((a, b))
print(b.shape)
np.save("./db/warmup/dual.npy", b)

exit()

b = []
for mask in ["1000000", "0100000", "0010000", "0001000", "0000100", "0000010", "0000001"]:
    a = np.load(f"./db/warmup/cec20_20d_40p_{mask}.npy")
    a= a.mean(axis=-1)
    b.append(a)
b = np.hstack(b)
print(b.shape)
np.save("./db/warmup/cec20_20d_40p.npy", np.sort(b))
exit()



def random_rules(count):
    rules = np.empty((count, 8), dtype=int)
    nq = _lhybris.fuzz_get_num_qualities_combinations()
    no = 8
    limits = [nq, no, nq, no, nq, 3, 3, 3]
    for i in range(8):
        rules[:, i] =np.random.randint(0, high=limits[i], size=(count,))

    return rules

from hybris.profiling import profile_configurations
rules = random_rules(10000)
for mask in ["1000000", "0100000", "0010000", "0001000", "0000100", "0000010", "0000001"]:
    from tqdm import trange
    res = profile_configurations([ (r.tolist(), mask) for r in rules], nruns=5, max_workers=8, endresult=True, progress=trange, benchmark="cec20_20d_40p")


    np.save(f"./db/warmup/cec20_20d_40p_{mask}.npy", res)
