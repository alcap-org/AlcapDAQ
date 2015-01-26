# -*- coding: utf-8 -*-
# <nbformat>3.0</nbformat>

# <codecell>

%matplotlib inline
import ROOT
from ROOT import TH1D
import random
import rootnotes
from utils import Ge
import numpy
import matplotlib.pyplot as plt

# <codecell>

h347  = TH1D('EfficiencyEstimate347',  'Error on 347keV Efficiency;Efficiency;',  200, 0.0003, 0.0007)
h1809 = TH1D('EfficiencyEstimate1809', 'Error on 1809keV Efficiency;Efficiency;', 200, 0.0000, 0.0002)
c1    = rootnotes.canvas('c1', (800, 800))
c2    = rootnotes.canvas('c2', (800, 800))
ge    = Ge()

# <codecell>

L = numpy.linalg.cholesky(ge.eff_cov)
M = (ge.eff_0, ge.eff)
val = [[], []]
for i in xrange(100000):
    rand = L.dot((random.gauss(0, 1.), random.gauss(0, 1.))) + M
    val[0].append(rand[0])
    val[1].append(rand[1])

# <codecell>

c1.cd(); h347.Draw();
c2.cd(); h1809.Draw();

# <codecell>

fig  = plt.figure()
axes = fig.add_axes((0.1, 0.1, 0.8, 0.8))
axes.plot(val[0], val[1], 'r')

# <codecell>

numpy.cov(val)

# <codecell>


