#include "./scripts/Al50/util.h"

#include "RooUnfoldBayes.h"
#include "RooUnfoldBinByBin.h"
#include "RooUnfoldInvert.h"
#include "RooUnfoldSvd.h"
#include "RooUnfoldTUnfold.h"
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
static const int   BAYESPAR = 4;
static const int   SVDPAR   = 15;
////////////////////////////////////////////////////////////////////////////////

void FillMeasuredHists(RooUnfoldResponse* r[2], TTree* tr[2],
                       TH1*& hle, TH1*& hre, double t, double dt) {
  const TAxis* ax[2] = { r[0]->Htruth()->GetXaxis(),
                         r[1]->Htruth()->GetXaxis() };
  char cmd[2][64];
  char cut[64];
  std::sprintf(cmd[0], "e1+e2>>hle(%d,%f,%f)", ax[0]->GetNbins(),
               ax[0]->GetBinLowEdge(1), ax[0]->GetBinUpEdge(ax[0]->GetNbins()));
  std::sprintf(cmd[1], "e1+e2>>hre(%d,%f,%f)", ax[1]->GetNbins(),
               ax[1]->GetBinLowEdge(1), ax[1]->GetBinUpEdge(ax[1]->GetNbins()));
  std::sprintf(cut, "t > %g && abs(dt) < %g && e3==0", t, dt);
  std::printf("%s\n", cut);
  tr[0]->Draw(cmd[0], cut);
  tr[1]->Draw(cmd[1], cut);
  hle = (TH1*)gDirectory->Get("hle");
  hre = (TH1*)gDirectory->Get("hre");
}

void unfold(const char* ifname_tm, const char* ifname_data,
            const char* ofname, double t, double dt,
            int rebin, double elo, double ehi) {
  TH1::SetDefaultSumw2(kTRUE);
  TFile* fdata = new TFile(ifname_data);
  TFile* fmctm = new TFile(ifname_tm);
  RooUnfoldResponse* resp[2] = { (RooUnfoldResponse*)fmctm->Get("SiL_TM"),
                                 (RooUnfoldResponse*)fmctm->Get("SiR_TM") };
  TMUtils::Rebin(resp[0], rebin);
  TMUtils::Rebin(resp[1], rebin);
  if (0 <= elo && 0 < ehi && elo < ehi) {
    TMUtils::SetRange(resp[0], elo, ehi);
    TMUtils::SetRange(resp[1], elo, ehi);
  } else {
    std::printf("Not setting range because of inputs elo=%g and ehi=%g\n",
                elo, ehi);
  }
  TTree* tr[2] = { (TTree*)fdata->Get("PID_LP"), (TTree*)fdata->Get("PID_RP") };
  TH1* heraw[2];
  FillMeasuredHists(resp, tr, heraw[0], heraw[1], t, dt);
  RooUnfoldBayes*    bay[2] = { new RooUnfoldBayes(   resp[0], heraw[0], BAYESPAR),
                                new RooUnfoldBayes(   resp[1], heraw[1], BAYESPAR) };
  RooUnfoldSvd*      svd[2] = { new RooUnfoldSvd(     resp[0], heraw[0], SVDPAR),
                                new RooUnfoldSvd(     resp[1], heraw[1], SVDPAR) };
  RooUnfoldBinByBin* bbb[2] = { new RooUnfoldBinByBin(resp[0], heraw[0]),
                                new RooUnfoldBinByBin(resp[1], heraw[1]) };
  RooUnfoldTUnfold*  tun[2] = { new RooUnfoldTUnfold( resp[0], heraw[0]),
                                new RooUnfoldTUnfold( resp[1], heraw[1]) };
  RooUnfoldInvert*   inv[2] = { new RooUnfoldInvert(  resp[0], heraw[0]),
                                new RooUnfoldInvert(  resp[1], heraw[1]) };
  TH1* he_bay[2] = { bay[0]->Hreco(), bay[1]->Hreco() };
  TH1* he_svd[2] = { svd[0]->Hreco(), svd[1]->Hreco() };
  TH1* he_bbb[2] = { bbb[0]->Hreco(), bbb[1]->Hreco() };
  // TH1* he_tun[2]  = { tun[0]->Hreco(), tun[1]->Hreco() };
  // printf("TUnfold done.\n");
  TH1* he_inv[2] = { inv[0]->Hreco(), inv[1]->Hreco() };
  TFile* ofile = new TFile(ofname, "RECREATE");
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
