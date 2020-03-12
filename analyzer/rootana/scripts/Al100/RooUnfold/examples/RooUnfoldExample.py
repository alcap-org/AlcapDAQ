#!/usr/bin/env python
# ==============================================================================
#  File and Version Information:
#       $Id$
#
#  Description:
#       Simple example usage of the RooUnfold package using toy MC.
#
#  Author: Tim Adye <T.J.Adye@rl.ac.uk>
#
# ==============================================================================

from ROOT import gRandom, TH1, TH1D, cout
from ROOT import RooUnfoldResponse
from ROOT import RooUnfold
from ROOT import RooUnfoldBayes
# from ROOT import RooUnfoldSvd
# from ROOT import RooUnfoldTUnfold
# from ROOT import RooUnfoldIds

# ==============================================================================
#  Gaussian smearing, systematic translation, and variable inefficiency
# ==============================================================================

def smear(xt):
  xeff= 0.3 + (1.0-0.3)/20*(xt+10.0);  #  efficiency
  x= gRandom.Rndm();
  if x>xeff: return None;
  xsmear= gRandom.Gaus(-2.5,0.2);     #  bias and smear
  return xt+xsmear;

# ==============================================================================
#  Example Unfolding
# ==============================================================================

print "==================================== TRAIN ===================================="
response= RooUnfoldResponse (40, -10.0, 10.0);

#  Train with a Breit-Wigner, mean 0.3 and width 2.5.
for i in xrange(100000):
  xt= gRandom.BreitWigner (0.3, 2.5);
  x= smear (xt);
  if x!=None:
    response.Fill (x, xt);
  else:
    response.Miss (xt);

print "==================================== TEST ====================================="
hTrue= TH1D ("true", "Test Truth",    40, -10.0, 10.0);
hMeas= TH1D ("meas", "Test Measured", 40, -10.0, 10.0);
#  Test with a Gaussian, mean 0 and width 2.
for i in xrange(10000):
  xt= gRandom.Gaus (0.0, 2.0)
  x= smear (xt);
  hTrue.Fill(xt);
  if x!=None: hMeas.Fill(x);

print "==================================== UNFOLD ==================================="
unfold= RooUnfoldBayes     (response, hMeas, 4);    #  OR
# unfold= RooUnfoldSvd     (response, hMeas, 20);     #  OR
# unfold= RooUnfoldTUnfold (response, hMeas);         #  OR
# unfold= RooUnfoldIds     (response, hMeas, 3);      #  OR

hReco= unfold.Hreco();
unfold.PrintTable (cout, hTrue);
hReco.Draw();
hMeas.Draw("SAME");
hTrue.SetLineColor(8);
hTrue.Draw("SAME");
