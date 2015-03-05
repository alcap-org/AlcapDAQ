# -*- coding: utf-8 -*-
# <nbformat>3.0</nbformat>

# <codecell>

import ROOT
from ROOT import TFile, TF1, TLegend, TArrow, TLatex
import rootnotes
import rootprint
from numpy import pi
from utils import ListTable, Ge

# <codecell>

histfile = TFile('files/al100_nopp.root', 'READ')
hall     = histfile.Get('total_all')
hprompt  = histfile.Get('total_prompt')
hoot     = histfile.Get('total_oot')

fall     = TF1('fall', 'gaus(0)+gaus(3)+pol1(6)')
fprompt  = TF1('fprompt', 'gaus(0)+gaus(3)+pol1(6)')
foot     = TF1('oot', 'gaus(0)+pol1(3)')

leg = TLegend(0.10,0.65,0.90,0.90)

text = TLatex()
axrayprompt = TArrow(347., 400., 347., 800.)
acapprompt  = TArrow(351.7, 200., 351.7, 500.)

# <codecell>

hall.SetLineColor(ROOT.kBlack); hprompt.SetLineColor(ROOT.kGreen); hoot.SetLineColor(ROOT.kBlue);
hall.GetXaxis().SetRangeUser(340., 355.); hprompt.GetXaxis().SetRangeUser(340., 355.); hoot.GetXaxis().SetRangeUser(340., 355.);
hall.SetMinimum(2.e2); hprompt.SetMinimum(2.e2); hoot.SetMinimum(2.e2);
hall.SetMaximum(1.e4); hprompt.SetMaximum(1.e4); hoot.SetMaximum(1.e4);
hall.SetStats(0);      hprompt.SetStats(0); hoot.SetStats(0);

fall.SetParameters(900., 347., 1., 900., 351., 1., 1100., 0.);
fprompt.SetParameters(700., 347., 1., 500., 351., 1., 300., 0.);
foot.SetParameters(200., 351., 1., 1000., 0.);

leg.AddEntry(hall,    'No time cut',                               'L');
leg.AddEntry(hprompt, 'Within 0.3 #mus of entering muons',           'L');
leg.AddEntry(hoot,    'More than 5 #mus away from entering muon', 'L');
leg.SetTextSize(0.035)

# <codecell>

%%rootprint
resall = hall.Fit(fall, 'SQN')
resprompt = hprompt.Fit(fprompt, 'SQN')
resoot = hoot.Fit(foot, 'SQN')

# <codecell>

c = rootnotes.canvas("Histograms", (800, 800))
hall.Draw()
hprompt.Draw("SAME")
hoot.Draw("SAME")
leg.Draw("SAME")

text.DrawLatex(346.3, 2100., '#color[{}]{}'.format(ROOT.kBlack, '{Al_{K#alpha}}'));
text.DrawLatex(346.3, 500.,  '#color[{}]{}'.format(ROOT.kGreen, '{Al_{K#alpha}}'));
text.DrawLatex(350.0, 250.,  '#color[{}]{}'.format(ROOT.kGreen, '{^{207}Tl*}'))
text.DrawLatex(350.7, 750.,  '#color[{}]{}'.format(ROOT.kBlue,  '{^{214}Pb}'));

# <codecell>


c.SetLogy()
c

# <codecell>


