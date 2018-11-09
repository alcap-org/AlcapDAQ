
#include "scripts/Al50/util.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TLegend.h"
#include "TStyle.h"

void compare_mark_john_reco() {
  TFile* fmw  = new TFile("data/m-wong/bayesian-output-unfolding.proton.10.20.al50.root");
  TFile* fjq  = new TFile("data/Al50/en.2layer.t400.dt200.rebin5.elo2e3keV.ehi20e3keV.root");
  TH1* hmw[2] = { (TH1*)fmw->Get("hRecoLeft"),
                  (TH1*)fmw->Get("hRecoRight") };
  TH1* hjq[2] = { (TH1*)fjq->Get("hl_e_reco_bay"),
                  (TH1*)fjq->Get("hr_e_reco_bay") };
  HistUtils::ScaleXAxis(hjq[0], 1e-3);
  HistUtils::ScaleXAxis(hjq[1], 1e-3);

  // Capture rate                          = 61%
  // Time cut efficiency                   = 65%
  // Distance between Si and target {L, R} = {133.2mm, 120.8,,}
  // Target side length                    = 50mm
  double norm_mw[2] = { 50*50/(4*TMath::Pi()*133.2*133.2)*130e6*0.65*0.61,
                        50*50/(4*TMath::Pi()*120.8*120.8)*130e6*0.65*0.61 };
  double norm_jq[2] = { Normalization::Al50(),
                        Normalization::Al50() };

  hmw[0]->Scale(1./norm_mw[0]);
  hmw[1]->Scale(1./norm_mw[1]);
  hjq[0]->Scale(1./norm_jq[0]);
  hjq[1]->Scale(1./norm_jq[1]);

  hmw[0]->SetLineColor(kBlue); hmw[1]->SetLineColor(kBlue);
  hjq[0]->SetLineColor(kRed);  hjq[1]->SetLineColor(kRed);
  hmw[0]->SetTitle("Al50 Left Reconstructed Spectrum;E[MeV];/p/500keV");
  hmw[1]->SetTitle("Al50 Right Reconstructed Spectrum;E[MeV];/p/500keV");

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(hmw[0], "Mark");
  leg->AddEntry(hjq[0], "John");

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1);
  hmw[0]->Draw();
  hjq[0]->Draw("SAME");
  leg->Draw();
  c->cd(2);
  hmw[1]->Draw();
  hjq[1]->Draw("SAME");
  leg->Draw();
  c->SaveAs("img/compare_mark_john_reco.png");


}
