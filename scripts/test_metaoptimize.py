from argparse import ArgumentParser
parser = ArgumentParser()
parser.add_argument("-graph", action="store_true")
parser.add_argument("-compute", action="store_true")
args = parser.parse_args()
import sys
sys.path.append(".")
from hybris.profiling import profile_configurations
from hybris.meta import optimize_self

import numpy as np
'''
rule =  "3  0 42  2 12  1  0  1  6  7 32  2 48  2  2  0"
config = list(map(int,rule.split()))

res = profile_configurations([(config, "1000100")], max_workers=8, endresult=True, benchmark="cec20_20d_40p")
print(res.mean(-1).mean(-1))

exit()
'''
db1 = np.load("./db/warmup/full.npy")
#db2 = np.load("./db/warmup/cec20_20d_40p.npy")
#db = []
#for i in range(len(db1)):
#    db.append(db1[i])
#for i in range(len(db2)):
#    db.append(db2[i])

#prof2, config = optimize_self("1000100", 43, benchmark="train", db=db1, max_fevals=200, num_agents=3)
#print(np.min(prof2))
#import matplotlib.pyplot as plt
#plt.plot(prof)
#plt.plot(prof2)
#plt.show()

#res = profile_configurations([(config, "1000100")], max_workers=8, endresult=True, benchmark="cec20_20d_40p")
#print(config)
#print(res.mean(-1).mean(-1))
if args.compute:
    from tqdm import trange
    profs = []
    configs = []
    for i in trange(50):
        # Get 50 configurations
        prof, config = optimize_self("1000000", i, benchmark="train", db=db1, max_fevals=200, num_agents=4)
        profs.append(prof)
        configs.append(config)

    import pickle
    with open("./tmp/configs2.pkl", "wb") as f:
        pickle.dump(configs, f)

if args.graph:
    import pickle
    with open("./tmp/configs2.pkl", "rb") as f:
        configs = pickle.load(f)
    import matplotlib.pyplot as plt
    from hybris.profiling import profile_configurations
    #res = profile_configurations([ (conf, "1000000") for conf in configs], max_workers=8, benchmark="cec20_20d_40p")
    #res = profile_configurations([ ([], "0000000") ], max_workers=8, benchmark="cec20_20d_40p")
    #np.save("./tmp/res3.npy", res)
    res = np.load("./tmp/res.npy")
    res2 = np.load("./tmp/res2.npy")
    res3 = np.load("./tmp/res3.npy")

    resm = res.reshape(10, -1, 1000)[:, :, -1]
    resm2 = res2.reshape(10, -1, 1000)[:, :, -1]
    resm3 = res3.reshape(10, -1, 1000)[:, :, -1]
    fig, axs = plt.subplots(1, 10, figsize=(10, 4))
    axs = axs.flatten()
    for ax, r, r2, r3 in zip(axs, resm, resm2, resm3):
        ax.violinplot(np.log10(r))
        #ax.violinplot(np.log10(r2))
        ax.violinplot(np.log10(r3))
        ax.set_xticks([])
    plt.tight_layout()
    plt.savefig("test.png")

