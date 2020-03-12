//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id$
//
// Description:
//      Test Harness class for the RooUnfold package using 2D toy MC.
//      Inherits from RooUnfoldTestHarness.
//
// Author: Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

#ifndef ROOUNFOLDTESTHARNESS2D_HH
#define ROOUNFOLDTESTHARNESS2D_HH

#include "RooUnfoldTestHarness.h"
#if !defined(__CINT__) || defined(__MAKECINT__)
#include "TH2.h"
#endif

class TH1D;

class RooUnfoldTestHarness2D : public RooUnfoldTestHarness {
public:
  // Parameters
  Int_t    ftrainy, ftesty, nty, nmy;
  Double_t ylo, yhi, mtrainy, wtrainy, btrainy, mtesty, wtesty, btesty, mscaley;
  Double_t effylo, effyhi, fakeylo, fakeyhi, rotxy, ybias, ysmear;

  // Data
  TH1D *hTrainX, *hTrainTrueX, *hTrainFakeX, *hTrueX, *hMeasX, *hRecoX, *hFakeX, *hPullsX;
  TH1D *hTrainY, *hTrainTrueY, *hTrainFakeY, *hTrueY, *hMeasY, *hRecoY, *hFakeY, *hPullsY, *hPDFy, *hTestPDFy;

  // Constructors
  RooUnfoldTestHarness2D (const char* name= "RooUnfoldTest2D");
  RooUnfoldTestHarness2D (const char* name, const char* args);
  RooUnfoldTestHarness2D (const char* name, int argc, const char* const* argv);
  virtual ~RooUnfoldTestHarness2D() {}

  // Methods and functions
  virtual void  Parms (ArgVars& args);
  virtual Int_t Train();
  virtual void  TrainResults();
  virtual Int_t Test();
  virtual void  Results();
  static  void  Rot     (Double_t& x, Double_t& y, Double_t angle, Double_t x0, Double_t y0);
  virtual bool  Eff2D   (Double_t  x, Double_t  y) const;
  virtual Int_t Fakes2D (TVectorD& xfake, TVectorD& yfake, Int_t nfake) const;
  virtual void  Smear2D (Double_t& x, Double_t& y) const;
  virtual void  Reset();
  virtual void  Init();
  virtual Int_t CheckParms();
  static TH1D* ProjectionX (const TH1* h, const char* name="_px", const char* title=0, Option_t* opt="");
  static TH1D* ProjectionY (const TH1* h, const char* name="_py", const char* title=0, Option_t* opt="");
};

#ifndef NOINLINE
#include "RooUnfoldTestHarness2D.icc"
#endif

#endif
