import numpy as np
from smt.sampling_methods import LHS
from smt.surrogate_models import KRG, RBF

class Surrogate():
    def __init__(self, num_dimensions, type="krg") -> None:
        if type == "krg":
            self.model = KRG(theta0=[1e-2]*num_dimensions, print_prediction = False, print_global = False)
        elif type == "rbf":
            self.model = RBF(d0=num_dimensions, print_prediction = False, print_global = False)


    def add_data(self, x, y, init=False) -> None:
        ''' Append points to the surrogate model. '''
        if init:
            self.x = x.copy()
            self.y = y.copy().reshape(-1, 1)
        else:
            self.x = np.vstack((self.x, x))
            self.y = np.vstack((self.y, y.reshape(-1, 1)))

    def train(self):
        ''' Fit the surrogate model. '''
        self.model.set_training_values(self.x, self.y)
        self.model.train()

    def predict(self, x, return_std=False):
        if return_std:
            return self.model.predict_values(x).flatten(), np.sqrt(self.model.predict_variances(x)).flatten()
        else:
            return self.model.predict_values(x).flatten()
    
    def predict_gradient(self, x):
        return self.model.predict_derivatives(x, 0).flatten()

