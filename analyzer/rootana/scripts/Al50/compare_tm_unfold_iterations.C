#include "./scripts/Al50/util.h"

#include "RooUnfoldBayes.h"
#include "RooUnfoldResponse.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TTree.h"

#include <cstdio>
#include <iostream>

void FillMeasuredHists(RooUnfoldResponse* r[2], TTree* tr[2],
                       TH1*& hle, TH1*& hre) {
  if (hle) { delete hle; hle = nullptr; }
  if (hre) { delete hre; hre = nullptr; }
  const TAxis* ax[2] = { r[0]->Htruth()->GetXaxis(),
                         r[1]->Htruth()->GetXaxis() };
  char cmd[2][64];
  std::sprintf(cmd[0], "e>>hle(%d,%f,%f)", ax[0]->GetNbins(),
               ax[0]->GetBinLowEdge(1), ax[0]->GetBinUpEdge(ax[0]->GetNbins()));
  std::sprintf(cmd[1], "e>>hre(%d,%f,%f)", ax[1]->GetNbins(),
               ax[1]->GetBinLowEdge(1), ax[1]->GetBinUpEdge(ax[1]->GetNbins()));
  tr[0]->Draw(cmd[0], "t > 400 && abs(dt) < 200 && e3 == 0");
  tr[1]->Draw(cmd[1], "t > 400 && abs(dt) < 200 && e3 == 0");
  hle = (TH1*)gDirectory->Get("hle");
  hre = (TH1*)gDirectory->Get("hre");
}

void compare_tm_unfold_iterations() {
  TFile* fdata = new TFile("data/Al100/pselal100.root");
  TFile* fmctm = new TFile("data/Al100/tm.root");
  RooUnfoldResponse* resp[2] = { (RooUnfoldResponse*)fmctm->Get("SiL_TM"),
                                 (RooUnfoldResponse*)fmctm->Get("SiR_TM") };
  TTree* tr[2] = { (TTree*)fdata->Get("PID_LP"), (TTree*)fdata->Get("PID_RP") };
  TMUtils::Rebin(resp[0], 5);
  TMUtils::Rebin(resp[1], 5);
  TH1* heraw[2] = {};
  TH1* h_e_reco[6][2];
  TMUtils::SetRange(resp[0], 0, 14e3);
  TMUtils::SetRange(resp[1], 0, 14e3);
  FillMeasuredHists(resp, tr, heraw[0], heraw[1]);
  RooUnfoldBayes* unf_0[2] = { new RooUnfoldBayes(resp[0], heraw[0], 1),
                               new RooUnfoldBayes(resp[1], heraw[1], 1) };
  h_e_reco[0][0] = unf_0[0]->Hreco();
  h_e_reco[0][1] = unf_0[1]->Hreco();
  FillMeasuredHists(resp, tr, heraw[0], heraw[1]);
  RooUnfoldBayes* unf_1[2] = { new RooUnfoldBayes(resp[0], heraw[0], 2),
                               new RooUnfoldBayes(resp[1], heraw[1], 2) };
  h_e_reco[1][0] = unf_1[0]->Hreco();
  h_e_reco[1][1] = unf_1[1]->Hreco();
  FillMeasuredHists(resp, tr, heraw[0], heraw[1]);
  RooUnfoldBayes* unf_2[2] = { new RooUnfoldBayes(resp[0], heraw[0], 3),
                               new RooUnfoldBayes(resp[1], heraw[1], 3) };
  h_e_reco[2][0] = unf_2[0]->Hreco();
  h_e_reco[2][1] = unf_2[1]->Hreco();
  FillMeasuredHists(resp, tr, heraw[0], heraw[1]);
  RooUnfoldBayes* unf_3[2] = { new RooUnfoldBayes(resp[0], heraw[0], 4),
                               new RooUnfoldBayes(resp[1], heraw[1], 4) };
  h_e_reco[3][0] = unf_3[0]->Hreco();
  h_e_reco[3][1] = unf_3[1]->Hreco();
  FillMeasuredHists(resp, tr, heraw[0], heraw[1]);
  RooUnfoldBayes* unf_4[2] = { new RooUnfoldBayes(resp[0], heraw[0], 5),
                               new RooUnfoldBayes(resp[1], heraw[1], 5) };
  h_e_reco[4][0] = unf_4[0]->Hreco();
  h_e_reco[4][1] = unf_4[1]->Hreco();
  FillMeasuredHists(resp, tr, heraw[0], heraw[1]);
  RooUnfoldBayes* unf_5[2] = { new RooUnfoldBayes(resp[0], heraw[0], 6),
                               new RooUnfoldBayes(resp[1], heraw[1], 6) };
  h_e_reco[5][0] = unf_5[0]->Hreco();
  h_e_reco[5][1] = unf_5[1]->Hreco();

  for (int i = 0; i < 6; ++i) {
    h_e_reco[i][0]->SetLineColor(i+2);
    h_e_reco[i][1]->SetLineColor(i+2);
    h_e_reco[i][0]->SetTitle("Left;Energy (keV);Total proton count");
    h_e_reco[i][1]->SetTitle("Right;Energy (keV);Total proton count");
  }
  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->SetHeader("Number of iterations");
  leg->AddEntry(h_e_reco[0][0], "1");
  leg->AddEntry(h_e_reco[1][0], "2");
  leg->AddEntry(h_e_reco[2][0], "3");
  leg->AddEntry(h_e_reco[3][0], "4");
  leg->AddEntry(h_e_reco[4][0], "5");
  leg->AddEntry(h_e_reco[5][0], "6");

  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1);
  h_e_reco[5][0]->Draw("SAME");
  h_e_reco[0][0]->Draw("SAME");
  h_e_reco[1][0]->Draw("SAME");
  h_e_reco[2][0]->Draw("SAME");
  h_e_reco[3][0]->Draw("SAME");
  h_e_reco[4][0]->Draw("SAME");
  leg->Draw();
  c->cd(2);
  h_e_reco[5][1]->Draw("SAME");
  h_e_reco[0][1]->Draw("SAME");
  h_e_reco[1][1]->Draw("SAME");
  h_e_reco[2][1]->Draw("SAME");
  h_e_reco[3][1]->Draw("SAME");
  h_e_reco[4][1]->Draw("SAME");
  leg->Draw();
  c->SaveAs("img/compare_tm_unfold_iterations.png");
}
