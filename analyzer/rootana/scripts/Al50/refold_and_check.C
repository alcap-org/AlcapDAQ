#include "./scripts/Al50/util.h"

#include "RooUnfoldResponse.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TLegend.h"
#include "TRatioPlot.h"
#include "TStyle.h"

void refold_and_check() {
  TFile* ftm = new TFile("data/Al50/tm.2layer.root");
  TFile* fen = new TFile("data/Al50/en.2layer.t400.dt200.rebin5.elo2e3keV.ehi15e3keV.root");

  TH1* hraw[2]               = { (TH1*)fen->Get("hl_e_raw"),
                                 (TH1*)fen->Get("hr_e_raw") };
  TH1* hunf[2]               = { (TH1*)fen->Get("hl_e_reco_bay"),
                                 (TH1*)fen->Get("hr_e_reco_bay") };
  RooUnfoldResponse* resp[2] = { (RooUnfoldResponse*)ftm->Get("SiL_TM"),
                                 (RooUnfoldResponse*)ftm->Get("SiR_TM") };
  TMUtils::Rebin(resp[0], 5);
  TMUtils::Rebin(resp[1], 5);
  TMUtils::SetRange(resp[0], 2e3, 15e3);
  TMUtils::SetRange(resp[1], 2e3, 15e3);
  TH1* hfolded[2] = { resp[0]->ApplyToTruth(hunf[0], "hl_e_folded"),
                      resp[1]->ApplyToTruth(hunf[1], "hr_e_folded") };

  hraw[0]   ->SetTitle("Al50 Validating Proton Spectrum (Left);E [keV];cts/500 keV");
  hraw[1]   ->SetTitle("Al50 Validating Proton Spectrum (Right);E [keV];cts/500 keV");
  hfolded[0]->SetTitle("Al50 Validating Proton Spectrum (Left);E [keV];cts/500 keV");
  hfolded[1]->SetTitle("Al50 Validating Proton Spectrum (Right);E [keV];cts/500 keV");
  hraw[0]->GetYaxis()->SetTitleOffset(1.55);
  hraw[1]->GetYaxis()->SetTitleOffset(1.55);
  hraw[0]->SetLineColor(1);
  hraw[1]->SetLineColor(1);
  hfolded[0]->SetLineColor(2);
  hfolded[1]->SetLineColor(2);

  TCanvas* c = new TCanvas("c", "c", 1400, 800);
  TRatioPlot* hrat[2] = { new TRatioPlot(hraw[0], hfolded[0], "diffsig pois"),
                          new TRatioPlot(hraw[1], hfolded[1], "diffsig pois")};

  TLegend* l = new TLegend(0.7, 0.7, 0.9, 0.9);
  l->AddEntry(hraw[0],    "Original raw");
  l->AddEntry(hfolded[0], "Unfolded+Refolded");

  gStyle->SetOptStat(0);
  c->Divide(2);
  c->cd(1);
  hrat[0]->Draw();
  hrat[0]->GetUpperPad()->cd();
  l->Draw();
  c->cd(2);
  hrat[1]->Draw();
  hrat[1]->GetUpperPad()->cd();
  l->Draw();
  c->SaveAs("img/refold_and_check.png");

}
