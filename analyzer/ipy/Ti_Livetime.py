# -*- coding: utf-8 -*-
# <nbformat>3.0</nbformat>

# <codecell>

import ROOT
from ROOT import TFile, TF1, TLegend, TCanvas, TLatex, TPaveText, gStyle
import rootnotes
import rootprint
from utils import Ge, Gauss
import random

# <rawcell>

# Energies (and intensities) of interest
# Prompt (Lyman and Balmer) are paired with relative intensities, SemiPrompt and Delayed with absolute-per-capture
# Delayed also includes yield per muon capture for that isotope

# <codecell>

Al2p1s     = 347.
Lyman      = ((942., 1.), (1122., 0.102), (1189., 0.031), (1220., 0.036), (1236., 0.041), (1246., 0.023))
Balmer     = ((190, 1.), (179., 0.018), (255., 0.180), (244., 0.006), (286., 0.099), (275., 0.006), (302., 0.081), (312., 0.055))
SemiPrompt = ((121.41, 0.105), (130.94, 0.104), (370.29, 0.122), (807.79, 0.13))
Delayed    = {'Sc48' : (0.111, ((983.517, 1.001  ), (1037.599, 0.976  ), (1312.096, 1.001))),
              'Sc47' : (0.130, ((159.377, 0.683  ),                                       )),
              'Sc46' : (0.081, ((889.277, 0.99984), (1120.545, 0.99987)                   )) }

# <codecell>

BrCap_Al  = 0.572
BrCap_Ti  = 0.853
Br2p1s_Al = 0.798

# <codecell>

ge = Ge()

# <codecell>

infile            = TFile('files/al100_nopp.root', 'READ')
source_prompt     = infile.Get('total_prompt')
source_delayed    = infile.Get('total_oot')
source_semiprompt = infile.Get('total_all')
result_prompt     = source_prompt.Clone('total_prompt_clone')
result_delayed    = source_delayed.Clone('total_oot_clone')
result_semiprompt = source_semiprompt.Clone('total_all_clone')

# <rawcell>

# Count the number of muons

# <codecell>

fit = TF1('fit', 'gaus(0)+gaus(3)+pol1(6)')
source_prompt.GetXaxis().SetRangeUser(340., 355.)
fit.SetParameters(650., Al2p1s, 1., 350., 351.5, 1., 250., 0.)
res = source_prompt.Fit(fit, 'S')
source_prompt.GetXaxis().UnZoom()
xray_peak = Gauss(fit_result=res, first_param=0)

nstops = xray_peak.GetCount()/ge.Eff(Al2p1s)/Br2p1s_Al/source_prompt.GetBinWidth(1)

# <codecell>

PromptGammas     = []
DelayedGammas    = []
SemiPromptGammas = []
for it in Balmer+Lyman:
    PromptGammas.append((it[0], nstops*it[1]*ge.Eff(it[0])))
for it in Delayed.values():
    for jt in it[1]:
        DelayedGammas.append((jt[0], nstops*it[0]*jt[1]*ge.Eff(jt[0])))
SemiPromptGammas = []
for it in SemiPrompt:
    SemiPromptGammas.append((it[0], nstops*it[1]*ge.Eff(it[0])))

# <codecell>

gen = TF1('gen', 'gaus(0)')
for it in PromptGammas:
    gen.SetParameters(1., it[0], 1.)
    result_prompt    .FillRandom('gen', int(it[1]))
for it in DelayedGammas:
    gen.SetParameters(1., it[0], 1.)
    result_delayed   .FillRandom('gen', int(it[1]))
for it in SemiPromptGammas:
    gen.SetParameters(1., it[0], 1.)
    result_semiprompt.FillRandom('gen', int(it[1]))

# <codecell>

c1 = rootnotes.canvas('Delayed Gamma Rays', (1600, 800))
c2 = rootnotes.canvas('SemiPrompt Gamma Rays', (1600, 800))
c3 = rootnotes.canvas('Prompt X rays',(1600, 800))
c1.Divide(2)
c2.Divide(2); c2.cd(2).Divide(1,2)
c3.Divide(2)

# <codecell>

#c1.Clear('D')
#c2.Clear('D')
#c3.Clear('D')

tex = TLatex()

txt1   = TLegend(0.1, 0.1, 0.5, 0.5)
txt2   = TLegend(0.1, 0.1, 0.5, 0.5)
txt3_1 = TLegend(0.43, 0.3, 0.9, 0.7)
txt3_2 = TLegend(0.45, 0.5, 0.9, 0.9)

txt1  .SetNColumns(3)
txt2  .SetNColumns(3)
txt3_1.SetNColumns(3)
txt3_2.SetNColumns(3)

txt1  .SetTextSize(0.04)
txt2  .SetTextSize(0.04)
txt3_1.SetTextSize(0.04)
txt3_2.SetTextSize(0.04)



txt1.AddEntry(0, '',   '');  txt1.AddEntry(0, 'Decay',   ''); txt1.AddEntry(0, 'Energy',   '');
txt1.AddEntry(0, 'A:', '');  txt1.AddEntry(0, '^{47}Sc', ''); txt1.AddEntry(0, '159 keV',  '');
txt1.AddEntry(0, 'B:', '');  txt1.AddEntry(0, '^{46}Sc', ''); txt1.AddEntry(0, '889 keV',  '');
txt1.AddEntry(0, 'C:', '');  txt1.AddEntry(0, '^{48}Sc', ''); txt1.AddEntry(0, '984 keV',  '');
txt1.AddEntry(0, 'D:', '');  txt1.AddEntry(0, '^{48}Sc', ''); txt1.AddEntry(0, '1038 keV', '');
txt1.AddEntry(0, 'E:', '');  txt1.AddEntry(0, '^{46}Sc', ''); txt1.AddEntry(0, '1121 keV', '');
txt1.AddEntry(0, 'F:', '');  txt1.AddEntry(0, '^{48}Sc', ''); txt1.AddEntry(0, '1312 keV', '');

txt2.AddEntry(0, '',   '');  txt2.AddEntry(0, 'Product', ''); txt2.AddEntry(0, 'Energy',  '');
txt2.AddEntry(0, 'A:', '');  txt2.AddEntry(0, '^{48}Sc', ''); txt2.AddEntry(0, '121 keV', '');
txt2.AddEntry(0, 'B:', '');  txt2.AddEntry(0, '^{48}Sc', ''); txt2.AddEntry(0, '131 keV', '');
txt2.AddEntry(0, 'C:', '');  txt2.AddEntry(0, '^{48}Sc', ''); txt2.AddEntry(0, '370 keV', '');
txt2.AddEntry(0, 'D:', '');  txt2.AddEntry(0, '^{47}Sc', ''); txt2.AddEntry(0, '808 keV', '');

txt3_1.AddEntry(0, '', '');   txt3_1.AddEntry(0, 'Transition', '');      txt3_1.AddEntry(0, 'Energy', '');
txt3_1.AddEntry(0, 'A:', ''); txt3_1.AddEntry(0, '2p#rightarrow1s', ''); txt3_1.AddEntry(0, '942 keV',  '');
txt3_1.AddEntry(0, 'B:', ''); txt3_1.AddEntry(0, '3p#rightarrow1s', ''); txt3_1.AddEntry(0, '1122 keV', '');
txt3_1.AddEntry(0, 'C:', ''); txt3_1.AddEntry(0, '4p#rightarrow1s', ''); txt3_1.AddEntry(0, '1189 keV', '');
txt3_1.AddEntry(0, 'D:', ''); txt3_1.AddEntry(0, '5p#rightarrow1s', ''); txt3_1.AddEntry(0, '1220 keV', '');
txt3_1.AddEntry(0, 'E:', ''); txt3_1.AddEntry(0, '6p#rightarrow1s', ''); txt3_1.AddEntry(0, '1236 keV', '');
txt3_1.AddEntry(0, 'F:', ''); txt3_1.AddEntry(0, '7p#rightarrow1s', ''); txt3_1.AddEntry(0, '1246 keV', '');

txt3_2.AddEntry(0, '', '');   txt3_2.AddEntry(0, 'Transition', '');              txt3_2.AddEntry(0, 'Energy', '');
txt3_2.AddEntry(0, 'G:', ''); txt3_2.AddEntry(0, '3d,s#rightarrow2p', ''); txt3_2.AddEntry(0, '190 keV', '');
txt3_2.AddEntry(0, 'H:', ''); txt3_2.AddEntry(0, '3p#rightarrow2s',   ''); txt3_2.AddEntry(0, '179 keV', '');
txt3_2.AddEntry(0, 'I:', ''); txt3_2.AddEntry(0, '4d,s#rightarrow2p', ''); txt3_2.AddEntry(0, '255 keV', '');
txt3_2.AddEntry(0, 'J:', ''); txt3_2.AddEntry(0, '4p#rightarrow2s',   ''); txt3_2.AddEntry(0, '244 keV', '');
txt3_2.AddEntry(0, 'K:', ''); txt3_2.AddEntry(0, '5d,s#rightarrow2p', ''); txt3_2.AddEntry(0, '286 keV', '');
txt3_2.AddEntry(0, 'L:', ''); txt3_2.AddEntry(0, '5p#rightarrow2s',   ''); txt3_2.AddEntry(0, '275 keV', '');
txt3_2.AddEntry(0, 'M:', ''); txt3_2.AddEntry(0, '6d,s#rightarrow2p', ''); txt3_2.AddEntry(0, '302 keV', '');
txt3_2.AddEntry(0, 'N:', ''); txt3_2.AddEntry(0, '7d,s#rightarrow2p', ''); txt3_2.AddEntry(0, '312 keV', '');


source_prompt    .SetLineColor(ROOT.kRed); result_prompt    .SetLineColor(ROOT.kBlack);
source_delayed   .SetLineColor(ROOT.kRed); result_delayed   .SetLineColor(ROOT.kBlack);
source_semiprompt.SetLineColor(ROOT.kRed); result_semiprompt.SetLineColor(ROOT.kBlack);
source_prompt    .SetStats(0);             result_prompt    .SetStats(0);
source_delayed   .SetStats(0);             result_delayed   .SetStats(0);
source_semiprompt.SetStats(0);             result_semiprompt.SetStats(0);

# <codecell>

leg = TLegend(0.5, 0.7, 0.9, 0.9)
leg.AddEntry(source_delayed, 'Al100 OOT Data'); leg.AddEntry(result_delayed, '+Simulated Ti #gammas');
leg.SetTextSize(0.04)

c1.cd(1)
result_delayed.GetXaxis().SetRangeUser(100., 200.);
result_delayed.SetTitle('Ti Simulated Delayed #gammas (100-200 keV)')
result_delayed.DrawCopy(); source_delayed.DrawCopy('SAME');
tex.SetTextSize(0.05)
tex.DrawLatex(157., 3050., '#color[{}]{}'.format(ROOT.kBlack, '{A}'))
leg.Draw('SAME')
txt1.Draw('SAME')

c1.cd(2)
result_delayed.GetXaxis().SetRangeUser(800., 1400.);
result_delayed.SetTitle('Ti Simulated Delayed #gammas (800-1400 keV)')
result_delayed.Draw(); source_delayed.Draw('SAME');
tex.SetTextSize(0.05)
tex.DrawLatex(880.,  75.,  '#color[{}]{}'.format(ROOT.kBlack, '{B}'))
tex.DrawLatex(975.,  60.,  '#color[{}]{}'.format(ROOT.kBlack, '{C}'))
tex.DrawLatex(1030., 55.,  '#color[{}]{}'.format(ROOT.kBlack, '{D}'))
tex.DrawLatex(1110., 200., '#color[{}]{}'.format(ROOT.kBlack, '{E}'))
tex.DrawLatex(1300., 95.,  '#color[{}]{}'.format(ROOT.kBlack, '{F}'))

c2.cd(1)
result_semiprompt.GetXaxis().SetRangeUser(100., 200.);
result_semiprompt.SetTitle('Ti Simulated Prompt w/Capture #gammas')
result_semiprompt.DrawCopy(); source_semiprompt.DrawCopy('SAME');
tex.SetTextSize(0.05)
tex.DrawLatex(120., 4700., '#color[{}]{}'.format(ROOT.kBlack, '{A}'))
tex.DrawLatex(130., 4600., '#color[{}]{}'.format(ROOT.kBlack, '{B}'))
leg.Draw('SAME')
txt2.Draw('SAME')


result_semiprompt.SetLabelSize(0.08)
c2.cd(2).cd(1)
result_semiprompt.GetXaxis().SetRangeUser(350., 400.);
result_semiprompt.SetTitle(';;');
result_semiprompt.DrawCopy(); source_semiprompt.DrawCopy('SAME');
tex.SetTextSize(0.10)
tex.DrawLatex(368., 1200., '#color[{}]{}'.format(ROOT.kBlack, '{C}'))
c2.cd(2).cd(2)
result_semiprompt.GetXaxis().SetRangeUser(770., 820.);
result_semiprompt.SetTitle(';;');
result_semiprompt.Draw(); source_semiprompt.Draw('SAME');
tex.SetTextSize(0.10)
tex.DrawLatex(805., 280., '#color[{}]{}'.format(ROOT.kBlack, '{D}'))


c3.cd(1)
result_prompt.GetXaxis().SetRangeUser(925., 1250.);
result_prompt.SetTitle('Ti Simulated Lyman Series')
result_prompt.DrawCopy(); source_prompt.DrawCopy('SAME');
tex.SetTextSize(0.05)
tex.DrawLatex(945.,  400., '#color[{}]{}'.format(ROOT.kBlack, '{A}'))
tex.DrawLatex(1115., 100., '#color[{}]{}'.format(ROOT.kBlack, '{B}'))
tex.DrawLatex(1185., 60., '#color[{}]{}'.format(ROOT.kBlack, '{C}'))
tex.DrawLatex(1213., 60., '#color[{}]{}'.format(ROOT.kBlack, '{D}'))
tex.DrawLatex(1228., 60., '#color[{}]{}'.format(ROOT.kBlack, '{E}'))
tex.DrawLatex(1240., 60., '#color[{}]{}'.format(ROOT.kBlack, '{F}'))
leg.Draw('SAME')
txt3_1.Draw('SAME')

c3.cd(2)
result_prompt.GetXaxis().SetRangeUser(150., 350.);
result_prompt.SetTitle('Ti Simulated Balmer Series')
result_prompt.Draw(); source_prompt.Draw('SAME');
tex.SetTextSize(0.05)
tex.DrawLatex(180., 1800., '#color[{}]{}'.format(ROOT.kBlack, '{G}'))
tex.DrawLatex(174., 700., '#color[{}]{}'.format(ROOT.kBlack, '{H}'))
tex.DrawLatex(253., 650., '#color[{}]{}'.format(ROOT.kBlack, '{I}'))
tex.DrawLatex(241., 450., '#color[{}]{}'.format(ROOT.kBlack, '{J}'))
tex.DrawLatex(281., 410., '#color[{}]{}'.format(ROOT.kBlack, '{K}'))
tex.DrawLatex(270., 400., '#color[{}]{}'.format(ROOT.kBlack, '{L}'))
tex.DrawLatex(295., 400., '#color[{}]{}'.format(ROOT.kBlack, '{M}'))
tex.DrawLatex(310., 350., '#color[{}]{}'.format(ROOT.kBlack, '{N}'))
txt3_2.Draw('SAME')

# <codecell>

c1

# <codecell>

c2

# <codecell>

c3

# <codecell>


# <codecell>


