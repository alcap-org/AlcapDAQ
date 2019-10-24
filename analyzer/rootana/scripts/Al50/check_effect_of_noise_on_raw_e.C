#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TStyle.h"

void BuilddEvEHists(TH2*& l, TH2*& r) {
  TChain* tr[2] = { new TChain(), new TChain() };
  tr[0]->Add("data/Al50/psel.3layer.root/PID_LP");
  tr[0]->Add("data/Al50/psel.3layer.root/PID_LD");
  tr[0]->Add("data/Al50/psel.3layer.root/PID_LT");
  tr[0]->Add("data/Al50/psel.3layer.root/PID_LA");
  tr[0]->Add("data/Al50/psel.3layer.root/PID_LU");
  tr[1]->Add("data/Al50/psel.3layer.root/PID_RP");
  tr[1]->Add("data/Al50/psel.3layer.root/PID_RD");
  tr[1]->Add("data/Al50/psel.3layer.root/PID_RT");
  tr[1]->Add("data/Al50/psel.3layer.root/PID_RA");
  tr[1]->Add("data/Al50/psel.3layer.root/PID_RU");
  tr[0]->Draw("de:e>>hldeve(850,1.5e3,10e3,300,0,3e3)", "", "goff");
  tr[1]->Draw("de:e>>hrdeve(850,1.5e3,10e3,300,0,3e3)", "", "goff");
  l = (TH2*)gDirectory->Get("hldeve");
  r = (TH2*)gDirectory->Get("hrdeve");
}

void check_effect_of_noise_on_raw_e() {
  TFile* fclean = new TFile("data/Al50/en.2layer.t400.dt200.rebin5.elo2e3keV.ehi20e3keV.root");
  TFile* fflat  = new TFile("data/Al50/en.2layer.t400.dt200.rebin5.elo2e3keV.ehi20e3keV.flatnoise.root");
  TFile* fpois  = new TFile("data/Al50/en.2layer.t400.dt200.rebin5.elo2e3keV.ehi20e3keV.poissonnoise.root");
  TFile* fpsel  = new TFile("data/Al50/psel.3layer.root");
  TH1* hrawclean[2] = { (TH1*)fclean->Get("hl_e_raw"),
                        (TH1*)fclean->Get("hr_e_raw") };
  TH1* hrawflat[2]  = { (TH1*)fflat ->Get("hl_e_raw"),
                        (TH1*)fflat ->Get("hr_e_raw") };
  TH1* hrawpois[2]  = { (TH1*)fpois ->Get("hl_e_raw"),
                        (TH1*)fpois ->Get("hr_e_raw") };
  TH1* heclean[2]   = { (TH1*)fclean->Get("hl_e_reco_bay"),
                        (TH1*)fclean->Get("hr_e_reco_bay") };
  TH1* heflat[2]    = { (TH1*)fflat ->Get("hl_e_reco_bay"),
                        (TH1*)fflat ->Get("hr_e_reco_bay") };
  TH1* hepois[2]    = { (TH1*)fpois ->Get("hl_e_reco_bay"),
                        (TH1*)fpois ->Get("hr_e_reco_bay") };
  TH2* h2deve[2];
  BuilddEvEHists(h2deve[0], h2deve[1]);

  hrawclean[0]->SetTitle("Left Raw;E [keV];p/500 keV");
  hrawclean[1]->SetTitle("Right Raw;E [keV];p/500 keV");
  heclean[0]  ->SetTitle("Left Unfolded;E [keV];p/500 keV");
  heclean[1]  ->SetTitle("Right Unfolded;E [keV];p/500 keV");
  hrawflat[0] ->SetTitle("Left Raw;E [keV];p/500 keV");
  hrawflat[1] ->SetTitle("Right Raw;E [keV];p/500 keV");
  heflat[0]   ->SetTitle("Left Unfolded;E [keV];p/500 keV");
  heflat[1]   ->SetTitle("Right Unfolded;E [keV];p/500 keV");
  hrawpois[0] ->SetTitle("Left Raw;E [keV];p/500 keV");
  hrawpois[1] ->SetTitle("Right Raw;E [keV];p/500 keV");
  hepois[0]   ->SetTitle("Left Unfolded;E [keV];p/500 keV");
  hepois[1]   ->SetTitle("Right Unfolded;E [keV];p/500 keV");
  h2deve[0]   ->SetTitle("Left;E [keV]/10 keV;dE [keV]/10 keV");
  h2deve[1]   ->SetTitle("Right;E [keV]/10 keV;dE [keV]/10 keV");
  hrawclean[0]->SetLineColor(kGreen);
  hrawclean[1]->SetLineColor(kGreen);
  hrawflat[0] ->SetLineColor(kRed);
  hrawflat[1] ->SetLineColor(kRed);
  hrawpois[0] ->SetLineColor(kBlue);
  hrawpois[1] ->SetLineColor(kBlue);
  heclean[0]  ->SetLineColor(kGreen);
  heclean[1]  ->SetLineColor(kGreen);
  heflat[0]   ->SetLineColor(kRed);
  heflat[1]   ->SetLineColor(kRed);
  hepois[0]   ->SetLineColor(kBlue);
  hepois[1]   ->SetLineColor(kBlue);
  hrawflat[0] ->GetXaxis()->SetRangeUser(2e3, 16e3);
  hrawflat[1] ->GetXaxis()->SetRangeUser(2e3, 16e3);
  hrawpois[0] ->GetXaxis()->SetRangeUser(2e3, 16e3);
  hrawpois[1] ->GetXaxis()->SetRangeUser(2e3, 16e3);
  hrawclean[0]->GetXaxis()->SetRangeUser(2e3, 16e3);
  hrawclean[1]->GetXaxis()->SetRangeUser(2e3, 16e3);
  heflat[0]   ->GetXaxis()->SetRangeUser(2e3, 16e3);
  heflat[1]   ->GetXaxis()->SetRangeUser(2e3, 16e3);
  hepois[0]   ->GetXaxis()->SetRangeUser(2e3, 16e3);
  hepois[1]   ->GetXaxis()->SetRangeUser(2e3, 16e3);
  heclean[0]  ->GetXaxis()->SetRangeUser(2e3, 16e3);
  heclean[1]  ->GetXaxis()->SetRangeUser(2e3, 16e3);
  hrawflat[0] ->Sumw2(kFALSE);
  hrawflat[1] ->Sumw2(kFALSE);
  hrawpois[0] ->Sumw2(kFALSE);
  hrawpois[1] ->Sumw2(kFALSE);
  hrawclean[0]->Sumw2(kFALSE);
  hrawclean[1]->Sumw2(kFALSE);
  heflat[0]   ->Sumw2(kFALSE);
  heflat[1]   ->Sumw2(kFALSE);
  hepois[0]   ->Sumw2(kFALSE);
  hepois[1]   ->Sumw2(kFALSE);
  heclean[0]  ->Sumw2(kFALSE);
  heclean[1]  ->Sumw2(kFALSE);
  hrawflat[0] ->SetMinimum(0);
  hrawflat[1] ->SetMinimum(0);
  heflat[0]   ->SetMinimum(0);
  heflat[1]   ->SetMinimum(0);

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(hrawclean[0], "Original");
  leg->AddEntry(hrawflat[0],  "Added flat noise");
  leg->AddEntry(hrawpois[0],  "Poisson Modified");

  gStyle->SetOptStat(0);
  TCanvas* c1 = new TCanvas("c1", "c1", 1400, 1000);
  c1->Divide(2, 2);
  c1->cd(1);
  hrawflat[0] ->Draw("E");
  hrawpois[0] ->Draw("E SAME");
  hrawclean[0]->Draw("E SAME");
  leg->Draw();
  c1->cd(2);
  hrawflat[1] ->Draw("E");
  hrawpois[1] ->Draw("E SAME");
  hrawclean[1]->Draw("E SAME");
  leg->Draw();
  c1->cd(3);
  heflat[0] ->Draw("E");
  hepois[0] ->Draw("E SAME");
  heclean[0]->Draw("E SAME");
  leg->Draw();
  c1->cd(4);
  heflat[1] ->Draw("E");
  hepois[1] ->Draw("E SAME");
  heclean[1]->Draw("E SAME");
  leg->Draw();
  c1->SaveAs("img/check_effect_of_noise_on_raw_e_spectra.png");

  TBox* b = new TBox(3e3, 200, 4e3, 600);
  b->SetFillStyle(0);
  b->SetLineStyle(1);
  b->SetLineColor(kRed);

  TCanvas* c2 = new TCanvas("c2", "c2", 1400, 500);
  c2->Divide(2);
  c2->cd(1);
  h2deve[0]->Draw();
  b->Draw();
  c2->cd(2);
  h2deve[1]->Draw();
  b->Draw();
  c2->SaveAs("img/check_effect_of_noise_on_raw_e_deve.png");
}
