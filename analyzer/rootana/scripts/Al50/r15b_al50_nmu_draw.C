#include "./scripts/Al50/util.h"

#include "Rtypes.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TH1.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "TStyle.h"

namespace {
  TH1 *hemu;
  TH1 *hgehi_e_all,  *hgelo_e_all;
  TH1 *hgehi_e_tcut, *hgelo_e_tcut;
  TH1 *hgehi_t_ecut, *hgelo_t_ecut;
}

void LoadHistograms(TFile* f) {
  hemu = (TH1*)f->Get("hemu");
  hgehi_e_all  = (TH1*)f->Get("hgehi_e_all");
  hgelo_e_all  = (TH1*)f->Get("hgelo_e_all");
  hgehi_e_tcut = (TH1*)f->Get("hgehi_e_tcut");
  hgelo_e_tcut = (TH1*)f->Get("hgelo_e_tcut");
  hgehi_t_ecut = (TH1*)f->Get("hgehi_t_ecut");
  hgelo_t_ecut = (TH1*)f->Get("hgelo_t_ecut");
}

void DrawFullScaleHistograms() {
  hgehi_e_all->SetStats(false);
  hgelo_e_all->SetStats(false);
  hgehi_e_all->SetTitle("Ge Spectrum (Al50 Target) all times;Energy [keV];Raw Count");
  hgehi_e_all->GetXaxis()->SetTitleOffset(1.2);
  hgehi_e_all->GetYaxis()->SetTitleOffset(1.4);
  hgehi_e_all->SetLineColor(kRed);
  hgehi_e_all->GetXaxis()->SetRangeUser(150., 1200.);
  TLegend* l_all = new TLegend(0.7, 0.7, 0.9, 0.9);
  l_all->AddEntry(hgehi_e_all, "Ge High Gain");
  l_all->AddEntry(hgelo_e_all, "Ge Low Gain");

  TCanvas* c_all = new TCanvas;
  hgehi_e_all->Draw();
  hgelo_e_all->Draw("SAME");
  l_all->Draw();
  c_all->SaveAs("~/data/results/Al50/normalisation/ge_e.png");
}

struct Norm {
  int n;
  double en, chi2, ndf;
  double Chi2_red() { return ((double)chi2)/ndf; }
  int Chi2()        { return (int)chi2;          }
  int Ndf()         { return (int)ndf;           }
};

void ConstructFitFunctions(TF1*& hi, TF1*& lo) {
  hi = new TF1("fcn_hi",
               "pol1(0)+[2]*exp(-0.5*(x-[3])^2/[6]^2)+[4]*exp(-0.5*(x-[5])^2/[6]^2)",
               335, 355);
  lo = new TF1("fcn_lo",
               "pol1(0)+[2]*exp(-0.5*(x-[3])^2/[6]^2)+[4]*exp(-0.5*(x-[5])^2/[6]^2)",
               335, 355);
  hi->SetParameters(1.5e3, 0., 10e3, 345., 2e3, 347.5, 1.);
  lo->SetParameters(1.5e3, 0., 13e3, 344., 2e3, 349.5, 1.);
  hi->SetParName(0, "Bkg y-int"); lo->SetParName(0, "Bkg y-int");
  hi->SetParName(1, "Bkg Slope"); lo->SetParName(1, "Bkg Slope");
  hi->SetParName(2, "Al Amp");    lo->SetParName(2, "Al Amp");
  hi->SetParName(3, "Al Energy"); lo->SetParName(3, "Al Energy");
  hi->SetParName(4, "Pb Amp");    lo->SetParName(4, "Pb Amp");
  hi->SetParName(5, "Pb Energy"); lo->SetParName(5, "Pb Energy");
  hi->SetParName(6, "Sigma");     lo->SetParName(6, "Sigma");
  hi->SetLineColor(kRed);
  lo->SetLineColor(kBlue);
}

Norm FitGeHist(TH1* h, TF1* f) {
  double I = 0.798, dI = 0.008, E = 347.;
  f->Print();
  Double_t pars[7];
  f->GetParameters(pars);
  printf("%g %g %g %g %g %g %g\n", pars[0], pars[1], pars[2], pars[3], pars[4], pars[5], pars[6]);
  h->Fit(f, "R");
  f->GetParameters(pars);
  printf("%g %g %g %g %g %g %g\n", pars[0], pars[1], pars[2], pars[3], pars[4], pars[5], pars[6]);
  TFitResultPtr r = h->Fit(f, "SR");
  Norm N = {
    (int)GeFcn::NMuHi(r->Parameter(2), r->Parameter(6), h->GetBinWidth(1), I, E),
    GeFcn::NMuErrHi(r->Parameter(2), r->ParError(2), r->Parameter(6),
                    r->ParError(6), h->GetBinWidth(0), I, dI, E),
    r->Chi2(), (double)r->Ndf() };
  return N;
}

void DrawFits(TH1* hi, TH1* lo, Norm& nhi, Norm& nlo) {
  TCanvas* c_fit = new TCanvas;
  hi->GetXaxis()->SetRangeUser(335., 355.);
  lo->GetXaxis()->SetRangeUser(335., 355.);
  hi->SetTitle("X-Ray Spectrum (Al50 Target) #pm200ns;Energy [keV];Raw Count");
  hi->GetXaxis()->SetTitleOffset(1.2);
  hi->GetYaxis()->SetTitleOffset(1.4);
  hi->SetLineColor(kRed);
  lo->SetLineColor(kBlue);
  gStyle->SetOptStat("n");
  gStyle->SetOptFit(111);
  hi->SetStats(kTRUE);
  lo->SetStats(kTRUE);
  hi->SetName("GeHiGain");
  lo->SetName("GeLoGain");
  hi->Draw();
  c_fit->Update();
  TPaveStats* stats_hi = (TPaveStats*)hi->GetListOfFunctions()->FindObject("stats");
  lo->Draw();
  c_fit->Update();
  TPaveStats* stats_lo = (TPaveStats*)lo->GetListOfFunctions()->FindObject("stats");
  stats_hi->SetX1NDC(0.10); stats_hi->SetX2NDC(0.35);
  stats_hi->SetY1NDC(0.50); stats_hi->SetY2NDC(0.90);
  stats_lo->SetX1NDC(0.10); stats_lo->SetX2NDC(0.35);
  stats_lo->SetY1NDC(0.10); stats_lo->SetY2NDC(0.50);
  TLegend* l_fit = new TLegend(0.55, 0.70, 0.95, 0.90);
  char l_fit_hi[64], l_fit_lo[64];
  sprintf(l_fit_hi, "Ge High Gain (%.3g#pm%.2g #mu)", (double)nhi.n, nhi.en);
  sprintf(l_fit_lo, "Ge Low Gain (%.3g#pm%.2g #mu)",  (double)nlo.n, nlo.en);
  l_fit->AddEntry(hi, l_fit_hi);
  l_fit->AddEntry(lo, l_fit_lo);
  hi->Draw();
  lo->Draw("SAME");
  l_fit->Draw();
  c_fit->Update();
  c_fit->SaveAs("~/data/results/Al50/normalisation/ge_e_zoom.png");
}

void r15b_al50_nmu_draw () {
  const char IFNAME[] = "~/data/results/Al50/normalisation/nmual50.root";
  TFile* f = new TFile(IFNAME);
  LoadHistograms(f);

  // Draw SiT energy spectrum (muons)
  // gStyle->SetOptStat("e");
  // TCanvas* cmu = new TCanvas();
  // hemu->SetTitle("Energy of Central Muon in each TME;E [keV];Count [/2.5keV]");
  // hemu->GetXaxis()->SetRangeUser(0, 8e3);
  // hemu->Draw();
  // cmu->SetLogy();
  // cmu->SaveAs("img/incoming_muons.png");

  // Fit and draw Ge spectra
  // DrawFullScaleHistograms();
  TH1* hhi = hgehi_e_tcut;
  TH1* hlo = hgelo_e_tcut;

  TF1 *fcn_hi, *fcn_lo;
  ConstructFitFunctions(fcn_hi, fcn_lo);
  Norm nhi = FitGeHist(hhi, fcn_hi);
  Norm nlo = FitGeHist(hlo, fcn_lo);
  DrawFits(hhi, hlo, nhi, nlo);

  printf("Hi chi2 %d/%d (%f), n %d +/- %f\n",
         nhi.Chi2(), nhi.Ndf(), nhi.Chi2_red(), nhi.n, nhi.en);
  printf("Lo chi2 %d/%d (%f), n %d +/- %f\n",
         nlo.Chi2(), nlo.Ndf(), nlo.Chi2_red(), nlo.n, nlo.en);
}
