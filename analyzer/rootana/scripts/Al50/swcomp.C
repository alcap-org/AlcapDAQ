#include "util.h"

#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH1D.h"
#include "TLegend.h"

TH1* swgr2hist(TGraph* g) {
  int n = g->GetN();
  Double_t *xs  = g->GetX(),   *ys  = g->GetY();
  Double_t *exs = g->GetEX(), *eys = g->GetEY();
  TH1* h = new TH1D("", "", 365, 150, 26100);
  for (int i = 1; i <= n; ++i) {
    h->SetBinContent(i, ys[i]);
    h->SetBinError  (i, eys[i]);
  }
  return h;
}

void swcomp() {
  TFile* fsw = new TFile("data/SW-plots.root");
  TFile* fal = new TFile("data/Al50/en.p.t400ns.dt200ns.rebin5.pp10us.elo2e3keV.ehi12e3keV.bay5.pidcorr.root");

  TGraphErrors* gsw = (TGraphErrors*)fsw->Get("Graph");
  TH1* hal[2] = { (TH1*)fal->Get("hl_e_reco_bay"),
                  (TH1*)fal->Get("hr_e_reco_bay") };

  TList* fits = gsw->GetListOfFunctioons();

  hal[0]->Scale(1./Normalization::Al50());
  hal[1]->Scale(1./Normalization::Al50());
  hal[0]->Scale(1./hal[0]->GetBinWidth(1));
  hal[1]->Scale(1./hal[1]->GetBinWidth(1));
  TH1* hsw = swgr2hist(gsw);
  hal[0]->SetLineColor(kBlue);
  hal[1]->SetLineColor(kRed);
  hsw   ->SetLineColor(kBlack);
  gsw->Draw();
  hal[0]->Draw("SAME");
  hal[1]->Draw("SAME");
  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(hal[0], "SiL");
  leg->AddEntry(hal[1], "SiR");
  leg->AddEntry(hsw,    "SW");
  leg->Draw();
  double nhl = 500*hal[0]->Integral(hal[0]->FindFixBin(3.5e3), hal[0]->FindFixBin(10.e3));
  double nhr = 500*hal[1]->Integral(hal[1]->FindFixBin(3.5e3), hal[1]->FindFixBin(10.e3));
  double nsw = 75*hsw   ->Integral(hsw   ->FindFixBin(3.5e3), hsw   ->FindFixBin(10.e3));
  std::cout << "Left: "  << nhl  << std::endl;
  std::cout << "Right: " << nhr  << std::endl;
  std::cout << "SW: "    << nsw  << std::endl;
}
