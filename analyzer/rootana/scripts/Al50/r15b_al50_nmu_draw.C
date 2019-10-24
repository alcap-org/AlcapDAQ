#include "./scripts/Al50/util.h"

#include "Rtypes.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TH1.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "TStyle.h"

namespace {
  TH1 *hemu;
  TH1 *hgehi_e_all,     *hgelo_e_all;
  TH1 *hgehi_e_prompt,  *hgelo_e_prompt;
  TH1 *hgehi_e_delayed, *hgelo_e_delayed;
  TH1 *hgehi_e_oot,     *hgelo_e_oot;
  TH1 *hgehi_t_ecut,    *hgelo_t_ecut;
}

void LoadHistograms(TFile* f) {
  hemu            = (TH1*)f->Get("hemu");
  hgehi_e_all     = (TH1*)f->Get("hgehi_e_all");
  hgelo_e_all     = (TH1*)f->Get("hgelo_e_all");
  hgehi_e_prompt  = (TH1*)f->Get("hgehi_e_prompt");
  hgelo_e_prompt  = (TH1*)f->Get("hgelo_e_prompt");
  hgehi_e_delayed = (TH1*)f->Get("hgehi_e_delayed");
  hgelo_e_delayed = (TH1*)f->Get("hgelo_e_delayed");
  hgehi_e_oot     = (TH1*)f->Get("hgehi_e_oot");
  hgelo_e_oot     = (TH1*)f->Get("hgelo_e_oot");
  hgehi_t_ecut    = (TH1*)f->Get("hgehi_t_ecut");
  hgelo_t_ecut    = (TH1*)f->Get("hgelo_t_ecut");
}

void DrawSiTHistograms() {
  TCanvas* cmu = new TCanvas();
  hemu->SetTitle("Energy of Central Muon in each TME;E [keV];Count [/2.5keV]");
  hemu->GetXaxis()->SetRangeUser(0, 8e3);
  hemu->Draw();
  cmu->SetLogy();
  cmu->SaveAs("img/incoming_muons.png");
}

void DrawFullScaleGeHistograms() {
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
  c_all->SaveAs("img/ge_e.png");
}

struct Norm {
  int n, nx;
  double en, enx, chi2, ndf;
  double Chi2_red() { return ((double)chi2)/ndf; }
  int Chi2()        { return (int)chi2;          }
  int Ndf()         { return (int)ndf;           }
  int NXrays()      { return nx;                 }
};

void ConstructFitFunctions(TF1*& hi, TF1*& lo) {
  hi = new TF1("fcn_hi",
               "pol1(0)+[2]*exp(-0.5*(x-[3])^2/[6]^2)+[4]*exp(-0.5*(x-[5])^2/[6]^2)",
               335, 355);
  lo = new TF1("fcn_lo",
               "pol1(0)+[2]*exp(-0.5*(x-[3])^2/[6]^2)+[4]*exp(-0.5*(x-[5])^2/[6]^2)",
               335, 355);
  //////////////////////////////////////////////////////////////////////////////
  // SWITCH
  //////////////////////////////////////////////////////////////////////////////
  hi->SetParameters(1.5e3, 0., 10e3, 345, 2e3, 349.5, 1.); // Al50 total
  lo->SetParameters(1.5e3, 0., 13e3, 345, 2e3, 348.5, 1.); // Al50 total
  // hi->SetParameters(20,    0., 200,  345, 31,  350,  1.); // Al100 29.9 MeV/c
  // lo->SetParameters(20,    0., 200,  345, 31,  350,  1.); // Al100 29.9 MeV/c
  // hi->SetParameters(20,    0., 200,  345, 31,  350,  1.); // Al100 26.2 MeV/c
  // lo->SetParameters(20,    0., 200,  345, 31,  350,  1.); // Al100 26.2 MeV/c
  // hi->SetParameters(100,   0., 690,  345, 111, 350,  1.); // Al100 26.4 MeV/c
  // lo->SetParameters(100,   0., 690,  345, 111, 350,  1.); // Al100 26.4 MeV/c
  // hi->SetParameters(1e3, 0, 10e3, 345., 1e3, 349.5, 1); // Al100
  // lo->SetParameters(1e3, 0, 9e3,  346., 1e3, 350,   1); // Al100
  hi->SetParNames("Bkg y-int", "Bkg Slope", "Al Amp", "Al Energy", "Pb Amp", "Pb Energy", "Sigma");
  lo->SetParNames("Bkg y-int", "Bkg Slope", "Al Amp", "Al Energy", "Pb Amp", "Pb Energy", "Sigma");
  hi->SetLineColor(kBlack);
  // hi->SetLineColor(kRed);
  lo->SetLineColor(kBlue);
}

// Norm RooFitGeHist(TH1* h) {
//   RooWorkSpace* ws = new RooWorkspace("rws");
//   ws.factory
// }

Norm FitGeHist(TH1* h, TF1* f) {
  double I = 0.798, dI = 0.008, E = 347.;
  h->Fit(f, "RQ");
  TFitResultPtr r = h->Fit(f, "MESRQ");
  double A  = r->Parameter(2);
  double s  = r->Parameter(6);
  double eA = r->ParError(2);
  double es = r->ParError(6);
  double db = h->GetBinWidth(1);
  Norm N = {
    (int)GeFcn::NMuHi(A, s, db, I, E), (int)GeFcn::GaussArea(A, s, db),
    GeFcn::NMuErrHi(A, eA, s, es, db, I, dI, E),
    GeFcn::GaussAreaErr(A, eA, s, es, db),
    r->Chi2(), (double)r->Ndf() };
  return N;
}

void DrawBlindedBox() {
  TPaveText* p = new TPaveText(0.1, 0.5, 0.35, 0.90, "NDC");
  p->AddText(0, 0.9, "Fit Result")->SetTextSize(0.05);
  p->AddLine(0., 0.8, 1, 0.8);
  p->AddText("Blinded");
  p->Draw();
}

void DrawPeakLabels() {
  TText* al = new TLatex(343.8, 3000, "Al 2p-1s");
  TText* pb = new TLatex(347.5, 4500, "#mu+Pb#rightarrow^{207}Tl");
  al->Draw();
  pb->Draw();
}

void SetStyle(TH1* h) {
  h->GetXaxis()->SetLabelSize(0.040);
  h->GetYaxis()->SetLabelSize(0.040);
  h->GetXaxis()->SetTitleSize(0.040);
  h->GetYaxis()->SetTitleSize(0.040);
  // h->GetXaxis()->SetTitleOffset(1.02);
  h->GetYaxis()->SetTitleOffset(1.35);
}

void DrawFits(TH1* hi, TH1* lo, Norm& nhi, Norm& nlo, bool prompt_only) {
  TCanvas* c_fit = new TCanvas;
  hi->GetXaxis()->SetRangeUser(335., 355.);
  lo->GetXaxis()->SetRangeUser(335., 355.);
  if (prompt_only)
    hi->SetTitle("X-Ray Spectrum (Al50 Target) #pm200ns;Energy [keV];Raw Count");
  else
    hi->SetTitle("X-Ray Spectrum (Al50 Target) all time;Energy [keV];Raw Count");
  SetStyle(hi);
  hi->SetLineColor(kBlack);
  // hi->SetLineColor(kRed);
  lo->SetLineColor(kBlue);
  gStyle->SetOptStat("n");
  gStyle->SetOptFit(111);
  hi->SetStats(kTRUE);
  lo->SetStats(kTRUE);
  // hi->SetName("GeHiGain");
  hi->SetName("GeFit");
  lo->SetName("GeLoGain");
  hi->Draw();
  c_fit->Update();
  TPaveStats* stats_hi = (TPaveStats*)hi->GetListOfFunctions()->FindObject("stats");
  lo->Draw();
  c_fit->Update();
  TPaveStats* stats_lo = (TPaveStats*)lo->GetListOfFunctions()->FindObject("stats");
  // stats_hi->SetX1NDC(0.10); stats_hi->SetX2NDC(0.35);
  // stats_hi->SetY1NDC(0.50); stats_hi->SetY2NDC(0.90);
  stats_hi->SetX1NDC(0.10); stats_hi->SetX2NDC(0.40);
  stats_hi->SetY1NDC(0.20); stats_hi->SetY2NDC(0.90);
  stats_lo->SetX1NDC(0.10); stats_lo->SetX2NDC(0.35);
  stats_lo->SetY1NDC(0.10); stats_lo->SetY2NDC(0.50);
  // TLegend* l_fit = new TLegend(0.55, 0.70, 0.95, 0.90);
  TPaveText* l_fit = new TPaveText(348., 12e3, 355, 14e3);
  char l_fit_hi[64], l_fit_lo[64];
  sprintf(l_fit_hi, "Ge (%04.3g#pm%.2g #mu)", (double)nhi.n, nhi.en);
  // sprintf(l_fit_hi, "Ge High Gain (%.3g#pm%.2g #mu)", (double)nhi.n, nhi.en);
  sprintf(l_fit_hi, "%.3g#pm%.2g #mu", (double)nhi.n, nhi.en);
  sprintf(l_fit_lo, "Ge Low Gain (%.3g#pm%.2g #mu)",  (double)nlo.n, nlo.en);
  // l_fit->AddEntry(hi, l_fit_hi);
  l_fit->AddText(l_fit_hi);
  // l_fit->AddEntry(lo, l_fit_lo);
  hi->Draw();
  // lo->Draw("SAME");
  l_fit->Draw();
  // DrawBlindedBox();
  DrawPeakLabels();
  c_fit->Update();
  c_fit->SaveAs("img/ge_e_zoom.png");
}

void r15b_al50_nmu_draw (const char* ifname, bool prompt_only=true) {
  TFile* f = new TFile(ifname);
  LoadHistograms(f);
  gStyle->SetOptStat("e");

  // Draw SiT energy spectrum (muons)
  // DrawSiTHistograms();

  // Fit and draw Ge spectra
  // DrawFullScaleGeHistograms();
  TH1 *hhi, *hlo;
  if (prompt_only) {
    hhi = hgehi_e_prompt;
    hlo = hgelo_e_prompt;
  } else {
    hhi = hgehi_e_all;
    hlo = hgelo_e_all;
  }

  TF1 *fcn_hi, *fcn_lo;
  ConstructFitFunctions(fcn_hi, fcn_lo);
  Norm nhi = FitGeHist(hhi, fcn_hi);
  Norm nlo = FitGeHist(hlo, fcn_lo);
  DrawFits(hhi, hlo, nhi, nlo, prompt_only);

  printf("Hi chi2 %d/%d (%f), n %d +/- %f, nx %d +/- %f\n",
         nhi.Chi2(), nhi.Ndf(), nhi.Chi2_red(), nhi.n, nhi.en, nhi.nx, nhi.enx);
  printf("Lo chi2 %d/%d (%f), n %d +/- %f, nx %d +/- %f\n",
         nlo.Chi2(), nlo.Ndf(), nlo.Chi2_red(), nlo.n, nlo.en, nlo.nx, nlo.enx);
}
