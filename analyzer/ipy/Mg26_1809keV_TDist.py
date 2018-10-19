# -*- coding: utf-8 -*-
# <nbformat>3.0</nbformat>

# <codecell>

%matplotlib inline
import ROOT
from ROOT import TFile
import rootnotes
import rootprint
import matplotlib.pyplot as plt
from utils import ListTable

# <codecell>

infile = TFile('files/mg26_1809keV.root', 'READ')
timehist = infile.Get('GeSpectrum/hTimePeak1809').ProjectionX()

# <codecell>

res = ListTable()
res.append(('Lower Limit','Lifetime','Error','Chi2/Ndf'))
xaxis = timehist.GetXaxis()
maxlim = 4000
for minlim in range(0,2100,10):
    xaxis.SetRangeUser(minlim, maxlim)
    fitres = timehist.Fit('expo', 'LS')
    res.append((minlim,-1./fitres.Parameter(1),fitres.ParError(1)/fitres.Parameter(1)**2,fitres.Chi2()/fitres.Ndf()))

# <codecell>

fig = plt.figure()
axes = []

axes.append(fig.add_axes([0.1, 0.1, 1.5, 1.5]))
axes[0].errorbar([row[0] for row in res[1:]], [row[1] for row in res[1:]], yerr=[row[2] for row in res[1:]], color='blue')
axes[0].set_xlabel('Lower Bound of Fit (ns)', fontsize=18)
axes[0].set_ylabel('Lifetime', fontsize=18, color='blue')
axes[0].set_title('Lifetime of 1809keV in Al100 (Endpoint to fit 4000 ns)')

axes.append(axes[0].twinx())
axes[1].plot([row[0] for row in res[1:]], [row[3] for row in res[1:]], color='red')
axes[1].set_ylabel(r'$\chi^2/$NDF', fontsize=18, color='red')
axes[1].axhline(1, color='red', ls='--')

# <codecell>

res

