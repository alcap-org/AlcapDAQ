//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id$
//
// Description:
//      Simple example usage of the RooUnfold package using toy MC.
//
// Authors: Tim Adye <T.J.Adye@rl.ac.uk> and Fergus Wilson <fwilson@slac.stanford.edu>
//
//==============================================================================

#if !(defined(__CINT__) || defined(__CLING__)) || defined(__ACLIC__)
#include <iostream>
using std::cout;
using std::endl;

#include "TRandom.h"
#include "TH1D.h"

#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
//#include "RooUnfoldSvd.h"
//#include "RooUnfoldTUnfold.h"
//#include "RooUnfoldIds.h"
#endif

//==============================================================================
// Global definitions
//==============================================================================

const Double_t cutdummy= -99999.0;

//==============================================================================
// Gaussian smearing, systematic translation, and variable inefficiency
//==============================================================================

Double_t smear (Double_t xt)
{
  Double_t xeff= 0.3 + (1.0-0.3)/20*(xt+10.0);  // efficiency
  Double_t x= gRandom->Rndm();
  if (x>xeff) return cutdummy;
  Double_t xsmear= gRandom->Gaus(-2.5,0.2);     // bias and smear
  return xt+xsmear;
}

//==============================================================================
// Example Unfolding
//==============================================================================

void RooUnfoldExample()
{
  cout << "==================================== TRAIN ====================================" << endl;
  RooUnfoldResponse response (40, -10.0, 10.0);

  // Train with a Breit-Wigner, mean 0.3 and width 2.5.
  for (Int_t i= 0; i<100000; i++) {
    Double_t xt= gRandom->BreitWigner (0.3, 2.5);
    Double_t x= smear (xt);
    if (x!=cutdummy)
      response.Fill (x, xt);
    else
      response.Miss (xt);
  }

  cout << "==================================== TEST =====================================" << endl;
  TH1D* hTrue= new TH1D ("true", "Test Truth",    40, -10.0, 10.0);
  TH1D* hMeas= new TH1D ("meas", "Test Measured", 40, -10.0, 10.0);
  // Test with a Gaussian, mean 0 and width 2.
  for (Int_t i=0; i<10000; i++) {
    Double_t xt= gRandom->Gaus (0.0, 2.0), x= smear (xt);
    hTrue->Fill(xt);
    if (x!=cutdummy) hMeas->Fill(x);
  }

  cout << "==================================== UNFOLD ===================================" << endl;
  RooUnfoldBayes   unfold (&response, hMeas, 4);    // OR
//RooUnfoldSvd     unfold (&response, hMeas, 20);   // OR
//RooUnfoldTUnfold unfold (&response, hMeas);       // OR
//RooUnfoldIds     unfold (&response, hMeas, 1);

  TH1D* hReco= (TH1D*) unfold.Hreco();

  unfold.PrintTable (cout, hTrue);
  hReco->Draw();
  hMeas->Draw("SAME");
  hTrue->SetLineColor(8);
  hTrue->Draw("SAME");
}

#ifndef __CINT__
int main () { RooUnfoldExample(); return 0; }  // Main program when run stand-alone
#endif
