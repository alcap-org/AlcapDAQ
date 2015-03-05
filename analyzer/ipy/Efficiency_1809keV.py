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

h347  = TH1D('EfficiencyEstimate347',  'Error on 347keV Efficiency;Efficiency;',  800, 0.0001, 0.0007)
h400  = TH1D('EfficiencyEstimate400',  'Error on 400keV Efficiency;Efficiency;',  800, 0.0001, 0.0007)
h844  = TH1D('EfficiencyEstimate844',  'Error on 844keV Efficiency;Efficiency;',  800, 0.0001, 0.0007)
h1809 = TH1D('EfficiencyEstimate1809', 'Error on 1809keV Efficiency;Efficiency;', 800, 0.0001, 0.0007)
c1    = rootnotes.canvas('c1', (800, 800))
c2    = rootnotes.canvas('c2', (800, 800))
c3    = rootnotes.canvas('c3', (800, 800))
c4    = rootnotes.canvas('c4', (800, 800))
ge    = Ge()

# <codecell>

L = numpy.linalg.cholesky(ge.eff_cov)
M = (ge.eff_0, ge.eff_1)
for i in xrange(1000000):
    rand = L.dot((random.gauss(0, 1.), random.gauss(0, 1.))) + M
    h347 .Fill(rand[0]*347. **rand[1])
    h400 .Fill(rand[0]*400. **rand[1])
    h844 .Fill(rand[0]*844. **rand[1])
    h1809.Fill(rand[0]*1809.**rand[1])

# <codecell>

c1.cd(); h347 .Draw();
c2.cd(); h400 .Draw();
c3.cd(); h844 .Draw();
c4.cd(); h1809.Draw();

# <codecell>

print "347  keV : ", h347 .GetRMS()/h347 .GetBinCenter(h347 .GetMaximumBin())*100., "%"
print "400  keV : ", h400 .GetRMS()/h400 .GetBinCenter(h400 .GetMaximumBin())*100., "%"
print "844  keV : ", h844 .GetRMS()/h844 .GetBinCenter(h844 .GetMaximumBin())*100., "%"
print "1809 keV : ", h1809.GetRMS()/h1809.GetBinCenter(h1809.GetMaximumBin())*100., "%"

# <codecell>

print ge.Eff(347.), ge.Eff(400.), ge.Eff(844.), ge.Eff(1809.)

# <codecell>

c1

# <codecell>

c2

# <codecell>

c3

# <codecell>

c4

# <codecell>


