# The objective is to optimize test functions using a surrogate
import sys
sys.path.append(".")
sys.path.append("..")

from argparse import ArgumentParser
parser = ArgumentParser()
parser.add_argument("-graph", action="store_true")
parser.add_argument("-nruns", type=int, default=1)
parser.add_argument("-compute", action="store_true")
parser.add_argument("-strat", type=int, default=1)
parser.add_argument("-problem", type=int, default=0)
parser.add_argument("-dimensions", type=int, default=2)
parser.add_argument("-token", type=str, default="untitled")
parser.add_argument("-ratio", type=int, default=0)
parser.add_argument("-num_hidden", type=int, default=32)
parser.add_argument("-agents", type=int, default=20)
parser.add_argument("-strategy", type=int, default=0)
parser.add_argument("-sigma", type=float, default=0.1)
parser.add_argument("-ndoe", type=int, default=10)
args = parser.parse_args()

from hybris import testfunction
import numpy as np
from hybris.optim import Optimizer

from hybris.problems import ProblemSet, ProblemTag, get_doe
problems = ProblemSet.list_matching_tag(ProblemTag.TRAINING)
prob = problems[args.problem]

to_npy = lambda x: x.detach().numpy()
to_torch = lambda x: torch.from_numpy(x).type(torch.double)

# Setup surrogate
import torch
import torch.nn as nn
from torch.utils.data import DataLoader, TensorDataset
from torch.optim import Adam
class Surrogate(nn.Module):
    def __init__(self, num_inputs, num_outputs, num_hidden, optim_params={"lr":1e-3, "weight_decay":1e-5}):
        super(Surrogate, self).__init__()
        self.num_inputs = num_inputs
        self.num_outputs = num_outputs
        self.num_hidden = num_hidden
        self.l1 = nn.Linear(num_inputs, num_hidden)
        self.l2 = nn.Linear(num_hidden, num_hidden)
        self.l3 = nn.Linear(num_hidden, num_outputs)
        self.x = None
        self.y = None
        self.criterion = nn.MSELoss()
        self.optimizer = Adam(self.parameters(), **optim_params)
        self.double()
    def forward(self, x):
        if not isinstance(x, torch.Tensor):
            x = to_torch(x)
        x = torch.relu(self.l1(x))
        x = torch.tanh(self.l2(x))
        x = self.l3(x)
        return x
    def predict(self, x):
        self.eval()
        return self(to_torch(x)) * self.max
    def add_data(self, x, y, init=False):
        if init:
            self.x = x.copy()
            self.y = y[:, np.newaxis].copy()
            self.max = np.abs(np.max(y))
        else:
            self.x = np.vstack((self.x, x))
            self.y = np.vstack((self.y, y[:, np.newaxis]))
    
    def fit(self, epochs, batch_size=4):
        self.train()
        loader = DataLoader(TensorDataset(to_torch(self.x), to_torch(self.y)), batch_size=batch_size, shuffle=True)
        for _ in range(epochs):
            epoch_loss = 0.0
            for x, y in loader:
                self.optimizer.zero_grad()
                loss = self.criterion(self(x), y / self.max)
                loss.backward()
                epoch_loss += loss.item()
                self.optimizer.step()
            epoch_loss /= len(loader)
        return epoch_loss

from hybris.problems import get_doe
M = args.dimensions
N = args.agents

def run_sbo_I(seed, ratio):
    opt = Optimizer(N, [M, 0], max_fevals=1000)
    model = Surrogate(M, 1, args.num_hidden)
    model.add_data(*get_doe(prob, opt.num_dimensions, args.ndoe), init=True)
    model.fit(100)

    opt.vmin = prob.lower
    opt.vmax = prob.upper
    opt.reset(seed)
    evals = 0
    true_profile= []
    while not opt.stop():
        for i in range(ratio-1):
            if opt.stop():
                break
            x = np.vstack((opt.ask(), opt.position_memories))
            model.eval()
            y = to_npy(model.predict(x)).flatten()
            cur, mem = np.split(y, 2)
            opt.aptitude_memories[:] = mem
            opt.tell(cur)
        if not opt.stop():
            x = opt.ask()
            y = testfunction(prob.function, x)
            model.add_data(x, y)
            model.fit(20)
            evals += opt.num_agents
            opt.tell(y)
        y_true = testfunction(prob.function, x)
        true_profile.append(np.min(y_true))
    return opt.profile, np.zeros_like(opt.profile)

def run_sbo_II(seed, ratio):
    def acquisition_function(candidate, pred_fitness, model, std=args.sigma):
        asort = np.argsort(model.y.flatten())
        sorted_y = model.y.flatten()[asort]
        rank_cost = np.searchsorted(sorted_y, pred_fitness) / sorted_y.shape[0]
        
        cost = 0.0
        for i, x in enumerate(model.x[asort]):
            dist = np.linalg.norm(x - candidate) / np.sqrt(M*(prob.upper - prob.lower)**2)
            cost += np.exp(-dist**2/std**2)
        dist_cost = cost / len(model.x)
        return rank_cost + dist_cost
    opt = Optimizer(N, [M,0], max_fevals=1000)
    model = Surrogate(M, 1, N)
    model.add_data(*get_doe(prob, opt.num_dimensions, args.ndoe), init=True)
    initial_loss = model.fit(100)

    opt.vmin = prob.lower
    opt.vmax = prob.upper
    opt.reset(seed)
    true_profile = []
    while not opt.stop():
        cur_x = opt.ask()
        x = np.vstack((cur_x, opt.position_memories))
        y = to_npy(model.predict(x)).flatten()
        cur, mem = np.split(y, 2)
        opt.aptitude_memories[:] = mem
        # old y = model(torch.from_numpy(x)).detach().numpy().flatten()
        # Just flipped everything -- x -> cur_x
        acost = np.empty(len(x))
        for i, xx in enumerate(x):
            acost[i] = acquisition_function(xx, y[i], model, std=args.sigma-len(true_profile)* 0.001)
        true_evals = 1000 // 50 // ratio
        acost = np.argsort(acost)[:true_evals]
        y_true = testfunction(prob.function, x[acost])
        model.add_data(x[acost], y_true)
        loss = model.fit(10)
        trials = 0
        while loss >= initial_loss*0.1 and trials < 20:
            loss = model.fit(10)
            trials += 1
        
        print(loss)
        #cur[acost] = y_true
        y = to_npy(model.predict(x)).flatten()
        y[acost] = y_true
        cur, mem = np.split(y, 2)
        opt.tell(cur)
        y_true = testfunction(prob.function, x)
        true_profile.append(np.min(y_true))
    return opt.profile, np.asarray(true_profile)

from smt.surrogate_models import LS, QP, KPLS, KRG, KPLSK, GEKPLS, MGP
from smt.sampling_methods import LHS
from hybris.sbo import KrigingSurrogate
from hybris.metrics import gaussian_proximity, rank_ratio
def run_sbo_III(seed, ratio):
    model = KrigingSurrogate(M)
    
    opt = Optimizer(N, [M,0], max_fevals=1000)
    
    std = np.sqrt(M*(prob.upper - prob.lower)**2) * args.sigma
    

    opt.vmin = prob.lower
    opt.vmax = prob.upper
    opt.reset(seed)
    sampling = LHS(xlimits=np.asarray([(opt.vmin, opt.vmax)]*M), criterion='ese', random_state=1)
    x = sampling(args.ndoe)
    y = testfunction(prob.function, x)
    
    model.add_data(x, y, init=True)
    model.train()
    true_profile = []
    while not opt.stop():
        cur_x = opt.ask()
        x = np.vstack((cur_x, opt.position_memories))
        y, ys = model.predict(x, return_std=True)
        cur, mem = np.split(y, 2)
        opt.aptitude_memories[:] = mem
        # old y = model(torch.from_numpy(x)).detach().numpy().flatten()
        # Just flipped everything -- x -> cur_x
        acost = gaussian_proximity(x, model.x, std)
        acost = rank_ratio(y, model.y)
        acost[:] += y / ys.flatten()
        true_evals = 1000 // 50 // ratio
        acost = np.argsort(acost)[:true_evals]
        y_true = testfunction(prob.function, x[acost])
        model.add_data(x[acost], y_true)
        model.train()
        y = model.predict(x)
        y[acost] = y_true
        cur, mem = np.split(y, 2)
        opt.tell(cur)
        y_true = testfunction(prob.function, x)
        true_profile.append(np.min(y_true))
    return opt.profile, np.asarray(true_profile)


if __name__ == '__main__':
    import os
    ratios = [ 1, 2, 5, 10, 20 ]
    if args.compute:
        ratio = ratios[args.ratio]
        res = np.zeros((2, args.nruns, 1000 // args.agents))
        strategy = (run_sbo_I, run_sbo_II, run_sbo_III)[args.strategy]
        for i in range(args.nruns):
            res[0, i,:], res[1, i,:] =  strategy(i, ratio)
        
        os.makedirs("db/results", exist_ok=True)
        np.savez_compressed(f"./db/results/surrogate.{args.token}.{args.ratio}.{args.dimensions}.npz", profiles=res[0], true_profiles=res[1], args=args)