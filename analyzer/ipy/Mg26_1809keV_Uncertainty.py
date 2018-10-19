# -*- coding: utf-8 -*-
# <nbformat>3.0</nbformat>

# <codecell>

import ROOT
from ROOT import TFile, TF1, TLatex, TPaveText
import rootnotes
import rootprint
import utils
from utils import ListTable, Gauss, PolN, Ge

# <codecell>

infile = TFile('files/al100_nopp.root', 'READ')
h_g = infile.Get('total_all')
h_x = infile.Get('total_prompt')
fit_g = TF1('fitfncg', 'gaus(0)+expo(3)', 1795., 1825.)
fit_x = TF1('fitfncx', 'gaus(0)+gaus(3)+expo(6)', 340., 355.)

# <codecell>

%%rootprint
fit_g.SetParameters(60., 1808.66, 1., *utils.GuessExpoParams((1800., 51.), (1820., 48.)))
fit_x.SetParameters(700., 346.828, 1., 700., 351., 1., *utils.GuessExpoParams((340., 260.), (355., 220.)))

h_g.GetXaxis().SetRangeUser(1795., 1825.)
res_g = h_g.Fit(fit_g, 'SLN')
h_x.GetXaxis().SetRangeUser(340., 355.)
res_x = h_x.Fit(fit_x, 'SLN')

sig_g = Gauss(fit_result=res_g, first_param=0)
sig_x = Gauss(fit_result=res_x, first_param=0)
ge = Ge()

# <codecell>

Br2p1s = 0.798
BrCapt = 0.609
val = (sig_g.GetCount()/ge.Eff(1808.66)/BrCapt)/(sig_x.GetCount()/ge.Eff(346.828)/Br2p1s)
err = (sig_g.GetCountError()**2+sig_x.GetCountError()**2)**0.5

# <codecell>

print val, val*err

# <codecell>

c = rootnotes.canvas("Hist", (800, 800))
h_g.GetXaxis().SetRangeUser(1750., 1850.)
h_g.SetStats(0)
h_g.SetLineColor(ROOT.kBlack)
h_g.SetTitle('Prompt #gamma (1809 keV) from #mu^{-} Nuclear Capture on ^{27}Al')
h_g.Draw()

tex = TLatex()
tex.DrawLatex(1766., 130.,'#color[{}]{}'.format(ROOT.kBlack, '{^{214}Bi}'))
tex.DrawLatex(1810., 102., '#color[{}]{}'.format(ROOT.kBlack, '{^{26}Mg*}'))
txt = TPaveText(0.5, 0.7, 0.9, 0.9, 'NDC BL')
txt.AddText('Branching Ratio of ^{26}Mg*')
txt.AddLine(0.0, 0.75, 1.0, 0.75)
txt.AddText('Lit: 51(5)%')
txt.AddText('Now: 53(5)%')
txt.SetTextSize(0.03)
txt.Draw()
c

