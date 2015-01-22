# -*- coding: utf-8 -*-
# <nbformat>3.0</nbformat>

# <codecell>

import ROOT
from ROOT import TH1D, TF1, TCanvas
import rootnotes
import rootprint
from utils import ListTable, Gauss, PolN

# <rawcell>

# The numbers below were determines from a fit to the entirety of the Al100 dataset

# <codecell>

peakFe1 = Gauss(2.22483e+01, 8.35193e+02, 8.90065e-01)
peakMg  = Gauss(2.48341e+01, 8.43777e+02, 7.23463e-01)
peakFe2 = Gauss(3.88316e+01, 8.46816e+02, 9.37715e-01)
bg      = PolN ([2.36570e+02, -1.44502e-01])
ndata   = 26517

# <codecell>

specHist = TH1D("specHist", "Ge Spectrum;Energy (keV);Counts", 163, 830., 850.)
fillFunc = TF1("fillFunc", "gaus(0)+gaus(3)+gaus(6)+pol1(9)")
fitFunc  = TF1("fitFunc", "gaus(0)+gaus(3)+gaus(6)+pol1(9)")

# <codecell>

fillFunc.SetParameters(peakFe1.amp, peakFe1.mean, peakFe1.sig,
                       peakMg.amp,  peakMg.mean,  peakMg.sig,
                       peakFe2.amp, peakFe2.mean, peakFe2.sig,
                       *bg.Parameters())

# <codecell>

specHist.Reset()
hists = []
fitres = []
fitgaus = []
for i in range(1, 10):
    fitFunc.SetParameters(i*peakFe1.amp, peakFe1.mean, peakFe1.sig,
                          i*peakMg.amp,  peakMg.mean,  peakMg.sig,
                          i*peakFe2.amp, peakFe2.mean, peakFe2.sig,
                          i*bg.coef[0], bg.coef[1])
    fitFunc .SetParLimits(2, 0., 10.)
    fitFunc .SetParLimits(5, 0., 10.)
    fitFunc .SetParLimits(8, 0., 10.)
    specHist.SetTitle("Ge Spectrum (" + str(i) + "x data)")
    specHist.FillRandom("fillFunc", ndata)
    res = specHist.Fit(fitFunc,"QS")
    fitres  .append(res)
    hists   .append(specHist.Clone("data"+str(i)))
    fitgaus .append(Gauss(fit_result=res, first_param=3))

# <rawcell>

# Make table and canvas for output

# <codecell>

c = rootnotes.canvas("Amount of Data", (1200, 1200))
c.Divide(3,3)
for i in range(9):
    c.cd(i+1)
    hists[i].Draw()

# <codecell>

table = ListTable()
table.append(['Runtime', 'Mg27 Count', 'Fraction Error'])
for i in range(1,10):
    table.append([str(i)+'x', fitgaus[i-1].GetCount(), fitgaus[i-1].GetCountError()])

# <rawcell>

# Print results

# <codecell>

c

# <codecell>

table

# <codecell>


