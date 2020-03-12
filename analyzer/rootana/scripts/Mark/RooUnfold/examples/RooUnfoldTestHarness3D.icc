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

#ifndef ROOUNFOLDTESTHARNESS3D_ICC
#define ROOUNFOLDTESTHARNESS3D_ICC

#include "RooUnfoldTestHarness3D.h"

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <iostream>

#include "TROOT.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TH1D.h"
#include "TH3D.h"
#include "TProfile.h"
#include "TVectorD.h"
#include "TLine.h"

#include "RooUnfoldResponse.h"
#endif

#include "ArgVars.h"

#if !defined(__CINT__) || defined(__MAKECINT__)
using std::cerr;
using std::endl;
#endif

//==============================================================================
// Test parameters
//==============================================================================

void RooUnfoldTestHarness3D::Parms (ArgVars& args)
{
  RooUnfoldTestHarness2D::Parms (args);
  args.SetDefault ("onepage", 10);
  args.SetDefault ("ntx",     10);
  args.SetDefault ("nty",      8);
  args.SetDefault ("ftestx",   2);
  args.SetDefault ("wtestx", 3.0);
  args.SetDefault ("wtesty", 3.0);
  args.SetDefault ("effxlo", 0.9);
  args.Add ("ftrainz", &ftrainz,      0, "training Z PDF");
  args.Add ("ftestz",  &ftestz,       6, "test Z PDF");
  args.Add ("ntz",     &ntz,          6, "#truth Z bins");
  args.Add ("nmz",     &nmz,         -1, "#measured Z bins", "ntz");
  args.Add ("zlo",     &zlo,        0.0, "Z range minimum");
  args.Add ("zhi",     &zhi,       10.0, "Z range maximum");
  args.Add ("mtrainz", &mtrainz,    5.0, "training signal PDF Z mean");
  args.Add ("wtrainz", &wtrainz,    1.0, "training signal PDF Z width");
  args.Add ("btrainz", &btrainz,    0.2, "training Z PDF background fraction");
  args.Add ("mtestz",  &mtestz,     4.0, "test signal PDF Z mean");
  args.Add ("wtestz",  &wtestz,     1.5, "test signal PDF Z width");
  args.Add ("btestz",  &btestz,     0.3, "test Z PDF background fraction");
  args.Add ("effzlo",  &effzlo,    0.75, "efficiencz at zlo");
  args.Add ("effzhi",  &effzhi,    0.95, "efficiencz at zhi");
  args.Add ("fakezlo", &fakezlo,    0.6, "fraction of fakes at zlo");
  args.Add ("fakezhi", &fakezhi,    0.1, "fraction of fakes at zhi");
  args.Add ("rotxz",   &rotxz,      0.4, "rotation angle in XZ plane");
  args.Add ("rotyz",   &rotyz,      0.4, "rotation angle in YZ plane");
  args.Add ("zbias",   &zbias,      1.0, "shift in Z");
  args.Add ("zsmear",  &zsmear,     0.5, "Z smearing width in bins");
}

//==============================================================================
// Train: create response matrix
//==============================================================================

Int_t RooUnfoldTestHarness3D::Train()
{
  TVectorD xtrue(ntrain), ytrue(ntrain), ztrue(ntrain);

  hPDFx= Generate (xtrue, "trainpdfx", "Training PDF X", ntx, ftrainx, ntrain, xlo, xhi, mtrainx, wtrainx, btrainx, xbias, xsmear);
  if (!hPDFx) return 0;
  hPDFy= Generate (ytrue, "trainpdfy", "Training PDF Y", nty, ftrainy, ntrain, ylo, yhi, mtrainy, wtrainy, btrainy, ybias, ysmear);
  if (!hPDFy) return 0;
  hPDFz= Generate (ztrue, "trainpdfz", "Training PDF Z", ntz, ftrainz, ntrain, zlo, zhi, mtrainz, wtrainz, btrainz, zbias, zsmear);
  if (!hPDFz) return 0;

  hTrainTrue= new TH3D ("traintrue", "Training Truth", ntx, xlo, xhi, nty, ylo, yhi, ntz, zlo, zhi);
  hTrainTrue->SetLineColor(kBlue);
  hTrain= new TH3D ("train", "Training Measured", nmx, xlo, xhi, nmy, ylo, yhi, nmz, zlo, zhi);
  hTrain->SetLineColor(kRed);

  response->Setup (hTrain, hTrainTrue);

  for (Int_t i= 0; i<ntrain; i++) {
    Double_t xt= (*&xtrue)[i], yt= (*&ytrue)[i], zt= (*&ztrue)[i];
    Fill (hTrainTrue, xt, yt, zt);

    if (Eff3D (xt, yt, zt)) {
      Double_t x= xt, y= yt, z= zt;
      Smear3D (x, y, z);
      Fill (hTrain, x, y, z);
      response->Fill (x, y, z, xt, yt, zt);
    } else {
      response->Miss (xt, yt, zt);
    }
  }

  TVectorD xfake, yfake, zfake;
  Int_t nfake= Fakes3D (xfake, yfake, zfake, ntrain);
  if (nfake>0) {
    hTrainFake= new TH3D ("trainfake", "Training Fakes", nmx, xlo, xhi, nmy, ylo, yhi, nmz, zlo, zhi);
    hTrainFake->SetLineColor(93);
    for (Int_t i= 0; i<nfake; i++) {
      Double_t xf= (*&xfake)[i], yf= (*&yfake)[i], zf= (*&zfake)[i];
      Fill (hTrain,     xf, yf, zf);
      Fill (hTrainFake, xf, yf, zf);
      response  ->Fake (xf, yf, zf);
    }
    hTrainFakeX= ProjectionX (hTrainFake, "hTrainFakeX", "Training Fakes X");
    hTrainFakeY= ProjectionY (hTrainFake, "hTrainFakeY", "Training Fakes Y");
    hTrainFakeZ= ProjectionZ (hTrainFake, "hTrainFakeZ", "Training Fakes Z");
  }

  hTrainTrueX= ProjectionX (hTrainTrue, "hTrainTrueX", "Training X");
  hTrainTrueY= ProjectionY (hTrainTrue, "hTrainTrueY", "Training Y");
  hTrainTrueZ= ProjectionZ (hTrainTrue, "hTrainTrueZ", "Training Z");

  hTrainX=     ProjectionX (hTrain,     "hTrainX",     "Training Measured X");
  hTrainY=     ProjectionY (hTrain,     "hTrainY",     "Training Measured Y");
  hTrainZ=     ProjectionZ (hTrain,     "hTrainZ",     "Training Measured Z");

  return 1;
}


void RooUnfoldTestHarness3D::TrainResults()
{
  if (mscalex!=1.0) {
    // should really use Sumw2() for these, but we don't show errors
    hTrainX->Scale(mscalex,"nosw2");
    if (hTrainFakeX) hTrainFakeX->Scale(mscalex,"nosw2");
  }
  if (mscaley!=1.0) {
    hTrainY->Scale(mscaley,"nosw2");
    if (hTrainFakeY) hTrainFakeY->Scale(mscaley,"nosw2");
  }
  if (mscalez!=1.0) {
    hTrainZ->Scale(mscalez,"nosw2");
    if (hTrainFakeZ) hTrainFakeZ->Scale(mscalez,"nosw2");
  }
  setmax (hTrainTrueX, hPDFx, hTrainX, hTrainFakeX);
  setmax (hTrainTrueY, hPDFy, hTrainY, hTrainFakeY);
  setmax (hTrainTrueZ, hPDFz, hTrainZ, hTrainFakeZ);

  if (onepage && (onepage<6 || ipad>=onepage)) return;
  if (onepage) canvas->cd(++ipad);
  hTrainTrueX->Draw();
  hPDFx      ->Draw("LSAME");
  hTrainX    ->Draw("SAME");
  if (hTrainFakeX) hTrainFakeX->Draw("SAME");
  if (!onepage) Legend (lTrain, hPDFx, hTrainTrueX, hTrainFakeX, hTrainX);
  canvas->Update();

  if (onepage && ipad>=onepage) return;
  if (onepage) canvas->cd(++ipad);
  hTrainTrueY->Draw();
  hPDFy      ->Draw("LSAME");
  hTrainY    ->Draw("SAME");
  if (hTrainFakeY) hTrainFakeY->Draw("SAME");
  if (!onepage) lTrain->Draw();
  canvas->Update();

  if (onepage && ipad>=onepage) return;
  if (onepage) canvas->cd(++ipad);
  hTrainTrueZ->Draw();
  hPDFz      ->Draw("LSAME");
  hTrainZ    ->Draw("SAME");
  if (hTrainFakeZ) hTrainFakeZ->Draw("SAME");
  if (!onepage) lTrain->Draw();
  canvas->Update();
}

//==============================================================================
// Test unfolding algorithm
//==============================================================================

Int_t RooUnfoldTestHarness3D::Test()
{
  TVectorD xtest(ntest), ytest(ntest), ztest(ntest);

  hTestPDFx= Generate (xtest, "pdfx", "PDF X", ntx, ftestx, ntest, xlo, xhi, mtestx, wtestx, btestx, xbias, xsmear);
  if (!hTestPDFx) return 0;
  hTestPDFy= Generate (ytest, "pdfy", "PDF Y", nty, ftesty, ntest, ylo, yhi, mtesty, wtesty, btesty, ybias, ysmear);
  if (!hTestPDFy) return 0;
  hTestPDFz= Generate (ztest, "pdfz", "PDF Z", ntz, ftestz, ntest, zlo, zhi, mtestz, wtestz, btestz, zbias, zsmear);
  if (!hTestPDFz) return 0;

  hTrue= new TH3D ("true", "Test Truth", ntx, xlo, xhi, nty, ylo, yhi, ntz, zlo, zhi);
  hTrue->SetLineColor(kBlue);
  hMeas= new TH3D ("meas", "Test Measured", nmx, xlo, xhi, nmy, ylo, yhi, nmz, zlo, zhi);
  hMeas->SetLineColor(kRed);

  for (Int_t i=0; i<ntest ; i++) {
    Double_t xt= (*&xtest)[i], yt= (*&ytest)[i], zt= (*&ztest)[i];
    Fill (hTrue, xt, yt, zt);
    if (Eff3D (xt, yt, zt)) {
      Double_t x= xt, y= yt, z= zt;
      Smear3D (x, y, z);
      Fill (hMeas, x, y, z);
    }
  }

  TVectorD xfake, yfake, zfake;
  Int_t nfake= Fakes3D (xfake, yfake, zfake, ntest);
  if (nfake>0) {
    hFake= new TH3D ("fake", "Test Fakes", nmx, xlo, xhi, nmy, ylo, yhi, nmz, zlo, zhi);
    hFake->SetLineColor(93);
    for (Int_t i= 0; i<nfake; i++) {
      Double_t xf= (*&xfake)[i], yf= (*&yfake)[i], zf= (*&zfake)[i];
      Fill (hMeas,    xf, yf, zf);
      Fill (hFake,    xf, yf, zf);
    }
    hFakeX= ProjectionX (hFake, "hFakeX", "Test Fakes X");
    hFakeY= ProjectionY (hFake, "hFakeY", "Test Fakes Y");
    hFakeZ= ProjectionZ (hFake, "hFakeZ", "Test Fakes Z");
  }

  hTrueX= ProjectionX (hTrue, "hTrueX", "Test X");
  hMeasX= ProjectionX (hMeas, "hMeasX", "Test Measured X");
  hTrueY= ProjectionY (hTrue, "hTrueY", "Test Y");
  hMeasY= ProjectionY (hMeas, "hMeasY", "Test Measured Y");
  hTrueZ= ProjectionZ (hTrue, "hTrueZ", "Test Z");
  hMeasZ= ProjectionZ (hMeas, "hMeasZ", "Test Measured Z");

  hTrueXY= Projection2D (hTrue, "xy", "hTrueXY", "Test XY");
  hMeasXY= Projection2D (hMeas, "xy", "hMeasXY", "Test Measured XY");
  hTrueYZ= Projection2D (hTrue, "yz", "hTrueYZ", "Test YZ");
  hMeasYZ= Projection2D (hMeas, "yz", "hMeasYZ", "Test Measured YZ");
  hTrueXZ= Projection2D (hTrue, "xz", "hTrueXZ", "Test XZ");
  hMeasXZ= Projection2D (hMeas, "xz", "hMeasXZ", "Test Measured XZ");

  return 1;
}

//==============================================================================
// Show results
//==============================================================================

void RooUnfoldTestHarness3D::Results()
{
  if (mscalex!=1.0) {
    // should really use Sumw2() for these, but we don't show errors
    hMeasX->Scale(mscalex,"nosw2");
    if (hFakeX) hFakeX->Scale(mscalex,"nosw2");
  }
  if (mscaley!=1.0) {
    hMeasY->Scale(mscaley,"nosw2");
    if (hFakeY) hFakeY->Scale(mscaley,"nosw2");
  }
  if (mscalez!=1.0) {
    hMeasZ->Scale(mscalez,"nosw2");
    if (hFakeZ) hFakeZ->Scale(mscalez,"nosw2");
  }
  setmax (hTrueX, hTestPDFx, hMeasX, hFakeX);
  setmax (hTrueY, hTestPDFy, hMeasY, hFakeX);
  setmax (hTrueZ, hTestPDFz, hMeasZ, hFakeZ);

  if (hReco) {
    hRecoX= ProjectionX (hReco, "hRecoX", "Reconstructed X", "E");
    hRecoY= ProjectionY (hReco, "hRecoY", "Reconstructed Y", "E");
    hRecoZ= ProjectionZ (hReco, "hRecoZ", "Reconstructed Z", "E");
    hRecoXY= Projection2D (hReco, "xy", "hRecoXY", "Reconstructed XY");
    hRecoYZ= Projection2D (hReco, "yz", "hRecoYZ", "Reconstructed YZ");
    hRecoXZ= Projection2D (hReco, "xz", "hRecoXZ", "Reconstructed XZ");
    hRecoXY->SetMarkerColor(kRed);
    hRecoYZ->SetMarkerColor(kRed);
    hRecoXZ->SetMarkerColor(kRed);
    hRecoX->SetMarkerStyle(kFullDotLarge);
    hRecoY->SetMarkerStyle(kFullDotLarge);
    hRecoZ->SetMarkerStyle(kFullDotLarge);
    setmax (hTrueX, hRecoX);
    setmax (hTrueY, hRecoY);
    setmax (hTrueZ, hRecoZ);
  }

  if (onepage && ipad>=onepage) return;
  if (onepage) canvas->cd(++ipad);
  hTrueX   ->Draw();
  hTestPDFx->Draw("LSAME");
  hMeasX   ->Draw("SAME");
  if (hFakeX) hFakeX->Draw("SAME");
  if (hRecoX) hRecoX->Draw("SAME");
  Legend (lTest, hTestPDFx, hTrueX, hFakeX, hMeasX, hRecoX);
  canvas->Update();

  if (onepage && ipad>=onepage) return;
  if (onepage) canvas->cd(++ipad);
  hTrueY   ->Draw();
  hTestPDFy->Draw("LSAME");
  hMeasY   ->Draw("SAME");
  if (hFakeY) hFakeY->Draw("SAME");
  if (hRecoY) hRecoY->Draw("SAME");
  lTest->Draw();
  canvas->Update();

  if (onepage && ipad>=onepage) return;
  if (onepage) canvas->cd(++ipad);
  hTrueZ   ->Draw();
  hTestPDFz->Draw("LSAME");
  hMeasZ   ->Draw("SAME");
  if (hFakeZ) hFakeZ->Draw("SAME");
  if (hRecoZ) hRecoZ->Draw("SAME");
  lTest->Draw();
  canvas->Update();

  if (!hReco) return;

  if (onepage && ipad>=onepage) return;
  if (onepage) canvas->cd(++ipad);
  gStyle->SetPalette(1,0);
  hCorr->Draw("COLZ");
  canvas->Update();

  hPullsX= new TProfile ("hPullsX", "Pulls X", ntx, xlo, xhi);
  hPullsY= new TProfile ("hPullsY", "Pulls Y", nty, ylo, yhi);
  hPullsZ= new TProfile ("hPullsZ", "Pulls Z", ntz, zlo, zhi);
  for (Int_t ix= 1; ix<=ntx; ix++) {
    for (Int_t iy= 1; iy<=nty; iy++) {
      for (Int_t iz= 1; iz<=ntz; iz++) {
        if ( hReco->GetBinContent(ix,iy,iz)!=0.0             && hReco->GetBinError(ix,iy,iz)>0.0 &&
            (hTrue->GetBinContent(ix,iy,iz)!=0.0 || (doerror && hTrue->GetBinError(ix,iy,iz)>0.0))) {
          Double_t pull= (hReco->GetBinContent(ix,iy,iz) - hTrue->GetBinContent(ix,iy,iz)) / hReco->GetBinError(ix,iy,iz);
          hPullsX->Fill (hReco->GetXaxis()->GetBinCenter(ix), pull);
          hPullsY->Fill (hReco->GetYaxis()->GetBinCenter(iy), pull);
          hPullsZ->Fill (hReco->GetZaxis()->GetBinCenter(iz), pull);
        }
      }
    }
  }

  hPullsX->SetMarkerStyle(kFullDotLarge);
  hPullsY->SetMarkerStyle(kFullDotLarge);
  hPullsZ->SetMarkerStyle(kFullDotLarge);

  if (onepage && ipad>=onepage) return;
  if (onepage) canvas->cd(++ipad);
  hPullsX->Draw("P");
  TLine().DrawLine(hPullsX->GetBinLowEdge(1), 0.0, hPullsX->GetBinLowEdge(ntx+1), 0.0);  // draw a line at pull=0;
  canvas->Update();

  if (onepage && ipad>=onepage) return;
  if (onepage) canvas->cd(++ipad);
  hPullsY->Draw("P");
  TLine().DrawLine(hPullsY->GetBinLowEdge(1), 0.0, hPullsY->GetBinLowEdge(nty+1), 0.0);  // draw a line at pull=0;
  canvas->Update();

  if (onepage && ipad>=onepage) return;
  if (onepage) canvas->cd(++ipad);
  hPullsZ->Draw("P");
  TLine().DrawLine(hPullsZ->GetBinLowEdge(1), 0.0, hPullsZ->GetBinLowEdge(ntz+1), 0.0);  // draw a line at pull=0;
  canvas->Update();

  if (onepage && ipad>=onepage) return;
  if (onepage) canvas->cd(++ipad);
  hReco->Draw();
  canvas->Update();

  if (onepage && (ipad>=onepage || ploterrors)) return;
  if (onepage) canvas->cd(++ipad);
  hTrueXY->Draw();
  if (hRecoXY) hRecoXY->Draw("SAME");
  canvas->Update();

  if (onepage && (ipad>=onepage || ploterrors)) return;
  if (onepage) canvas->cd(++ipad);
  hMeasXY->Draw();
  canvas->Update();

  if (onepage && (ipad>=onepage || ploterrors)) return;
  if (onepage) canvas->cd(++ipad);
  hTrueYZ->Draw();
  if (hRecoYZ) hRecoYZ->Draw("SAME");
  canvas->Update();

  if (onepage && (ipad>=onepage || ploterrors)) return;
  if (onepage) canvas->cd(++ipad);
  hMeasYZ->Draw();
  canvas->Update();

  if (onepage && (ipad>=onepage || ploterrors)) return;
  if (onepage) canvas->cd(++ipad);
  hTrueXZ->Draw();
  if (hRecoXZ) hRecoXZ->Draw("SAME");
  canvas->Update();

  if (onepage && (ipad>=onepage || ploterrors)) return;
  if (onepage) canvas->cd(++ipad);
  hMeasXZ->Draw();
  canvas->Update();
}

//==============================================================================
// Gaussian smearing, systematic translation, and variable inefficiency
//==============================================================================

bool RooUnfoldTestHarness3D::Eff3D (Double_t x, Double_t y, Double_t z) const
{
  return Eff (x, xlo, xhi, effxlo, effxhi) &&
         Eff (y, ylo, yhi, effylo, effyhi) &&
         Eff (z, zlo, zhi, effzlo, effzhi);
}

Int_t RooUnfoldTestHarness3D::Fakes3D (TVectorD& xfake, TVectorD& yfake, TVectorD& zfake, Int_t nfake) const
{
  Int_t nfakex= Fakes2D (xfake, yfake, nfake);
  Int_t nfakez= Fakes (zfake, nmz, nfake, zlo, zhi, fakezlo, fakezhi);
  if        (nfakex<nfakez) {
    zfake.ResizeTo(nfakex);
  } else if (nfakez<nfakex) {
    xfake.ResizeTo(nfakez);
    yfake.ResizeTo(nfakez);
    nfakex= nfakez;
  }
  return nfakex;
}

void RooUnfoldTestHarness3D::Smear3D (Double_t& x, Double_t& y, Double_t& z) const
{
  x= Smear (x, ntx, xlo, xhi, xbias, xsmear);
  y= Smear (y, nty, ylo, yhi, ybias, ysmear);
  z= Smear (z, ntz, zlo, zhi, zbias, zsmear);
  if (!addbias) return;
  Rot (x, y, rotxy, 0.5*(xlo+xhi), 0.5*(ylo+yhi));
  Rot (x, z, rotxz, 0.5*(xlo+xhi), 0.5*(zlo+zhi));
  Rot (y, z, rotyz, 0.5*(ylo+yhi), 0.5*(zlo+zhi));
}

//==============================================================================
// Constructors
//==============================================================================

RooUnfoldTestHarness3D::RooUnfoldTestHarness3D (const char* name)
  : RooUnfoldTestHarness2D(name)
{
  Reset();
  SetDefaults();
}

RooUnfoldTestHarness3D::RooUnfoldTestHarness3D (const char* name, int argc, const char* const* argv)
  : RooUnfoldTestHarness2D(name)
{
  Reset();
  error= SetArgs (argc, argv);
}

RooUnfoldTestHarness3D::RooUnfoldTestHarness3D (const char* name, const char* args)
  : RooUnfoldTestHarness2D(name)
{
  Reset();
  const char* const argv[]= { name, args };
  error= SetArgs (2, argv, true);
}

//==============================================================================
// Utility routines
//==============================================================================

void RooUnfoldTestHarness3D::Reset()
{
  RooUnfoldTestHarness2D::Reset();
  hTrainZ= hTrainTrueZ= hTrueZ= hTrainFakeZ= hFakeZ= hMeasZ= hRecoZ= hPDFz= hTestPDFz= hPullsZ= 0;
  hTrueXY= hTrueYZ= hTrueXZ= hMeasXY= hMeasYZ= hMeasXZ= hRecoXY= hRecoYZ= hRecoXZ= 0;
  dim= 3;
}

void RooUnfoldTestHarness3D::Init()
{
  RooUnfoldTestHarness2D::Init();
  if (ntz==-1) ntz= nty;
  if (nmz==-1) nmz= ntz;
  if (overflow==2) {
    Double_t zwid= (zhi-zlo) / ntz;
    zlo -= zwid;
    zhi += zwid;
    ntz += 2;
    nmz += Int_t (2.0*(Double_t(nmz)/Double_t(ntz)) + 0.5);
  }
  mscalez= (nmz==ntz) ? 1.0 : Double_t(nmz)/Double_t(ntz);
  ntbins= ntx*nty*ntz;
  nmbins= nmx*nmy*nmz;
}

Int_t RooUnfoldTestHarness3D::CheckParms()
{
  RooUnfoldTestHarness2D::CheckParms();
  if (nmz<=0)     {cerr << "Error: nmz ("    << nmz    << ") <= 0"                  << endl; error = 2;}
  if (ntz<=0)     {cerr << "Error: ntz ("    << ntz    << ") <= 0"                  << endl; error = 2;}
  if (zlo >= zhi) {cerr << "Error: zlo ("    << zlo    << ") >= zhi(" << zhi << ")" << endl; error = 2;}
  if (ftrainz<0)  {cerr << "Error: ftrainz ("<< ftrainz<< ") < 0"                   << endl; error = 2;}
  if (ftestz<0)   {cerr << "Error: ftestz (" << ftestz << ") < 0"                   << endl; error = 2;}
  return error;
}

TH1D* RooUnfoldTestHarness3D::Projection1D (const TH1* h, Option_t* xyz, const char* name, const char* title, Option_t* opt)
{
  const TH3* h3= dynamic_cast<const TH3*>(h);
  TString opts (xyz);
  opts += opt;
  opts += "NUFNOF";
  TH1D* h1= dynamic_cast<TH1D*>(h3->Project3D (opts));
  h1->SetLineColor (h3->GetLineColor());  // doesn't seem to be copied in older versions of ROOT
  if (h1 && name)  h1->SetName  (name);
  if (h1 && title) h1->SetTitle (title);
  return h1;
}

TH2D* RooUnfoldTestHarness3D::Projection2D (const TH1* h, Option_t* xyz, const char* name, const char* title, Option_t* opt)
{
  const TH3* h3= dynamic_cast<const TH3*>(h);
  TString opts (xyz);
  opts += opt;
  opts += "NUFNOF";
  TH2D* h2= dynamic_cast<TH2D*>(h3->Project3D (opts));
  if (h2 && name)  h2->SetName  (name);
  if (h2 && title) h2->SetTitle (title);
  return h2;
}

#endif
