#include "./scripts/Al50/util.h"

#include "RooUnfoldBayes.h"
#include "RooUnfoldResponse.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

#include <cstdio>
#include <iostream>

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
static const int   NITER       = 1;
////////////////////////////////////////////////////////////////////////////////

void FillMeasuredHists(RooUnfoldResponse* r[2], TTree* tr[2],
                       TH1*& hle, TH1*& hre, double t, double dt) {
  const TAxis* ax[2] = { r[0]->Htruth()->GetXaxis(),
                         r[1]->Htruth()->GetXaxis() };
  char cmd[2][64];
  char cut[64];
  std::sprintf(cmd[0], "e>>hle(%d,%f,%f)", ax[0]->GetNbins(),
               ax[0]->GetBinLowEdge(1), ax[0]->GetBinUpEdge(ax[0]->GetNbins()));
  std::sprintf(cmd[1], "e>>hre(%d,%f,%f)", ax[1]->GetNbins(),
               ax[1]->GetBinLowEdge(1), ax[1]->GetBinUpEdge(ax[1]->GetNbins()));
  std::sprintf(cut, "t > %g && abs(dt) < %g", t, dt);
  std::printf("%s\n", cut);
  tr[0]->Draw(cmd[0], cut);
  tr[1]->Draw(cmd[1], cut);
  hle = (TH1*)gDirectory->Get("hle");
  hre = (TH1*)gDirectory->Get("hre");
}

void unfold(const char* ifname_tm, const char* ifname_data,
            const char* ofname, double t=400, double dt=200,
            int rebin=5, double elo=0., double ehi=0.) {
  TH1::SetDefaultSumw2(kTRUE);
  TFile* fdata = new TFile(ifname_data);
  TFile* fmctm = new TFile(ifname_tm);
  RooUnfoldResponse* resp[2] = { (RooUnfoldResponse*)fmctm->Get("SiL_TM"),
                                 (RooUnfoldResponse*)fmctm->Get("SiR_TM") };
  TMUtils::Rebin(resp[0], rebin);
  TMUtils::Rebin(resp[1], rebin);
  if (0 < elo && 0 < ehi) {
    TMUtils::SetRange(resp[0], elo, ehi);
    TMUtils::SetRange(resp[1], elo, ehi);
  }
  TTree* tr[2] = { (TTree*)fdata->Get("PID_LP"), (TTree*)fdata->Get("PID_RP") };
  TH1* heraw[2];
  FillMeasuredHists(resp, tr, heraw[0], heraw[1], t, dt);
  RooUnfoldBayes* unfold[2] = { new RooUnfoldBayes(resp[0], heraw[0], NITER),
                                new RooUnfoldBayes(resp[1], heraw[1], NITER) };
  TH1* h_e_reco[2] = { unfold[0]->Hreco(),
                       unfold[1]->Hreco()};
  TFile* ofile = new TFile(ofname, "RECREATE");
  h_e_reco[0]->SetDirectory(ofile);
  h_e_reco[1]->SetDirectory(ofile);
  h_e_reco[0]->SetName("hl_e_reco");
  h_e_reco[1]->SetName("hr_e_reco");
  ofile->Write();
  ofile->Close();
}

void R15b_Al50_Unfold(char mode='\0', const char* ifname_tm=nullptr,
                      const char* ifname_data=nullptr,
                      const char* ofname=nullptr, double t=400, double dt=200,
                      int rebin=5, double elo=0., double ehi=0.) {
  switch(mode) {
    case 'p': unfold(ifname_tm, ifname_data, ofname, t, dt, rebin, elo, ehi); return;
    case 'd': unfold(ifname_tm, ifname_data, ofname, t, dt, rebin, elo, ehi); return;
    case 't': unfold(ifname_tm, ifname_data, ofname, t, dt, rebin, elo, ehi); return;
    case 'a': unfold(ifname_tm, ifname_data, ofname, t, dt, rebin, elo, ehi); return;
  }
}
