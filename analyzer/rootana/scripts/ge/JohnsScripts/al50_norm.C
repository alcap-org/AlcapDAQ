#include "Rtypes.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH1D.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TMath.h"
#include "TPad.h"

#include <cmath>
#include <iostream>
#include <vector>
using std::sqrt;
using std::vector;

namespace {
  double XRAYEFF          = 6.6e-4;
  double XRAYEFFERR       = 0.2e-4;
  double XRAYINTENSITY    = 0.798;
  double XRAYINTENSITYERR = 0.008;
  double ELIMFIT[2]       = { 335., 355. };
};

// Assumes uniform bin width
double NMu(const TFitResultPtr& res) {
  return TMath::Sqrt(2.*TMath::Pi())*3.*
         res->Parameter(4)*res->Parameter(2)/
         XRAYINTENSITY/XRAYEFF;
}

double NMuRelativeErr(TFitResultPtr res) {
  double eI = XRAYINTENSITYERR/XRAYINTENSITY;
  double eA = res->ParError(2)/res->Parameter(2);
  double eS = res->ParError(4)/res->Parameter(4);
  return sqrt(eI*eI+eA*eA+eS*eS);
}

void PrintFitError(const TFitResultPtr& r) {
  std::cout << "Chi2/ndf:\t" << r->Chi2() << "/" << r->Ndf() << std::endl
            << "Nmu (err):\t" << NMu(r) << " (" << NMuRelativeErr(r)*NMu(r)
            << ")" << std::endl;
}

TF1* FitGeSpectrum(TH1* h) {
  TF1* f = new TF1("fcn_hi",
                   "pol1(0)+gaus(2)+2*[2]*exp(-0.5*(x-([3]+0.266))^2/[4]^2)+"
                   "[5]*exp(-0.5*(x-[6])^2/[4]^2)",
                   ELIMFIT[0], ELIMFIT[1]);
  f->SetParNames("Bkg y-int", "Bkg Slope",
                 "Al 2p_{1/2}-1s Amp.", "Al 2p_{1/2}-1s Energy",
                 "Resolution (#sigma)", "Pb #gamma Amp.", "Pb #gamma Energy");
  f->SetParameters(7.5e3, 0., 10e3, 345., 2., 2e3, 350.);
  TFitResultPtr r = h->Fit(f, "MESRLN");
  PrintFitError(r);
  return f;
}

void Rebin_keVtoMeV(TH1* h) {
  const int nbins = h->GetXaxis()->GetNbins();
  double new_bins[nbins+1];
  for(int i=0; i <= nbins; ++i)
    new_bins[i] = h->GetBinLowEdge(i+1)/1e3;
  h->SetBins(nbins, new_bins);
}

void Rebin_MeVtokeV(TH1* h) {
  const int nbins = h->GetXaxis()->GetNbins();
  double new_bins[nbins+1];
  for(int i=0; i <= nbins; ++i)
    new_bins[i] = h->GetBinLowEdge(i+1)*1e3;
  h->SetBins(nbins, new_bins);
}

TLatex* BuildDatasetLabel() {
  TLatex* txt = new TLatex(360., 40.0e3, "Al 50#mum");
  txt->SetTextSize(0.06);
  return txt;
}

TLatex* BuildChi2Label() {
  TLatex* txt = new TLatex(336., 25e3, "#splitline{#chi^{2}/ndf}{= 200/101}");
  txt->SetTextSize(0.07);
  return txt;
}

TLatex* BuildNStopLabel() {
  TLatex* txt =
  new TLatex(336., 35e3, "#splitline{163(1)#times10^{6}}{#mu-stops}");
  txt->SetTextSize(0.07);
  txt->SetTextAlign(11);
  return txt;
}

vector<TLatex*> BuildPeakLabels() {
  vector<TLatex*> labs;
  labs.push_back(new TLatex(270., 37.0e3, "#muPb 6-5"));
  labs.push_back(new TLatex(270., 22.5e3, "#muPb#rightarrowTl"));
  labs.push_back(new TLatex(329., 36.5e3, "#muAl 2p-1s"));
  labs.push_back(new TLatex(405., 21.0e3, "#muPb 5g_{9/2}-4f_{7/2}"));
  labs.push_back(new TLatex(475., 17.0e3, "#muPb 5g_{7/2}-4f_{5/2}"));
  labs.push_back(new TLatex(520,  27.0e3, "511 keV"));
  labs.push_back(new TLatex(915,  2.2e3,  "#muPb 4f_{7/2}-3d_{5/2}"));
  labs.push_back(new TLatex(1010, 2.2e3,  "#muPb 4f_{5/2}-3d_{3/2}"));
  for (int i = 0; i < labs.size(); ++i) {
    labs[i]->SetTextAngle(90.);
    labs[i]->SetTextSize (0.033);
  }
  return labs;
}

TGraph* BuildPeakMarkers() {
  TGraph* g = new TGraph();
  int i = -1;
  g->SetPoint(++i, 232.,  43.6e3);
  g->SetPoint(++i, 266.,  21.7e3);
  g->SetPoint(++i, 347.,  43.8e3);
  g->SetPoint(++i, 429.,  23.7e3);
  g->SetPoint(++i, 439,   20.2e3);
  g->SetPoint(++i, 511.,  26e3);
  g->SetPoint(++i, 937.8, 9e3);
  g->SetPoint(++i, 971.7, 7e3);
  g->SetMarkerColor(kBlack);
  g->SetMarkerStyle(kFullTriangleDown);
  return g;
}

vector<TF1*> BuildDisjointFcns(const TF1* f) {
  double dE = 0.266; // Fine splitting energy
  double mA = 2.;    // Fine splitting relative populations
  vector<TF1*> fs(4, nullptr);
  fs[0] = new TF1("fitfcn_contbg",  "pol1(0)", ELIMFIT[0], ELIMFIT[1]);
  fs[1] = new TF1("fitfcn_2p1shf1", "gaus(0)", ELIMFIT[0], ELIMFIT[1]);
  fs[2] = new TF1("fitfcn_2p1shf2", "gaus(0)", ELIMFIT[0], ELIMFIT[1]);
  fs[3] = new TF1("fitfcn_pbbg",    "gaus(0)", ELIMFIT[0], ELIMFIT[1]);
  fs[0]->SetLineColor(kBlack);
  fs[1]->SetLineColor(kBlack);
  fs[2]->SetLineColor(kBlack);
  fs[3]->SetLineColor(kBlack);
  fs[0]->SetLineStyle(3);
  fs[1]->SetLineStyle(5);
  fs[2]->SetLineStyle(7);
  fs[3]->SetLineStyle(1);
  fs[0]->SetLineWidth(2);
  fs[1]->SetLineWidth(1);
  fs[2]->SetLineWidth(1);
  fs[3]->SetLineWidth(1);
  fs[0]->SetParameters(f->GetParameter(0), f->GetParameter(1));
  fs[1]->SetParameters(f->GetParameter(2), f->GetParameter(3),
                       f->GetParameter(4));
  fs[2]->SetParameters(2*f->GetParameter(2), dE+f->GetParameter(3),
                       f->GetParameter(4));
  fs[3]->SetParameters(f->GetParameter(5), f->GetParameter(6),
                       f->GetParameter(4));
  fs[0]->SetParNames(f->GetParName(0), f->GetParName(1));
  fs[1]->SetParNames("Al 2p_{1/2}-1s Amp", "Al 2p_{1/2}-1s Energy",
                     f->GetParName(4));
  fs[2]->SetParNames("Al 2p_{3/2}-1s Amp", "Al 2p_{3/2}-1s Energy",
                     f->GetParName(4));
  fs[3]->SetParNames(f->GetParName(5), f->GetParName(6),
                     f->GetParName(4));
  return fs;
}

TLegend* BuildInsetLegend(const TH1* h, const TF1* f, const vector<TF1*> fs) {
  TLegend* l = new TLegend(0.68, 0.50, 0.95, 0.90);
  l->AddEntry(h, "Data",                          "p");
  l->AddEntry(f, "Model",                         "l");
  l->AddEntry(fs[0], "Linear b.g.",               "l");
  l->AddEntry(fs[1], "Al 2p_{1/2}-1s",            "l");
  l->AddEntry(fs[2], "Al 2p_{3/2}-1s",            "l");
  l->AddEntry((TObject*)nullptr, "", "");
  l->AddEntry(fs[3], "#muPb#rightarrow^{207}Tl", "l");
  return l;
}

void SetOutsetHistStyle(TH1* h) {
  h->SetLineColor(kBlack);
  h->GetXaxis()->SetRangeUser(200., 1200.);
  h->GetYaxis()->SetRangeUser(0., 45e3);
  h->SetStats(false);
  h->SetTitle(";Energy [keV];Count / keV");
  h->GetXaxis()->SetNdivisions(7, 4, 0, true);
  h->GetYaxis()->SetNdivisions(7, 4, 4, true);
  h->GetXaxis()->SetMaxDigits(4);
  h->GetYaxis()->SetMaxDigits(3);
}

void SetInsetHistStyle(TH1* h) {
  h->GetXaxis()->SetRangeUser(335., 355.);
  h->GetXaxis()->SetMaxDigits(3);
  h->GetYaxis()->SetMaxDigits(3);
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->SetMarkerStyle(kCircle);
  h->GetXaxis()->SetTitleOffset(0.95);
  h->GetYaxis()->SetTitleOffset(0.95);
  h->GetXaxis()->SetTitleSize(0.05);
  h->GetYaxis()->SetTitleSize(0.05);
}

void SetFitFcnStyle(TF1* f) {
  f->SetLineColor(kBlack);
  f->SetLineWidth(2);
  f->SetLineStyle(1);
}

void al50_norm() {
  TFile* f = new TFile("~jrquirk/data/R15b/Al50/nmu_10uspp/nmu.pp10us.root");
  TH1* h = (TH1*)f->Get("hgehi_e_all");
  // Rebin_keVtoMeV(h);
  h->Scale(1./h->GetBinWidth(1)); // Counts/MeV

  TLatex*         lab_dataset = BuildDatasetLabel();
  vector<TLatex*> lab_peaks   = BuildPeakLabels();
  TLatex*         lab_nstop   = BuildNStopLabel();
  TLatex*         lab_chi2    = BuildChi2Label();
  TGraph*         gr_peaks    = BuildPeakMarkers();
  TF1*            ffit        = FitGeSpectrum(h);
  vector<TF1*>    vffits      = BuildDisjointFcns(ffit);
  TLegend*        leg         = BuildInsetLegend(h, ffit, vffits);

  TCanvas* c = new TCanvas("c", "c", 700, 500);
  c->SetTicks();
  SetOutsetHistStyle(h);
  h->DrawCopy();
  lab_dataset->Draw();
  for (int i = 0; i < lab_peaks.size(); ++i) lab_peaks[i]->Draw();
  gr_peaks->Draw("P");
  SetInsetHistStyle(h);
  SetFitFcnStyle(ffit);
  TPad*    p = new TPad("p", "p", 0.40, 0.36, 0.907, 0.92, kWhite, -1, 0);
  p->SetTicks();
  //  p->SetMargin(0.10, 0.011, 0.10, 0.075);
  p->SetMargin(0.10, 0.011, 0.10, 0.04);
  p->SetFillStyle(0);
  p->Draw();
  p->cd();
  h->Draw();
  ffit->Draw("L SAME");
  for (int i = 0; i < vffits.size(); ++i) vffits[i]->Draw("L SAME");
  lab_nstop->Draw();
  // lab_chi2->Draw();
  leg->Draw();

  //  c->SaveAs("img/cppaper/al50_norm.eps");
}
