#include "util.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TStyle.h"

#include <cmath>
#include <cstdio>

void compare_syst_and_stat_err() {
  TFile* fspec  = new TFile("data/Al50/en.p.t400ns.dt200ns.rebin5.pp10us.elo2e3keV.ehi12e3keV.bay5.pidcorr.root");
  TFile* ferr   = new TFile("data/Al50/corranderr/pid.root");
  TH1* hspec[2] = { (TH1*)fspec->Get("hl_e_reco_bay"),
                    (TH1*)fspec->Get("hr_e_reco_bay") };
  TH1* herr[2]  = { (TH1*)ferr->Get("hlpidcorrerr"),
                    (TH1*)ferr->Get("hrpidcorrerr") };
  TH1* hspec_stat[2] = { (TH1*)hspec[0]->Clone("hl_e_stat"),
                         (TH1*)hspec[1]->Clone("hr_e_stat") };
  TH1* hspec_syst[2] = { (TH1*)hspec[0]->Clone("hl_e_syst"),
                         (TH1*)hspec[1]->Clone("hr_e_syst") };

  for (int i = 1; i <= hspec[0]->GetNbinsX(); ++i) {
    // hspec_stat[0]->SetBinError(i, std::sqrt(hspec_stat[0]->GetBinContent(i)));
    // hspec_stat[1]->SetBinError(i, std::sqrt(hspec_stat[1]->GetBinContent(i)));
    hspec_syst[0]->SetBinError(i, herr[0]->GetBinError(i)*hspec_syst[0]->GetBinContent(i));
    hspec_syst[1]->SetBinError(i, herr[1]->GetBinError(i)*hspec_syst[1]->GetBinContent(i));
  }
  hspec_stat[0]->Scale(1/Normalization::Al50());
  hspec_stat[1]->Scale(1/Normalization::Al50());
  hspec_syst[0]->Scale(1/Normalization::Al50());
  hspec_syst[1]->Scale(1/Normalization::Al50());

  hspec_stat[0]->SetTitle("SiL Unfolded Rate;Energy [keV];Counts/500 keV/#mucap");
  hspec_stat[1]->SetTitle("SiR Unfolded Rate;Energy [keV];Counts/500 keV/#mucap");
  hspec_syst[0]->SetTitle("SiL Unfolded Rate;Energy [keV];Counts/500 keV/#mucap");
  hspec_syst[1]->SetTitle("SiR Unfolded Rate;Energy [keV];Counts/500 keV/#mucap");

  hspec_stat[0]->SetFillColor(kGreen);
  hspec_stat[1]->SetFillColor(kGreen);
  hspec_syst[0]->SetFillColor(kYellow);
  hspec_syst[1]->SetFillColor(kYellow);

  hspec_stat[0]->GetYaxis()->SetTitleOffset(1.4);
  hspec_stat[1]->GetYaxis()->SetTitleOffset(1.4);

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(hspec_stat[0], "Unf. Err.", "f");
  leg->AddEntry(hspec_syst[0], "PID Err.",  "f");

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1);
  hspec_stat[0]->Draw("SAME E3");
  hspec_syst[0]->Draw("SAME E3");
  leg->Draw();
  c->cd(2);
  hspec_stat[1]->Draw("SAME E3");
  hspec_syst[1]->Draw("SAME E3");
  leg->Draw();

}
