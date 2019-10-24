
#include "TCanvas.h"
#include "TGraph.h"
#include "TF1.h"
#include "TH1.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TTree.h"

#include <cmath>
#include <cstdio>
#include <utility>
using std::printf;
using std::sprintf;

namespace {
  const char* FPCUTFORMULA = "(x<=[0])*[1]*x^[2]+([0]<x)*[1]*[0]^([2]-[3])*x^[3]";
  // const char* FPCUTFORMULA = "[0]*x^[1]";
  TF1* fpcut[2][2] = { // [l/r][lo/hi]
    { new TF1("fprot_l_delo", FPCUTFORMULA),
      new TF1("fprot_l_dehi", FPCUTFORMULA) },
    { new TF1("fprot_r_delo", FPCUTFORMULA),
      new TF1("fprot_r_dehi", FPCUTFORMULA) },
  };
  const double WIDTH[20] = {
    300., 250., 250., 200., 200., 200., 150., 150., 150.,
    100., 100., 100., 100., 100., 100., 100., 100., 100.,
    100., 100.
  };
};

void init_pcut_fcns() {
  // fpcut[0][0]->SetParameters(4e3, 35e5, -1.02, -0.85);
  // fpcut[0][1]->SetParameters(4e3, 44e5, -1.00, -0.78);
  // fpcut[1][0]->SetParameters(4e3, 60e5, -1.09, -0.95);
  // fpcut[1][1]->SetParameters(4e3, 11e6, -1.10, -0.85);
  fpcut[0][0]->SetParameters(4e3, 35e6, -1.10, -0.80);
  fpcut[0][1]->SetParameters(4e3, 44e6, -1.10, -0.80);
  fpcut[1][0]->SetParameters(4e3, 60e5, -1.10, -0.80);
  fpcut[1][1]->SetParameters(4e3, 11e6, -1.10, -0.80);
  fpcut[0][0]->SetParNames("E_{cut}", "A", "#alpha_{1}", "#alpha_{2}");
  fpcut[0][1]->SetParNames("E_{cut}", "A", "#alpha_{1}", "#alpha_{2}");
  fpcut[1][0]->SetParNames("E_{cut}", "A", "#alpha_{1}", "#alpha_{2}");
  fpcut[1][1]->SetParNames("E_{cut}", "A", "#alpha_{1}", "#alpha_{2}");
  // fpcut[0][0]->FixParameter(0, 4e3);
  // fpcut[0][1]->FixParameter(0, 4e3);
  // fpcut[1][0]->FixParameter(0, 4e3);
  // fpcut[1][1]->FixParameter(0, 4e3);
}

std::pair<double,double> eff_corr(TF1* f, double elo, double ehi) {
  return std::make_pair(f->Integral(elo, ehi), f->IntegralError(elo, ehi));
}


void r15b_al50_psel_optimize_pidcut() {
  init_pcut_fcns();
  TFile* ifile = new TFile("data/Al50/psel.3layer.pidpow2.root");
  TTree* trp[2] = { (TTree*)ifile->Get("PID_LP"),
                    (TTree*)ifile->Get("PID_RP") };
  TTree* trd[2] = { (TTree*)ifile->Get("PID_LD"),
                    (TTree*)ifile->Get("PID_RD") };
  TTree* trt[2] = { (TTree*)ifile->Get("PID_LT"),
                    (TTree*)ifile->Get("PID_RT") };
  TTree* tra[2] = { (TTree*)ifile->Get("PID_LA"),
                    (TTree*)ifile->Get("PID_RA") };
  TTree* tru[2] = { (TTree*)ifile->Get("PID_LU"),
                    (TTree*)ifile->Get("PID_RU") };

  TF1* f[2][20];
  TH1* h[2][20];
  Double_t decut[2][2][20]; //[l/r][lo/hi][point]
  TFitResultPtr res[2][20];
  int n0[2][20], n[2][20];
  double eff[2][20], err[2][20];
  for (int i = 0; i < 20; ++i) {
    char hlname[64], hrname[64], hltitle[128], hrtitle[128];
    sprintf(hlname, "hl%d", 2000+i*500);
    sprintf(hrname, "hr%d", 2000+i*500);
    sprintf(hltitle, "dE Spread of %dkeV<E<%dkeV Slice;dE [keV];Counts/5 keV", 2000+i*500, 2500+i*500);
    sprintf(hrtitle, "dE Spread of %dkeV<E<%dkeV Slice;dE [keV];Counts/5 keV", 2000+i*500, 2500+i*500);
    h[0][i] = new TH1D(hlname, hltitle, 300, 0, 3000);
    h[1][i] = new TH1D(hrname, hrtitle, 300, 0, 3000);
    f[0][i] = new TF1("", "gaus(0)");
    f[1][i] = new TF1("", "gaus(0)");
    h[0][i]->SetLineColor(kBlue-8);
    h[1][i]->SetLineColor(kRed-8);
    f[0][i]->SetLineColor(kBlue);
    f[1][i]->SetLineColor(kRed);

    char cmd[64], cut[64];
    sprintf(cut, "%d<e && e<%d && e3==0", 2000+500*i, 2500+500*i);
    sprintf(cmd, "de>>+%s", hlname);
    trp[0]->Draw(cmd, cut, "goff");
    // trd[0]->Draw(cmd, cut, "goff");
    // tru[0]->Draw(cmd, cut, "goff");
    sprintf(cmd, "de>>+%s", hrname);
    trp[1]->Draw(cmd, cut, "goff");
    // trd[1]->Draw(cmd, cut, "goff");
    // tru[1]->Draw(cmd, cut, "goff");

    f[0][i]->SetParameters(h[0][i]->GetMaximum(),
                           h[0][i]->GetBinCenter(h[0][i]->GetMaximumBin()),
                           50., 0);
    f[1][i]->SetParameters(h[1][i]->GetMaximum(),
                           h[1][i]->GetBinCenter(h[1][i]->GetMaximumBin()),
                           50., 0);
    f[0][i]->SetRange(h[0][i]->GetBinCenter(h[0][i]->GetMaximumBin())-WIDTH[i],
                      h[0][i]->GetBinCenter(h[0][i]->GetMaximumBin())+WIDTH[i]);
    f[1][i]->SetRange(h[1][i]->GetBinCenter(h[1][i]->GetMaximumBin())-WIDTH[i],
                      h[1][i]->GetBinCenter(h[1][i]->GetMaximumBin())+WIDTH[i]);
    // f[0][i]->SetRange(h[0][i]->GetMean()-100, h[0][i]->GetMean()+100);
    // f[1][i]->SetRange(h[1][i]->GetMean()-100, h[1][i]->GetMean()+100);
    res[0][i] = h[0][i]->Fit(f[0][i], "SMELRQ");
    res[1][i] = h[1][i]->Fit(f[1][i], "SMELRQ");
    h[0][i]->GetXaxis()->SetRangeUser(res[0][i]->Parameter(1)-5*res[0][i]->Parameter(2),
                                      res[0][i]->Parameter(1)+5*res[0][i]->Parameter(2));
    h[1][i]->GetXaxis()->SetRangeUser(res[1][i]->Parameter(1)-5*res[0][i]->Parameter(2),
                                      res[1][i]->Parameter(1)+5*res[0][i]->Parameter(2));
    decut[0][0][i] = res[0][i]->Parameter(1)-2*res[0][i]->Parameter(2);
    decut[0][1][i] = res[0][i]->Parameter(1)+2*res[0][i]->Parameter(2);
    decut[1][0][i] = res[1][i]->Parameter(1)-2*res[1][i]->Parameter(2);
    decut[1][1][i] = res[1][i]->Parameter(1)+2*res[1][i]->Parameter(2);
  }

  Double_t x[20];
  for (int i = 0; i < 20; ++i)
    x[i] = 2250+i*500;
  TGraph* gde[2][2] = {
    { new TGraph(20, x, decut[0][0]), new TGraph(20, x, decut[0][1]) },
    { new TGraph(20, x, decut[1][0]), new TGraph(20, x, decut[1][1]) }
  };
  fpcut[0][0]->SetRange(2e3, 12e3);
  fpcut[0][1]->SetRange(2e3, 12e3);
  fpcut[1][0]->SetRange(2e3, 12e3);
  fpcut[1][1]->SetRange(2e3, 12e3);
  gde[0][0]->Fit(fpcut[0][0], "MES");
  gde[0][1]->Fit(fpcut[0][1], "MES");
  gde[1][0]->Fit(fpcut[1][0], "MES");
  gde[1][1]->Fit(fpcut[1][1], "MES");
  gde[0][0]->SetLineColor(kGreen);
  gde[0][1]->SetLineColor(kGreen);
  gde[1][0]->SetLineColor(kGreen);
  gde[1][1]->SetLineColor(kGreen);
  gde[0][0]->SetLineWidth(4);
  gde[0][1]->SetLineWidth(4);
  gde[1][0]->SetLineWidth(4);
  gde[1][1]->SetLineWidth(4);

  TCanvas* cdeve = new TCanvas("cdeve", "cdeve", 1400, 500);
  cdeve->Divide(2);
  cdeve->cd(1);
  trp[0]->Draw("de:e", "e3==0");
  trd[0]->Draw("de:e", "e3==0", "SAME");
  trt[0]->Draw("de:e", "e3==0", "SAME");
  tra[0]->Draw("de:e", "e3==0", "SAME");
  tru[0]->Draw("de:e", "e3==0", "SAME");
  gde[0][0]->Draw("C");
  gde[0][1]->Draw("C");
  cdeve->cd(2);
  trp[1]->Draw("de:e", "e3==0");
  trd[1]->Draw("de:e", "e3==0", "SAME");
  trt[1]->Draw("de:e", "e3==0", "SAME");
  tra[1]->Draw("de:e", "e3==0", "SAME");
  tru[1]->Draw("de:e", "e3==0", "SAME");
  gde[1][0]->Draw("C");
  gde[1][1]->Draw("C");
  cdeve->SaveAs("img/optimize_pidcut_dEvE.png");

  TCanvas* c1 = new TCanvas("c1", "c1", 2100, 1500);
  c1->Divide(3, 3);
  c1->cd(1); h[0][0]->Draw("SAME"); h[1][0]->Draw("SAME");
  c1->cd(2); h[0][1]->Draw("SAME"); h[1][1]->Draw("SAME");
  c1->cd(3); h[0][2]->Draw("SAME"); h[1][2]->Draw("SAME");
  c1->cd(4); h[0][3]->Draw("SAME"); h[1][3]->Draw("SAME");
  c1->cd(5); h[0][4]->Draw("SAME"); h[1][4]->Draw("SAME");
  c1->cd(6); h[0][5]->Draw("SAME"); h[1][5]->Draw("SAME");
  c1->cd(7); h[0][6]->Draw("SAME"); h[1][6]->Draw("SAME");
  c1->cd(8); h[0][7]->Draw("SAME"); h[1][7]->Draw("SAME");
  c1->cd(9); h[0][8]->Draw("SAME"); h[1][8]->Draw("SAME");
  TCanvas* c2 = new TCanvas("c2", "c2", 2100, 1500);
  c2->Divide(3, 3);
  c2->cd(1); h[0][9] ->Draw("SAME"); h[1][9] ->Draw("SAME");
  c2->cd(2); h[0][10]->Draw("SAME"); h[1][10]->Draw("SAME");
  c2->cd(3); h[0][11]->Draw("SAME"); h[1][11]->Draw("SAME");
  c2->cd(4); h[0][12]->Draw("SAME"); h[1][12]->Draw("SAME");
  c2->cd(5); h[0][13]->Draw("SAME"); h[1][13]->Draw("SAME");
  c2->cd(6); h[0][14]->Draw("SAME"); h[1][14]->Draw("SAME");
  c2->cd(7); h[0][15]->Draw("SAME"); h[1][15]->Draw("SAME");
  c2->cd(8); h[0][16]->Draw("SAME"); h[1][16]->Draw("SAME");
  c2->cd(9); h[0][17]->Draw("SAME"); h[1][17]->Draw("SAME");
  TCanvas* c3 = new TCanvas("c3", "c3", 1400, 500);
  c3->Divide(2);
  c3->cd(1); h[0][18]->Draw("SAME"); h[1][18]->Draw("SAME");
  c3->cd(2); h[0][19]->Draw("SAME"); h[1][19]->Draw("SAME");
  c1->SaveAs("img/optimize_pidcut_dE_1.png");
  c2->SaveAs("img/optimize_pidcut_dE_2.png");
  c3->SaveAs("img/optimize_pidcut_dE_3.png");



  // printf("Right Chi2red: %g/%d\n", (double)res[1]->Chi2(), (int)res[1]->Ndf());
}
