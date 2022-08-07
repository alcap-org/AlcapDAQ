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

#ifndef ROOUNFOLDTESTHARNESS2D_ICC
#define ROOUNFOLDTESTHARNESS2D_ICC

#include "RooUnfoldTestHarness2D.h"

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <cmath>
#include <iostream>

#include "TROOT.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TRandom.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TVectorD.h"
#include "TLine.h"

#include "RooUnfoldResponse.h"
#endif

#include "ArgVars.h"

#if !defined(__CINT__) || defined(__MAKECINT__)
using std::cerr;
using std::endl;
using std::sin;
using std::cos;
#endif

//==============================================================================
// Test parameters
//==============================================================================

void RooUnfoldTestHarness2D::Parms (ArgVars& args)
{
  RooUnfoldTestHarness::Parms (args);
  args.SetDefault ("ntx",    30);
  args.SetDefault ("onepage", 8);
  args.SetDefault ("overflow", 0);
  args.Add ("ftrainy", &ftrainy,      0, "training Y PDF");
  args.Add ("ftesty",  &ftesty ,      4, "test Y PDF");
  args.Add ("nty",     &nty,         10, "#truth Y bins");
  args.Add ("nmy",     &nmy,         -1, "#measured Y bins", "nty");
  args.Add ("ylo",     &ylo,        0.0, "Y range minimum");
  args.Add ("yhi",     &yhi,       10.0, "Y range maximum");
  args.Add ("mtrainy", &mtrainy,    5.0, "training signal PDF Y mean");
  args.Add ("wtrainy", &wtrainy,    1.2, "training signal PDF Y width");
  args.Add ("btrainy", &btrainy,    0.1, "training Y PDF background fraction");
  args.Add ("mtesty",  &mtesty,     4.5, "test signal PDF Y mean");
  args.Add ("wtesty",  &wtesty,     2.5, "test signal PDF Y width");
  args.Add ("btesty",  &btesty,     0.2, "test Y PDF background fraction");
  args.Add ("effylo",  &effylo,    0.95, "efficiency at ylo");
  args.Add ("effyhi",  &effyhi,     0.8, "efficiency at yhi");
  args.Add ("fakeylo", &fakeylo,    0.4, "fraction of fakes at ylo");
  args.Add ("fakeyhi", &fakeyhi,    0.3, "fraction of fakes at yhi");
  args.Add ("rotxy",   &rotxy,      0.6, "rotation angle in XY plane");
  args.Add ("ybias",   &ybias,      1.0, "shift in Y");
  args.Add ("ysmear",  &ysmear,     0.5, "Y smearing width in bins");
}

//==============================================================================
// Train: create response matrix
//==============================================================================

Int_t RooUnfoldTestHarness2D::Train ()
{
  TVectorD xtrue(ntrain), ytrue(ntrain);

  hPDFx= Generate (xtrue, "trainpdfx", "Training PDF X", ntx, ftrainx, ntrain, xlo, xhi, mtrainx, wtrainx, btrainx, xbias, xsmear);
  if (!hPDFx) return 0;
  hPDFy= Generate (ytrue, "trainpdfy", "Training PDF Y", nty, ftrainy, ntrain, ylo, yhi, mtrainy, wtrainy, btrainy, ybias, ysmear);
  if (!hPDFy) return 0;

  hTrainTrue= new TH2D ("traintrue", "Training Truth", ntx, xlo, xhi, nty, ylo, yhi);
  hTrainTrue->SetLineColor(kBlue);
  hTrain= new TH2D ("train", "Training Measured", nmx, xlo, xhi, nmy, ylo, yhi);
  hTrain->SetLineColor(kRed);

  response->Setup (hTrain, hTrainTrue);

  for (Int_t i= 0; i<ntrain; i++) {
    Double_t xt= (*&xtrue)[i], yt= (*&ytrue)[i];
    hTrainTrue->Fill (xt, yt);
    if (Eff2D (xt, yt)) {
      Double_t x= xt, y= yt;
      Smear2D (x, y);
      hTrain  ->Fill (x, y);
      response->Fill (x, y, xt, yt);
    } else {
      response->Miss (xt, yt);
    }
  }

  TVectorD xfake, yfake;
  Int_t nfake= Fakes2D (xfake, yfake, ntrain);
  if (nfake>0) {
    hTrainFake= new TH2D ("trainfake", "Training Fakes", nmx, xlo, xhi, nmy, ylo, yhi);
    hTrainFake->SetLineColor(93);
    for (Int_t i= 0; i<nfake; i++) {
      Double_t xf= (*&xfake)[i], yf= (*&yfake)[i];
      hTrain    ->Fill (xf, yf);
      hTrainFake->Fill (xf, yf);
      response  ->Fake (xf, yf);
    }
    hTrainFakeX= ProjectionX (hTrainFake, "hTrainFakeX", "Training Fakes X");
    hTrainFakeY= ProjectionY (hTrainFake, "hTrainFakeY", "Training Fakes Y");
  }

  hTrainTrueX= ProjectionX (hTrainTrue, "hTrainTrueX", "Training X");
  hTrainTrueY= ProjectionY (hTrainTrue, "hTrainTrueY", "Training Y");
  hTrainX=     ProjectionX (hTrain,     "hTrainX",     "Training Measured X");
  hTrainY=     ProjectionY (hTrain,     "hTrainY",     "Training Measured Y");

  return 1;
}

void RooUnfoldTestHarness2D::TrainResults()
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
  setmax (hTrainTrueX, hPDFx, hTrainX, hTrainFakeX);
  setmax (hTrainTrueY, hPDFy, hTrainY, hTrainFakeY);

  if (onepage && (onepage<7 || ipad>=onepage)) return;
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
}

//==============================================================================
// Test unfolding algorithm
//==============================================================================

Int_t RooUnfoldTestHarness2D::Test ()
{
  TVectorD xtest(ntest), ytest(ntest);

  hTestPDFx= Generate (xtest, "pdfx", "PDF X", ntx, ftestx, ntest, xlo, xhi, mtestx, wtestx, btestx, xbias, xsmear);
  if (!hTestPDFx) return 0;
  hTestPDFy= Generate (ytest, "pdfy", "PDF Y", nty, ftesty, ntest, ylo, yhi, mtesty, wtesty, btesty, ybias, ysmear);
  if (!hTestPDFy) return 0;

  hTrue= new TH2D ("true", "Test Truth", ntx, xlo, xhi, nty, ylo, yhi);
  hTrue->SetLineColor(kBlue);
  hMeas= new TH2D ("meas", "Test Measured", nmx, xlo, xhi, nmy, ylo, yhi);
  hMeas->SetLineColor(kRed);

  for (Int_t i=0; i<ntest ; i++) {
    Double_t xt= (*&xtest)[i], yt= (*&ytest)[i];
    hTrue->Fill (xt, yt);
    if (Eff2D (xt, yt)) {
      Double_t x= xt, y= yt;
      Smear2D (x, y);
      hMeas->Fill (x, y);
    }
  }

  TVectorD xfake, yfake;
  Int_t nfake= Fakes2D (xfake, yfake, ntest);
  if (nfake>0) {
    hFake= new TH2D ("fake", "Test Fakes", nmx, xlo, xhi, nmy, ylo, yhi);
    hFake->SetLineColor(93);
    for (Int_t i= 0; i<nfake; i++) {
      Double_t xf= (*&xfake)[i], yf= (*&yfake)[i];
      hMeas->Fill (xf, yf);
      hFake->Fill (xf, yf);
    }
    hFakeX= ProjectionX (hFake, "hFakeX", "Test Fakes X");
    hFakeY= ProjectionY (hFake, "hFakeY", "Test Fakes Y");
  }

  hTrueX= ProjectionX (hTrue, "hTrueX", "Test X");
  hTrueY= ProjectionY (hTrue, "hTrueY", "Test Y");
  hMeasX= ProjectionX (hMeas, "hMeasX", "Test Measured X");
  hMeasY= ProjectionY (hMeas, "hMeasY", "Test Measured Y");

  return 1;
}

//==============================================================================
// Show results
//==============================================================================

void RooUnfoldTestHarness2D::Results()
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
  setmax (hTrueX, hTestPDFx, hMeasX, hFakeX);
  setmax (hTrueY, hTestPDFy, hMeasY, hFakeY);

  if (hReco) {
    hRecoX= ProjectionX (hReco, "hRecoX", "Reconstructed X", "E");
    hRecoY= ProjectionY (hReco, "hRecoY", "Reconstructed Y", "E");
    hRecoX->SetMarkerStyle(kFullDotLarge);
    hRecoY->SetMarkerStyle(kFullDotLarge);
    setmax (hTrueX, hRecoX);
    setmax (hTrueY, hRecoY);
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

  if (onepage && (ipad>=onepage || ploterrors)) return;
  if (onepage) canvas->cd(++ipad);
  hTrue->Draw();
  canvas->Update();

  if (onepage && ipad>=onepage) return;
  canvas->cd(++ipad);
  hMeas->Draw();
  canvas->Update();

  if (!hReco) return;

  if (onepage && ipad>=onepage) return;
  if (onepage) canvas->cd(++ipad);
  hReco->Draw();
  canvas->Update();

  if (onepage && ipad>=onepage) return;
  if (onepage) canvas->cd(++ipad);
  gStyle->SetPalette(1,0);
  hCorr->Draw("COLZ");
  canvas->Update();

  if (overflow==2) {
    Int_t kilroy= 0;
    Double_t minpull= 0.0, maxpull= 0.0;
    for (Int_t ix= 2; ix<=ntx-1; ix++) {
      for (Int_t iy= 2; iy<=nty-1; iy++) {
        if ( hReco->GetBinContent(ix,iy)!=0.0             && hReco->GetBinError(ix,iy)>0.0 &&
            (hTrue->GetBinContent(ix,iy)!=0.0 || (doerror && hTrue->GetBinError(ix,iy)>0.0))) {
          Double_t pull= (hReco->GetBinContent(ix,iy) - hTrue->GetBinContent(ix,iy)) / hReco->GetBinError(ix,iy);
          if (!kilroy || pull < minpull) minpull= pull;
          if (!kilroy || pull > maxpull) maxpull= pull;
          kilroy++;
        }
      }
    }
    if (maxpull>minpull) {
      cout << "pulls " << minpull << " to " << maxpull << endl;
      hPulls->SetMinimum (minpull);
      hPulls->SetMinimum (maxpull);
    }
  }

  if (onepage && ipad>=onepage) return;
  if (onepage) canvas->cd(++ipad);
  canvas->cd(++ipad);
  hPulls->Draw("COLZ");
  canvas->Update();

  if (onepage && ipad>=onepage) return;
  if (onepage) canvas->cd(++ipad);
  hPullsX= new TProfile ("hPullsX", "Pulls X", ntx, xlo, xhi);
  hPullsY= new TProfile ("hPullsY", "Pulls Y", nty, ylo, yhi);
  for (Int_t ix= 1; ix<=ntx; ix++) {
    for (Int_t iy= 1; iy<=nty; iy++) {
      if ( hReco->GetBinContent(ix,iy)!=0.0             && hReco->GetBinError(ix,iy)>0.0 &&
          (hTrue->GetBinContent(ix,iy)!=0.0 || (doerror && hTrue->GetBinError(ix,iy)>0.0))) {
        Double_t pull= (hReco->GetBinContent(ix,iy) - hTrue->GetBinContent(ix,iy)) / hReco->GetBinError(ix,iy);
        hPullsX->Fill (hReco->GetXaxis()->GetBinCenter(ix), pull);
        hPullsY->Fill (hReco->GetYaxis()->GetBinCenter(iy), pull);
      }
    }
  }

  hPullsX->SetMarkerStyle(kFullDotLarge);
  hPullsY->SetMarkerStyle(kFullDotLarge);

  hPullsX->Draw("P");
  TLine().DrawLine(hPullsX->GetBinLowEdge(1), 0.0, hPullsX->GetBinLowEdge(ntx+1), 0.0);  // draw a line at pull=0;
  canvas->Update();

  if (onepage && ipad>=onepage) return;
  if (onepage) canvas->cd(++ipad);
  hPullsY->Draw("P");
  TLine().DrawLine(hPullsY->GetBinLowEdge(1), 0.0, hPullsY->GetBinLowEdge(nty+1), 0.0);  // draw a line at pull=0;
  canvas->Update();
}

//==============================================================================
// Gaussian smearing, systematic translation, and variable inefficiency
//==============================================================================

void RooUnfoldTestHarness2D::Rot (Double_t& x, Double_t& y, Double_t angle, Double_t x0, Double_t y0)
{
  if (angle==0.0) return;
  Double_t a= gRandom->Gaus (angle, 0.2*angle);
  Double_t s= sin(a), c= cos(a);
  Double_t xr= c*(x-x0) - s*(y-y0);
  Double_t yr= s*(x-x0) + c*(y-y0);
  x= x0+xr;
  y= y0+yr;
}

bool RooUnfoldTestHarness2D::Eff2D (Double_t x, Double_t y) const
{
  return Eff (x, xlo, xhi, effxlo, effxhi) &&
         Eff (y, ylo, yhi, effylo, effyhi);
}

Int_t RooUnfoldTestHarness2D::Fakes2D (TVectorD& xfake, TVectorD& yfake, Int_t nfake) const
{
  Int_t nfakex= Fakes (xfake, nmx, nfake, xlo, xhi, fakexlo, fakexhi);
  Int_t nfakey= Fakes (yfake, nmy, nfake, ylo, yhi, fakeylo, fakeyhi);
  if        (nfakex<nfakey) {
    yfake.ResizeTo(nfakex);
  } else if (nfakey<nfakex) {
    xfake.ResizeTo(nfakey);
    nfakex= nfakey;
  }
  return nfakex;
}

void RooUnfoldTestHarness2D::Smear2D (Double_t& x, Double_t& y) const
{
  x= Smear (x, ntx, xlo, xhi, xbias, xsmear);
  y= Smear (y, nty, ylo, yhi, ybias, ysmear);
  if (!addbias) return;
  Rot (x, y, rotxy, 0.5*(xlo+xhi), 0.5*(ylo+yhi));
}

//==============================================================================
// Constructors
//==============================================================================

RooUnfoldTestHarness2D::RooUnfoldTestHarness2D (const char* name)
  : RooUnfoldTestHarness(name)
{
  Reset();
  SetDefaults();
}

RooUnfoldTestHarness2D::RooUnfoldTestHarness2D (const char* name, int argc, const char* const* argv)
  : RooUnfoldTestHarness(name)
{
  Reset();
  error= SetArgs (argc, argv);
}

RooUnfoldTestHarness2D::RooUnfoldTestHarness2D (const char* name, const char* args)
  : RooUnfoldTestHarness(name)
{
  Reset();
  const char* const argv[]= { name, args };
  error= SetArgs (2, argv, true);
}

//==============================================================================
// Utility routines
//==============================================================================

void RooUnfoldTestHarness2D::Reset()
{
  RooUnfoldTestHarness::Reset();
  hTrainX= hTrainTrueX= hTrueX= hTrainFakeX= hFakeX= hMeasX= hRecoX= hPullsX=
  hTrainY= hTrainTrueY= hTrueY= hTrainFakeY= hFakeY= hMeasY= hRecoY= hPullsY= hPDFy= hTestPDFy= 0;
  dim= 2;
}

void RooUnfoldTestHarness2D::Init()
{
  RooUnfoldTestHarness::Init();
  if (nty==-1) nty= ntx;
  if (nmy==-1) nmy= nty;
  if (overflow==2) {
    Double_t ywid= (yhi-ylo) / nty;
    ylo -= ywid;
    yhi += ywid;
    nty += 2;
    nmy += Int_t (2.0*(Double_t(nmy)/Double_t(nty)) + 0.5);
  }
  mscaley= (nmy==nty) ? 1.0 : Double_t(nmy)/Double_t(nty);
  ntbins= ntx*nty;
  nmbins= nmx*nmy;
}

Int_t RooUnfoldTestHarness2D::CheckParms()
{
  RooUnfoldTestHarness::CheckParms();
  if (nmy<=0)     {cerr << "Error: nmy ("    << nmy    << ") <= 0"                  << endl; error = 2;}
  if (nty<=0)     {cerr << "Error: nty ("    << nty    << ") <= 0"                  << endl; error = 2;}
  if (ylo >= yhi) {cerr << "Error: ylo ("    << ylo    << ") >= yhi(" << yhi << ")" << endl; error = 2;}
  if (ftrainy<0)  {cerr << "Error: ftrainy ("<< ftrainy<< ") < 0"                   << endl; error = 2;}
  if (ftesty<0)   {cerr << "Error: ftesty (" << ftesty << ") < 0"                   << endl; error = 2;}
  if (dofit)      {cerr << "Warning: parametric fit test only implemented for 1D"   << endl; dofit = 0;}
  return error;
}

TH1D* RooUnfoldTestHarness2D::ProjectionX (const TH1* h, const char* name, const char* title, Option_t* opt)
{
  const TH2* h2= dynamic_cast<const TH2*>(h);
  TH1D* h1= h2->ProjectionX (name, 1, h->GetNbinsY(), opt);
  if (title) h1->SetTitle (title);
  return h1;
}

TH1D* RooUnfoldTestHarness2D::ProjectionY (const TH1* h, const char* name, const char* title, Option_t* opt)
{
  const TH2* h2= dynamic_cast<const TH2*>(h);
  TH1D* h1= h2->ProjectionY (name, 1, h->GetNbinsX(), opt);
  if (title) h1->SetTitle (title);
  return h1;
}

#endif
