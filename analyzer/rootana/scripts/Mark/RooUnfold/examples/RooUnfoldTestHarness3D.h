//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id$
//
// Description:
//      Test Harness class for the RooUnfold package using 3D toy MC.
//      Inherits from RooUnfoldTestHarness.
//
// Author: Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

#ifndef ROOUNFOLDTESTHARNESS3D_HH
#define ROOUNFOLDTESTHARNESS3D_HH

#include "RooUnfoldTestHarness2D.h"

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "TH3.h"
#endif

class TH1;
class TH1D;

class RooUnfoldTestHarness3D : public RooUnfoldTestHarness2D {
public:
  // Parameters
  Int_t    ftrainz, ftestz, ntz, nmz;
  Double_t zlo, zhi, mtrainz, wtrainz, btrainz, mtestz, wtestz, btestz, mscalez;
  Double_t effzlo, effzhi, fakezlo, fakezhi, rotxz, rotyz, zbias, zsmear;

  // Data
  TH1D *hTrainZ, *hTrainTrueZ, *hTrainFakeZ, *hTrueZ, *hMeasZ, *hRecoZ, *hFakeZ, *hPullsZ, *hPDFz, *hTestPDFz;
  TH2D *hTrueXY, *hTrueYZ, *hTrueXZ, *hMeasXY, *hMeasYZ, *hMeasXZ, *hRecoXY, *hRecoYZ, *hRecoXZ;

  // Constructors
  RooUnfoldTestHarness3D (const char* name= "RooUnfoldTest3D");
  RooUnfoldTestHarness3D (const char* name, const char* args);
  RooUnfoldTestHarness3D (const char* name, int argc, const char* const* argv);
  virtual ~RooUnfoldTestHarness3D() {}

  // Methods and functions
  virtual void  Parms (ArgVars& args);
  virtual Int_t Train();
  virtual void  TrainResults();
  virtual Int_t Test();
  virtual void  Results();
  virtual void  Smear3D (Double_t& x, Double_t& y, Double_t& z) const;
  virtual bool  Eff3D   (Double_t  x, Double_t  y, Double_t  z) const;
  virtual Int_t Fakes3D (TVectorD& xfake, TVectorD& yfake, TVectorD& zfake, Int_t nfake) const;
  virtual void  Reset();
  virtual void  Init();
  virtual Int_t CheckParms();

  static TH1D* Projection1D (const TH1* h, Option_t* xyz, const char* name=0, const char* title=0, Option_t* opt="");
  static TH2D* Projection2D (const TH1* h, Option_t* xyz, const char* name=0, const char* title=0, Option_t* opt="");
  static TH1D* ProjectionX  (const TH1* h, const char* name=0, const char* title=0, Option_t* opt="") {return Projection1D(h,"x",name,title,opt);}
  static TH1D* ProjectionY  (const TH1* h, const char* name=0, const char* title=0, Option_t* opt="") {return Projection1D(h,"y",name,title,opt);}
  static TH1D* ProjectionZ  (const TH1* h, const char* name=0, const char* title=0, Option_t* opt="") {return Projection1D(h,"z",name,title,opt);}
  Int_t Fill (TH1* h, Double_t x, Double_t y, Double_t z) {TH3* h3= dynamic_cast<TH3*>(h); return h3->Fill(x,y,z);}
};

#ifndef NOINLINE
#include "RooUnfoldTestHarness3D.icc"
#endif

#endif
