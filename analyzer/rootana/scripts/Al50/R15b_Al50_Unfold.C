#include "psel.h"
#include "util.h"

#include "RooUnfoldBayes.h"
#include "RooUnfoldBinByBin.h"
#include "RooUnfoldInvert.h"
#include "RooUnfoldSvd.h"
#include "RooUnfoldTUnfold.h"
#include "RooUnfoldResponse.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom3.h"
#include "TTree.h"

#include <cstdio>
#include <iostream>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// USER DEFINED VARIABLES
// Takes the post-cut data E vs dE plots and transfer matrices from
// the simulation and unfolds the energy spectrum for the desired particle
// Compile only: root -l -b -q scripts/Al50/R15b_Al50_Unfold.C+g
// Run:          root -l -b -q scripts/Al50/R15b_Al50_Unfold.C+g\(mode,
//                 ifname_tm, ifname_data, ofname\)
// where mode is a char and one of (all others compile only)
//   p (protons), d (deuterons), t (tritons), a (alphas)
// ifname_tm, ifname_data, and ofname are all c-strings.
// ifname_tm:   input file name with the transfer matrices whose names match
//              the global constants below
// ifname_data: The output of the particle selection script; the data.
// ofname:      Output file name.
// Iterations for Bayesian unfolding
static const int   BAYESPAR = 4;
static const int   SVDPAR   = 15;
////////////////////////////////////////////////////////////////////////////////

void ApplyPIDCorrection(TH1* hraw[2]) {
  TFile* ifile = new TFile("data/Al50/corranderr/pid.root");
  TH1* hcorr[2] = { (TH1*)ifile->Get("hlpidcorrerr"),
                    (TH1*)ifile->Get("hrpidcorrerr") };
  for (int i = 1; i <= hraw[0]->GetNbinsX(); ++i) {
    double err[2] = { hraw[0]->GetBinError(i), hraw[1]->GetBinError(i) };
    hraw[0]->SetBinContent(i, hraw[0]->GetBinContent(i)/hcorr[0]->GetBinContent(i));
    hraw[1]->SetBinContent(i, hraw[1]->GetBinContent(i)/hcorr[1]->GetBinContent(i));
    hraw[0]->SetBinError(i, err[0]);
    hraw[1]->SetBinError(i, err[1]);
  }
}

void FillMeasuredHists(RooUnfoldResponse* r[2], TTree* tr[2],
                       TH1*& hle, TH1*& hre, double pp, double t, double dt) {
  const TAxis* ax[2] = { r[0]->Htruth()->GetXaxis(),
                         r[1]->Htruth()->GetXaxis() };
  char cmd[2][64];
  char cut[64];
  std::sprintf(cmd[0], "e1+e2>>hle(%d,%f,%f)", ax[0]->GetNbins(),
               ax[0]->GetBinLowEdge(1), ax[0]->GetBinUpEdge(ax[0]->GetNbins()));
  std::sprintf(cmd[1], "e1+e2>>hre(%d,%f,%f)", ax[1]->GetNbins(),
               ax[1]->GetBinLowEdge(1), ax[1]->GetBinUpEdge(ax[1]->GetNbins()));
  std::sprintf(cut, "%g<t && t<%g && abs(dt)<%g && e3==0", t, pp, dt);
  std::printf("%s\n", cut);
  tr[0]->Draw(cmd[0], cut, "goff");
  tr[1]->Draw(cmd[1], cut, "goff");
  hle = (TH1*)gDirectory->Get("hle");
  hre = (TH1*)gDirectory->Get("hre");
}

// n is average number of background events per bin
void AddFlatBackground(TH1* h, double n) {
  static TRandom3 r;
  for (int i = 1; i <= h->GetNbinsX(); ++i)
    h->Fill(h->GetXaxis()->GetBinCenter(i), r.Poisson(n));
}

void SubtractFlatBackground(TH1* h, double n) {
  for (int i = 1; i < h->GetNbinsX(); ++i)
    h->SetBinContent(i, h->GetBinContent(i) + n);
}

void PoissonShake(TH1* h) {
  static TRandom3 r;
  for (int i = 1; i <= h->GetNbinsX(); ++i)
    // h->SetBinContent(i, r.Gaus(h->GetBinContent(i), h->GetBinError(i)));
    h->SetBinContent(i, r.Poisson(h->GetBinContent(i)));
}

void unfold(const char* ifname_tm, const char* ifname_data,
            const char* ofname, double pp, double t, double dt,
            int rebin, double elo, double ehi, int bayiter,
            bool pidcorrect, const std::string& noise) {
  TH1::SetDefaultSumw2(kTRUE);
  TFile* fdata = new TFile(ifname_data);
  TFile* fmctm = new TFile(ifname_tm);
  RooUnfoldResponse* resp[2] = { (RooUnfoldResponse*)fmctm->Get("SiL_TM"),
                                 (RooUnfoldResponse*)fmctm->Get("SiR_TM") };
  if (rebin > 1) {
    TMUtils::Rebin(resp[0], rebin);
    TMUtils::Rebin(resp[1], rebin);
  }
  if (0 <= elo && 0 < ehi && elo < ehi) {
    TMUtils::SetRange(resp[0], elo, ehi);
    TMUtils::SetRange(resp[1], elo, ehi);
  } else {
    std::printf("Not setting range because of inputs elo=%g and ehi=%g\n",
                elo, ehi);
  }
  TTree* tr[2] = { (TTree*)fdata->Get("PID_LP"), (TTree*)fdata->Get("PID_RP") };
  TH1* he_raw[2];
  FillMeasuredHists(resp, tr, he_raw[0], he_raw[1], pp, t, dt);
  if (noise == "poisson") {
    PoissonShake(he_raw[0]);
    PoissonShake(he_raw[1]);
  } else if (noise == "flat") {
    // Below the numbers mean [events/keV^2]*[keV^2]/[bins] = events/bin
    // where events/keV^2 is from a (E, dE) (3e3-4e3, 200-600) in the
    // dEvE plots, and [keV^2] is the area of the proton cut, so that
    // we have an estimate of the noise in the he_raw proton plots.
    // This was for the PSelPow method for protons, so not for other
    // methods and other particles, but honestly the number is otherwise
    // arbitrary so it how much it matters is debateable.
    AddFlatBackground(he_raw[0], 0.000825 *5233239. /he_raw[0]->GetNbinsX());
    AddFlatBackground(he_raw[1], 0.0021675*5212455.5/he_raw[1]->GetNbinsX());
  } else if (!noise.empty()) {
    throw "Unknown noise method!";
  }

  // Check if PID correction can be applied
  if (elo != 2e3 || ehi != 12e3 || rebin != 5)
    pidcorrect = false;
  if (pidcorrect)
    ApplyPIDCorrection(he_raw);

  RooUnfoldBayes*    bay[2] = { new RooUnfoldBayes(   resp[0], he_raw[0], bayiter),
                                new RooUnfoldBayes(   resp[1], he_raw[1], bayiter) };
  RooUnfoldSvd*      svd[2] = { new RooUnfoldSvd(     resp[0], he_raw[0], SVDPAR),
                                new RooUnfoldSvd(     resp[1], he_raw[1], SVDPAR) };
  RooUnfoldBinByBin* bbb[2] = { new RooUnfoldBinByBin(resp[0], he_raw[0]),
                                new RooUnfoldBinByBin(resp[1], he_raw[1]) };
  RooUnfoldTUnfold*  tun[2] = { new RooUnfoldTUnfold( resp[0], he_raw[0]),
                                new RooUnfoldTUnfold( resp[1], he_raw[1]) };
  RooUnfoldInvert*   inv[2] = { new RooUnfoldInvert(  resp[0], he_raw[0]),
                                new RooUnfoldInvert(  resp[1], he_raw[1]) };
  TH1* he_bay[2] = { bay[0]->Hreco(), bay[1]->Hreco() };
  TH1* he_svd[2] = { svd[0]->Hreco(), svd[1]->Hreco() };
  TH1* he_bbb[2] = { bbb[0]->Hreco(), bbb[1]->Hreco() };
  // TH1* he_tun[2]  = { tun[0]->Hreco(), tun[1]->Hreco() };
  TH1* he_inv[2] = { inv[0]->Hreco(), inv[1]->Hreco() };
  TFile* ofile = new TFile(ofname, "RECREATE");
  he_raw[0]->SetDirectory(ofile);
  he_raw[1]->SetDirectory(ofile);
  he_bay[0]->SetDirectory(ofile);
  he_bay[1]->SetDirectory(ofile);
  he_svd[0]->SetDirectory(ofile);
  he_svd[1]->SetDirectory(ofile);
  he_bbb[0]->SetDirectory(ofile);
  he_bbb[1]->SetDirectory(ofile);
  // he_tun[0]->SetDirectory(ofile);
  // he_tun[1]->SetDirectory(ofile);
  he_inv[0]->SetDirectory(ofile);
  he_inv[1]->SetDirectory(ofile);
  he_raw[0]->SetName("hl_e_raw");
  he_raw[1]->SetName("hr_e_raw");
  he_bay[0]->SetName("hl_e_reco_bay");
  he_bay[1]->SetName("hr_e_reco_bay");
  he_svd[0]->SetName("hl_e_reco_svd");
  he_svd[1]->SetName("hr_e_reco_svd");
  he_bbb[0]->SetName("hl_e_reco_bbb");
  he_bbb[1]->SetName("hr_e_reco_bbb");
  // he_tun[0]->SetName("hl_e_reco_tun");
  // he_tun[1]->SetName("hr_e_reco_tun");
  he_inv[0]->SetName("hl_e_reco_inv");
  he_inv[1]->SetName("hr_e_reco_inv");

  bay[0]->Write("rul_bay");
  bay[1]->Write("rur_bay");
  svd[0]->Write("rul_svd");
  svd[1]->Write("rur_svd");
  bbb[0]->Write("rul_bbb");
  bbb[1]->Write("rur_bbb");

  TTree* inputs = new TTree("inputs", "inputs");
  std::string nse = noise;
  inputs->Branch("pp",         &pp);
  inputs->Branch("t",          &t);
  inputs->Branch("dt",         &dt);
  inputs->Branch("rebin",      &rebin);
  inputs->Branch("elo",        &elo);
  inputs->Branch("ehi",        &ehi);
  inputs->Branch("pidcorrect", &pidcorrect);
  inputs->Branch("noisetype",  &nse);
  inputs->Fill();

  ofile->Write();
  ofile->Close();
}

void R15b_Al50_Unfold(char mode='\0', const char* ifname_tm=nullptr,
                      const char* ifname_data=nullptr,
                      const char* ofname=nullptr,
                      double pp=10e3, double t=400, double dt=200,
                      int rebin=5, double elo=2e3, double ehi=12e3,
                      int bayiter=5, bool pidcorrect=true, const std::string& noise="") {
  switch(mode) {
    case 'p': unfold(ifname_tm, ifname_data, ofname, pp, t, dt, rebin, elo, ehi, bayiter, pidcorrect, noise); return;
    case 'd': unfold(ifname_tm, ifname_data, ofname, pp, t, dt, rebin, elo, ehi, bayiter, pidcorrect, noise); return;
    case 't': unfold(ifname_tm, ifname_data, ofname, pp, t, dt, rebin, elo, ehi, bayiter, pidcorrect, noise); return;
    case 'a': unfold(ifname_tm, ifname_data, ofname, pp, t, dt, rebin, elo, ehi, bayiter, pidcorrect, noise); return;
  }
}
