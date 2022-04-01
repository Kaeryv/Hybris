# The objective is to optimize test functions using a surrogate
import sys
sys.path.append(".")
sys.path.append("..")

from hybris import testfunction
import numpy as np
from hybris.optim import Optimizer

from hybris.problems import ProblemSet, ProblemTag
problems = ProblemSet.list_matching_tag(ProblemTag.TRAINING)
prob = problems[1]
print(f"Working with {prob.name}")


# Setup surrogate
import torch
import torch.nn as nn
from torch.utils.data import DataLoader, TensorDataset
from torch.optim import Adam
class Surrogate(nn.Module):
    def __init__(self, num_inputs, num_outputs, num_hidden):
        super(Surrogate, self).__init__()
        self.num_inputs = num_inputs
        self.num_outputs = num_outputs
        self.num_hidden = num_hidden
        self.fc1 = nn.Linear(num_inputs, num_hidden)
        self.fc2 = nn.Linear(num_hidden, num_hidden)
        self.fc3 = nn.Linear(num_hidden, num_outputs)
        self.x = None
        self.y = None
        self.loss = nn.L1Loss()
        self.optimizer = Adam(self.parameters(), lr=0.01, weight_decay=1e-3)
        self.double()
    def forward(self, x):
        x = torch.relu(self.fc1(x))
        x = torch.tanh(self.fc2(x))
        x = self.fc3(x)
        return x
    
    def add_data(self, x, y, init=False):
        if init:
            self.norm = y.mean()
            self.x = x.copy()
            self.y = y[:, np.newaxis].copy()
            return
        self.x = np.vstack((self.x, x))
        self.y = np.vstack((self.y, y[:, np.newaxis]))
    
    def fit(self, epochs):
        self.train()
        loader = DataLoader(TensorDataset(torch.from_numpy(self.x), torch.from_numpy(self.y)), batch_size=8, shuffle=True)
        for e in range(epochs):
            for x, y in loader:
                self.optimizer.zero_grad()
                loss = self.loss(self(x), y)
                loss.backward()
                self.optimizer.step()


def acquisition_function(candidate, pred_fitness, model):
    asort = np.argsort(model.y.flatten())
    sorted_y = model.y[asort]
    rank_cost = np.searchsorted(sorted_y.flatten(), pred_fitness) / sorted_y.shape[0]
    
    cost = 0.0
    for i, x in enumerate(model.x[asort]):
        dist = np.linalg.norm(x - candidate) / np.sqrt(2*(prob.upper - prob.lower)**2)
        cost += np.exp(-dist**2/0.01**2)
    print(f"{rank_cost=} + {cost/len(model.x)=}")
    return rank_cost + cost / len(model.x)
# Creta optim and model
opt = Optimizer(20, [2,0], max_fevals=1000)
opt.vmin = prob.lower
opt.vmax = prob.upper
opt.reset(42)

# Gather initial DOE
x = prob.lower + (prob.upper - prob.lower) * np.random.rand(10, opt.num_dimensions)
y = testfunction(prob.function, x)
model = Surrogate(opt.num_dimensions, 1, 20)
model.add_data(x, y, init=True)
model.fit(100)



evals = 0
while not opt.stop():
    for i in range(5):
        if opt.stop():
            break
        x = np.vstack((opt.ask(), opt.position_memories))
        model.eval()
        y = model(torch.from_numpy(x)).detach().numpy().flatten()
        cur, mem = np.split(y, 2)
        opt.aptitude_memories[:] = mem
        opt.tell(cur)
    print("Fitting")
    x = opt.ask()
    y = model(torch.from_numpy(x)).detach().numpy().flatten()
    acost = np.empty(len(x))
    for i, xx in enumerate(x):
        acost[i] = acquisition_function(xx, y[i], model)
    acost = np.argsort(acost)[:5]
    y_true = testfunction(prob.function, x[acost])
    model.add_data(x[acost], y_true)
    model.fit(20)
    evals+= 5
    y[acost] = y_true
    opt.tell(y)
print(evals)
xx = np.linspace(prob.lower, prob.upper, 100)
X, Y = np.meshgrid(xx, xx)
xy = np.stack((X.flat, Y.flat)).T
y = testfunction(prob.function, xy)
xy_pred = model(torch.from_numpy(xy))

import matplotlib.pyplot as plt
fig, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(10, 5))
ax1.matshow(y.reshape(100, 100), extent=[prob.lower, prob.upper, prob.lower, prob.upper], origin='lower')
ax2.matshow(xy_pred.detach().numpy().reshape(100, 100), extent=[prob.lower, prob.upper, prob.lower, prob.upper], origin='lower')
ax2.scatter(*model.x.T, s=5, c='r')
ax1.set_title(f"True Function ({prob.name})")
ax2.set_title("Surrogate")
ax3.set_title("Convergence profile")
ax3.plot(opt.profile)
plt.show()


