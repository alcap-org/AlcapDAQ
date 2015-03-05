# -*- coding: utf-8 -*-
# <nbformat>3.0</nbformat>

# <markdowncell>

# For a good muon, we want not other muon $\pm10\mu$s.
# If we define $t_0$ as 10$\mu$s before an entering muon,
# then the chance $(p)$ of some number $(n)$ of muons appearing in
# the next 20$\mu$s is a Poisson distribution
# 
# $$p(n)=\frac{\lambda^n e^{-\lambda}}{n!}$$
# 
# The mean number of muons in that time window is going to be the product
# of the beam rate $(R)$ and the length of the pileup
# protection window $(t_{pp})$.
# This is beacause the beam rate is defined as the inverse of the
# mean time between muons in the beam. We want the number of muons
# in that time window to be $n=0$, so the probability
# that any given muon is a good muon is
# 
# $$p(0)=e^{-Rt_{pp}}$$
# 
# We want to maximize the number of good muons ($N$) we get in a run,
# which is the probability of a muon being good multiplied by the
# number of muons in the run. The number of muons in the run
# is the product of the beam rate multiplied by the run length ($T$).
# So the number of good muons is
# 
# $$N=e^{-Rt_{pp}}RT$$
# 
# We want to find the maximimum with respect to the beam rate:
# 
# $$
# \frac{dN}{dR} = Te^{-Rt_{pp}} - RTt_{pp}e^{-Rt_{pp}}
# $$
# 
# The maximum occurs at $\frac{dN}{dR}(R=R_{ideal})=0$, so
# 
# $$
# R_{ideal}=\frac{1}{t_{pp}}
# $$
# 
# We can see the plot below.

# <codecell>

%matplotlib inline
import matplotlib.pyplot as plt
import numpy as np

# <codecell>

def N(R, t_pp = 10e-6, T = 10.*60.):
    return np.exp(-R*2.*t_pp)*R*T
t_pp = 10e-6
T    = 10.*60.
R    = [ x/(100.*t_pp) for x in range(200) ]

# <codecell>

fig  = plt.figure()
axes = fig.add_axes([0.1, 0.1, 0.8, 0.8])

axes.plot(R, [N(r) for r in R], 'r');
axes.set_title('Pileup Window $t_{pp}=\pm10\mu s$')
axes.set_xlabel('Beam Rate (Hz)')
axes.set_ylabel('Number of Pileup Protected Muons')

