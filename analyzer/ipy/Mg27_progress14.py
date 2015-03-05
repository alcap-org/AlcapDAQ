# -*- coding: utf-8 -*-
# <nbformat>3.0</nbformat>

# <codecell>

import ROOT
from ROOT import TFile, TF1, TLegend
import rootnotes
import rootprint
import utils
from utils import ListTable, Gauss, PolN

# <codecell>

c       = rootnotes.canvas('Mg27 Histogram', (800, 800))
infile  = TFile('files/al100_nopp.root', 'READ')
h       = infile.Get('total_oot')
fit_tot = TF1('fittot', 'gaus(0)+gaus(3)+gaus(6)+expo(9)', 830., 855)
fit_mn  = TF1('fitmn',  'gaus(0)+gaus(3)', 830., 855.)
fit_mg  = TF1('fitmg',  'gaus(0)', 830., 855.)
fit_bg  = TF1('fitbg',  'expo(0)', 830., 855.)
l      = TLegend(0.1, 0.4, 0.45, 0.6)

# <codecell>

h.SetTitle('^{27}Mg Decay Peak in Al 100 Data');
h.Rebin(2);
h.GetXaxis().SetRangeUser(830., 855.);
h.SetMinimum(0)
h.SetLineColor(ROOT.kBlack);
h.SetStats(0);

fit_tot.SetLineColor(ROOT.kBlack);
fit_mn .SetLineColor(ROOT.kRed);
fit_mg .SetLineColor(ROOT.kGreen);
fit_bg .SetLineColor(ROOT.kBlue);
fit_tot.SetLineStyle(7);
fit_mn .SetLineStyle(7);
fit_mg .SetLineStyle(7);
fit_bg .SetLineStyle(7);

l.SetTextSize(0.03);
l.AddEntry(h,       'Al 100#mum data', 'l');
l.AddEntry(fit_tot, 'Fit (All)', 'l');
l.AddEntry(fit_mn,  'Fit (Mn Decay)');
l.AddEntry(fit_mg,  'Fit (Mg Decay)');
l.AddEntry(fit_bg,  'Fit (Background)');

# <codecell>

%%rootprint
fit_tot.SetParameters(60., 835., 1., 70., 844., 1., 80., 847., 1., *utils.GuessExpoParams((830.,220.), (855.,210.)))
res = h.Fit(fit_tot, 'SL')
pars = res.Parameters()

fit_mn.SetParameters(pars[0], pars[1], pars[2], pars[6], pars[7], pars[8])
fit_mg.SetParameters(pars[3], pars[4], pars[5])
fit_bg.SetParameters(pars[9], pars[10])

# <codecell>

mg27 = Gauss(fit_result=res, first_param=3)
print mg27.GetCount(), mg27.GetCountError(), res.Chi2()/res.Ndf()

# <codecell>

h.Draw()
l.Draw('SAME')
fit_mn.Draw('SAME')
fit_mg.Draw('SAME')
fit_bg.Draw('SAME')
c

# <codecell>


# <codecell>


