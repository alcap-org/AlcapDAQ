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
  double XRAYEFF          = 2.6e-4;
  double XRAYEFFERR       = 0.1e-4;
  double XRAYINTENSITY    = 0.752;//AE: changed to literature value. Was 0.90;
  double XRAYINTENSITYERR = 0.04;
  double ELIMFIT[2]       = { 923., 951. };
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
                   "pol1(0)+gaus(2)+2*[2]*exp(-0.5*(x-([3]+2.2))^2/[4]^2)+"
                   "[5]*exp(-0.5*(x-[6])^2/[4]^2)",
                   ELIMFIT[0], ELIMFIT[1]);
  f->SetParNames("Bkg y-int", "Bkg Slope",
                 "Ti 2p_{1/2}-1s Amp.", "Ti 2p_{1/2}-1s Energy",
                 "Resolution (#sigma)", "Pb #gamma Amp.", "Pb #gamma Energy");
  f->SetParameters(1.0e3, 0., 4.0e3, 929., 1.7, 2.0e3, 937.);
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
  TLatex* txt = new TLatex(500., 17.0e3, "Ti 50#mum");
  txt->SetTextSize(0.06);
  return txt;
}

TLatex* BuildChi2Label() {
  TLatex* txt = new TLatex(940., 1.5e3, "#splitline{#chi^{2}/ndf}{=181/145}");
  txt->SetTextSize(0.07);
  return txt;
}

TLatex* BuildNStopLabel() {
  TLatex* txt =
    //  new TLatex(932., 4.2e3, "#splitline{78(3)#times10^{6}}{#mu-stops}");
      new TLatex(932., 4.2e3, "#splitline{96(3)#times10^{6}}{#mu-stops}");
  txt->SetTextSize(0.07);
  txt->SetTextAlign(11);
  return txt;
}

vector<TLatex*> BuildPeakLabels() {
  vector<TLatex*> labs;
  labs.push_back(new TLatex(435.,  11.2e3, "#muPb 5g_{9/2}-4f_{7/2}"));
  labs.push_back(new TLatex(470.,  9.0e3,  "#muPb 5g_{7/2}-4f_{5/2}"));
  labs.push_back(new TLatex(515.,  12.0e3, "511 keV"));
  labs.push_back(new TLatex(720.,  3.0e3,  "#muPb #gamma"));
  labs.push_back(new TLatex(810.,  2.6e3,  "#muTi#rightarrow^{47}Sc"));
  labs.push_back(new TLatex(915.,  1.5e3,  "#muTi 2p-1s"));
  labs.push_back(new TLatex(960.,  1.5e3,  "#muPb 4f_{7/2}-3d_{5/2}"));
  labs.push_back(new TLatex(1010., 1.5e3,  "#muPb 4f_{5/2}-3d_{3/2}"));
  labs.push_back(new TLatex(1122., 1.5e3,  "#muTi 3p-1s"));
  for (int i = 0; i < labs.size(); ++i) {
    labs[i]->SetTextAngle(90.);
    labs[i]->SetTextSize (0.033);
  }
  return labs;
}

TGraph* BuildPeakMarkers() {
  TGraph* g = new TGraph();
  int i = -1;
  g->SetPoint(++i, 430.,  10.7e3);
  g->SetPoint(++i, 439.,  9.5e3);
  g->SetPoint(++i, 511.,  11.5e3);
  g->SetPoint(++i, 719.,  2.4e3);
  g->SetPoint(++i, 807.,  2.2e3);
  g->SetPoint(++i, 930.,  5.05e3);
  g->SetPoint(++i, 937.8, 4.3e3);
  g->SetPoint(++i, 971.7, 3.3e3);
  g->SetPoint(++i, 1122., 1.e3);
  g->SetMarkerColor(kBlack);
  g->SetMarkerStyle(kFullTriangleDown);
  return g;
}

vector<TF1*> BuildDisjointFcns(const TF1* f) {
  double dE = 2.2; // Fine splitting energy
  double mA = 2.;  // Fine splitting relative populations
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
  fs[2]->SetParameters(mA*f->GetParameter(2), dE+f->GetParameter(3),
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
  TLegend* l = new TLegend(0.68, 0.48, 0.95, 0.90);
  l->AddEntry(h, "Data",                     "p");
  l->AddEntry(f, "Model",                    "l");
  l->AddEntry(fs[0], "Linear b.g.",          "l");
  l->AddEntry(fs[1], "Ti 2p_{1/2}-1s",       "l");
  l->AddEntry(fs[2], "Ti 2p_{3/2}-1s",       "l");
  // l->AddEntry((TObject*)nullptr, "", "");
  l->AddEntry(fs[3], "Pb 4f_{7/2}-3d_{5/2}", "l");
  return l;
}

void SetOutsetHistStyle(TH1* h) {
  h->SetLineColor(kBlack);
  h->GetXaxis()->SetRangeUser(390., 1200.);
  h->GetYaxis()->SetRangeUser(0., 20e3);
  h->SetStats(false);
  h->SetTitle(";Energy [keV];Count / keV");
  h->GetXaxis()->SetNdivisions(7, 4, 0, true);
  h->GetYaxis()->SetNdivisions(7, 4, 4, true);
  //  h->GetYaxis()->SetTitleOffset(1.35);
  h->GetXaxis()->SetMaxDigits(4);
  h->GetYaxis()->SetMaxDigits(3);

}

void SetInsetHistStyle(TH1* h) {
  h->GetXaxis()->SetRangeUser(ELIMFIT[0], ELIMFIT[1]);
  h->GetYaxis()->SetRangeUser(0., 5.0e3);
  h->GetXaxis()->SetMaxDigits(3);
  h->GetYaxis()->SetMaxDigits(3);
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->SetMarkerStyle(kCircle);
  h->GetYaxis()->SetTitleOffset(1.55);
  h->GetXaxis()->SetTitleSize(0.05);
  h->GetYaxis()->SetTitleSize(0.05);
}

void SetFitFcnStyle(TF1* f) {
  f->SetLineColor(kBlack);
  f->SetLineWidth(2);
  f->SetLineStyle(1);
}

void ti50_norm() {
  TFile* f = new TFile("~jrquirk/data/R15b/Ti50/nmu_10uspp/nmu.pp10us.root");
  TH1* h = (TH1*)f->Get("hgehi_e_all");
  // Rebin_keVtoMeV(h);
  h->Scale(1./h->GetBinWidth(1)); // Counts/keV

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
  for (int i = 0; i < lab_peaks.size(); ++i) lab_peaks[i]->Draw();
  gr_peaks->Draw("P");
  lab_dataset->Draw();
  SetInsetHistStyle(h);
  SetFitFcnStyle(ffit);
  TPad*    p = new TPad("p", "p", 0.40, 0.36, 0.907, 0.92, kWhite, -1, 0);
  p->SetTicks();
  //  p->SetMargin(0.10, 0.05, 0.11, 0.075);
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

  //  c->SaveAs("img/cppaper/ti50_norm.eps");
}
