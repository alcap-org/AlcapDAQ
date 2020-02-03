#include "./scripts/Al50/util.h"

#include "Rtypes.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TH1.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TPave.h"
#include "TPaveStats.h"
#include "TRatioPlot.h"
#include "TStyle.h"

#include <cmath>
using std::erfc;
using std::exp;
using std::pow;
using std::sqrt;

namespace {
  TH1 *hemu;
  TH1 *hgehi_e_all,     *hgelo_e_all;
  TH1 *hgehi_e_early,   *hgelo_e_early;
  TH1 *hgehi_e_prompt,  *hgelo_e_prompt;
  TH1 *hgehi_e_delayed, *hgelo_e_delayed;
  TH1 *hgehi_e_oot,     *hgelo_e_oot;
  TH1 *hgehi_t_ecut,    *hgelo_t_ecut;
}

double FitFcn(double* x, double* par) {
  static const double RT2 = sqrt(2);
  double f = par[0]+par[1]*x[0];
  f += par[2]*exp(-0.5*pow((x[0]-par[3])/par[6], 2));
  f += par[4]*exp(-0.5*pow((x[0]-par[5])/par[6], 2));
  f += 0.5*par[7]*par[2]*exp((x[0]-par[3])/par[8])*erfc((RT2*par[6])/(2*par[8])+(x[0]-par[3])/(RT2*par[6]));
  f += 0.5*par[7]*par[4]*exp((x[0]-par[5])/par[8])*erfc((RT2*par[6])/(2*par[8])+(x[0]-par[5])/(RT2*par[6]));
  return f;
}

void LoadHistograms(TFile* f) {
  hemu            = (TH1*)f->Get("hemu");
  hgehi_e_all     = (TH1*)f->Get("hgehi_e_all");
  hgelo_e_all     = (TH1*)f->Get("hgelo_e_all");
  hgehi_e_early   = (TH1*)f->Get("hgehi_e_early");
  hgelo_e_early   = (TH1*)f->Get("hgelo_e_early");
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
  // hi = new TF1("fcn_hi", FitFcn, 335, 355, 9);
  // lo = new TF1("fcn_hi", FitFcn, 335, 355, 9);

  //////////////////////////////////////////////////////////////////////////////
  // SWITCH
  //////////////////////////////////////////////////////////////////////////////
  // hi->SetParameters(1.5e3, 0., 10e3, 345, 2e3, 349.5, 1.); // Al50 total
  // lo->SetParameters(1.5e3, 0., 13e3, 345, 2e3, 348.5, 1.); // Al50 total
  hi->SetParameters(1e3, 0., 10e3, 345, 1e3, 349.5, 1.); // Al100 total
  lo->SetParameters(1e3, 0., 8e3,  347, 1e3, 350.,  1.); // Al100 total
  hi->SetParLimits(8, -3.2, 3.2);
  lo->SetParLimits(8, -3.2, 3.2);
  hi->SetParLimits(9, 0, 200);
  lo->SetParLimits(9, 0, 200);
  // hi->SetParameters(1.5e3, 0., 10e3, 345, 2e3, 349.5, 1., 0.1, 0.1); // Al50 total
  // lo->SetParameters(1.5e3, 0., 13e3, 345, 2e3, 348.5, 1., 0.1, 0.1); // Al50 total
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
  h->Fit(f, "R");
  TFitResultPtr r = h->Fit(f, "MESRL");
  double A  = r->Parameter(2);
  double s  = r->Parameter(6);
  double eA = r->ParError(2);
  double es = r->ParError(6);
  double db = h->GetBinWidth(1);
  if (r->Parameter(5) < r->Parameter(3)) {
    A  = r->Parameter(4);
    s  = r->Parameter(6);
    eA = r->ParError(4);
    es = r->ParError(6);
    db = h->GetBinWidth(1);
  }
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
  if (prompt_only) {
    hi->SetTitle("X-Ray Spectrum (Al50 Target) #pm200ns;Energy [keV];Raw Count");
    lo->SetTitle("X-Ray Spectrum (Al50 Target) #pm200ns;Energy [keV];Raw Count");
  } else {
    hi->SetTitle("X-Ray Spectrum (Al50 Target) all time;Energy [keV];Raw Count");
    lo->SetTitle("X-Ray Spectrum (Al50 Target) all time;Energy [keV];Raw Count");
  }
  SetStyle(hi);
  hi->SetLineColor(kBlack);
  // hi->SetLineColor(kRed);
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
  // stats_hi->SetX1NDC(0.10); stats_hi->SetX2NDC(0.40);
  // stats_hi->SetY1NDC(0.20); stats_hi->SetY2NDC(0.90);
  stats_lo->SetX1NDC(0.10); stats_lo->SetX2NDC(0.35);
  stats_lo->SetY1NDC(0.10); stats_lo->SetY2NDC(0.50);
  TLegend* l_fit = new TLegend(0.55, 0.70, 0.95, 0.90);
  // TPaveText* l_fit = new TPaveText(348., 12e3, 355, 14e3);
  char l_fit_hi[64], l_fit_lo[64];
  std::sprintf(l_fit_hi, "Ge High Gain (%.3g#pm%.2g #mu)", (double)nhi.n, nhi.en);
  std::sprintf(l_fit_lo, "Ge Low Gain (%.3g#pm%.2g #mu)",  (double)nlo.n, nlo.en);
  l_fit->AddEntry(hi, l_fit_hi);
  // l_fit->AddText(l_fit_hi);
  l_fit->AddEntry(lo, l_fit_lo);
  hi->Draw();
  lo->Draw("SAME");
  l_fit->Draw();
  // DrawBlindedBox();
  DrawPeakLabels();
  c_fit->Update();
  c_fit->SaveAs("img/ge_e_zoom.png");


  stats_lo->SetX1NDC(0.10); stats_lo->SetX2NDC(0.40);
  stats_lo->SetY1NDC(0.30); stats_lo->SetY2NDC(0.90);
  stats_hi->SetX1NDC(0.10); stats_hi->SetX2NDC(0.40);
  stats_hi->SetY1NDC(0.30); stats_hi->SetY2NDC(0.90);
  TRatioPlot* rat_lo = new TRatioPlot(lo);
  TRatioPlot* rat_hi = new TRatioPlot(hi);
  TCanvas* c_res = new TCanvas("c_res", "c_res", 1400, 500);
  c_res->Divide(2);
  c_res->cd(1);
  rat_lo->Draw();
  c_res->cd(2);
  rat_hi->Draw();
  // rat_lo->GetLowerRefGraph()->GetYaxis()->SetRangeUser(-1e3, 1e3);
  // rat_hi->GetLowerRefGraph()->GetYaxis()->SetRangeUser(-1e3, 1e3);
  c_res->Update();
  c_res->SaveAs("img/xrawfit_residuals.png");

  stats_lo->SetX1NDC(0.10); stats_lo->SetX2NDC(0.40);
  stats_lo->SetY1NDC(0.70); stats_lo->SetY2NDC(0.90);
  stats_hi->SetX1NDC(0.10); stats_hi->SetX2NDC(0.40);
  stats_hi->SetY1NDC(0.70); stats_hi->SetY2NDC(0.90);
  TH1* hresdiff[2] = { (TH1*)lo->Clone("hresdifflo"),
                       (TH1*)hi->Clone("hresdiffhi") };
  hresdiff[0]->GetListOfFunctions()->Clear();
  hresdiff[1]->GetListOfFunctions()->Clear();
  hresdiff[0]->SetTitle("GeLo Fit Diff;Energy [keV];hist-fit");
  hresdiff[1]->SetTitle("GeHi Fit Diff;Energy [keV];hist-fit");
  lo->SetTitle("GeLo Spectrum");
  hi->SetTitle("GeHi Spectrum");
  lo         ->GetYaxis()->SetRangeUser(1e3, 13e3);
  hresdiff[0]->GetYaxis()->SetRangeUser(-9e2, 7e2);
  hi         ->GetYaxis()->SetRangeUser(1e3, 15e3);
  hresdiff[1]->GetYaxis()->SetRangeUser(-7e2, 7e2);
  TPave* boxspec[2] = { new TPave(341.,  1e3, 347.,  13e3, 0, "NB"),
                        new TPave(342.,  1e3, 348.,  15e3, 0, "NB") };
  TPave* boxdiff[2] = { new TPave(341., -900., 347., 700., 0, "NB"),
                        new TPave(342., -700., 348., 700., 0, "NB") };
  boxspec[0]->SetFillStyle(3003); boxspec[0]->SetFillColor(kBlack);
  boxspec[1]->SetFillStyle(3003); boxspec[1]->SetFillColor(kBlack);
  boxdiff[0]->SetFillStyle(3003); boxdiff[0]->SetFillColor(kBlack);
  boxdiff[1]->SetFillStyle(3003); boxdiff[1]->SetFillColor(kBlack);
  TF1* fgaus[2] = { new TF1("fgauslo", "gaus(0)", 341, 347.),
                    new TF1("fgaushi", "gaus(0)", 342, 348.) };
  for (int i = 1; i <= lo->GetNbinsX(); ++i) {
    TF1* f = lo->GetFunction("fcn_lo");
    hresdiff[0]->SetBinContent(i, lo->GetBinContent(i) -
                               f->Eval(lo->GetBinCenter(i)));
    fgaus[0]->SetParameters(f->GetParameters()+2);

  }
  for (int i = 0; i <= hi->GetNbinsX(); ++i) {
    TF1* f = hi->GetFunction("fcn_hi");
    hresdiff[1]->SetBinContent(i, hi->GetBinContent(i) -
                               f->Eval(hi->GetBinCenter(i)));
    fgaus[1]->SetParameters(f->GetParameters()+2);
  }

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(100);
  TCanvas* cfdiff = new TCanvas("cfdiff", "cfdiff", 1400, 1000);
  cfdiff->Divide(2, 2);
  cfdiff->cd(1);
  lo->Draw();
  boxspec[0]->Draw();
  cfdiff->cd(2);
  hi->Draw();
  boxspec[1]->Draw();
  cfdiff->cd(3);
  hresdiff[0]->Draw();
  boxdiff[0]->Draw();
  cfdiff->cd(4);
  hresdiff[1]->Draw();
  boxdiff[1]->Draw();
  cfdiff->SaveAs("img/xrayfit_raw_residuals.png");


  std::pair<double,double> dlo = HistUtils::IntegralByUserLimits(hresdiff[0], 341, 347);
  std::pair<double,double> dhi = HistUtils::IntegralByUserLimits(hresdiff[1], 342, 348);
  std::cout << "Difference between histogram and fit function under peak.\nRaw (err) percent%" << std::endl;
  std::cout << dlo.first << " (" << dlo.second << ") or "
            << 100.*dlo.first/fgaus[0]->Integral(341, 347) << "%" <<std::endl
            << dhi.first << " (" << dhi.second << ") or "
            << 100.*dhi.first/fgaus[1]->Integral(342, 348) << "%" << std::endl;

}

void r15b_al50_nmu_draw(const char* ifname, bool prompt_only=true) {
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
