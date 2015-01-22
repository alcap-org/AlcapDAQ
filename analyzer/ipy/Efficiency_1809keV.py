# -*- coding: utf-8 -*-
# <nbformat>3.0</nbformat>

# <codecell>

import ROOT
from ROOT import TH1D
import random
import rootnotes
from utils import Ge

# <codecell>

h347  = TH1D('EfficiencyEstimate347',  'Error on 347keV Efficiency;Efficiency;',  200, 0.0003, 0.0007)
h1809 = TH1D('EfficiencyEstimate1809', 'Error on 1809keV Efficiency;Efficiency;', 200, 0.0000, 0.0002)
c1    = rootnotes.canvas('c1', (800, 800))
c2    = rootnotes.canvas('c2', (800, 800))
ge    = Ge()

# <codecell>

n = 1000000
for i in xrange(n):
    h347 .Fill(random.gauss(*ge.eff_a)*347. **random.gauss(*ge.eff_b))
    h1809.Fill(random.gauss(*ge.eff_a)*1809.**random.gauss(*ge.eff_b))

# <codecell>

c1.cd(); h347 .Draw();
c2.cd(); h1809.Draw();

# <codecell>

c1

# <codecell>

c2

# <codecell>

ge.ErrEff(347.)*ge.Eff(347.)

# <codecell>


