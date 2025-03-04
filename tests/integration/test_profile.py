import sys
sys.path.append(".")
from hybris.profiling import profile_configurations

res = profile_configurations([None], max_workers=12, endresult=True)
print(",\n".join(map(str, res.mean(axis=-1).flat)))
import numpy as np
print(res.shape)
#np.save("tmp.npy", res)
#a = np.load("tmp.npy")
np.testing.assert_allclose(res, res)
