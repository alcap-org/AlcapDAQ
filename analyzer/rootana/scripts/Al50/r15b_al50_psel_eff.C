#include "./scripts/Al50/util.h"

#include "TCanvas.h"
#include "TF1.h"
#include "TH1.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TLine.h"
#include "TMultiGraph.h"
#include "TStyle.h"
#include "TTree.h"

#include <cmath>
#include <cstdio>
#include <utility>
using std::printf;
using std::sprintf;

// Double_t langaufun(Double_t *x, Double_t *par)
// // Copied from ROOT tuitorial script.
// /*
//    Fit parameters:
//    par[0]=Width (scale) parameter of Landau density
//    par[1]=Most Probable (MP, location) parameter of Landau density
//    par[2]=Total area (integral -inf to inf, normalization constant)
//    par[3]=Width (sigma) of convoluted Gaussian function
//    par[4]=Constant background

//    In the Landau distribution (represented by the CERLIB approximation),
//    the maximum is located at x=-0.22278298 with the location parameter=0.
//    This shift is corrected within this function, so that the actual
//    maximum is identical to the MP parameter.
// */

// {
//       // Numeric constants
//       Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
//       Double_t mpshift  = -0.22278298;       // Landau maximum location

//       // Control constants
//       Double_t np = 400.0;      // number of convolution steps (originally 100)
//       Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas

//       // Variables
//       Double_t xx;
//       Double_t mpc;
//       Double_t fland;
//       Double_t sum = 0.0;
//       Double_t xlow,xupp;
//       Double_t step;
//       Double_t i;


//       // MP shift correction
//       mpc = par[1] - mpshift * par[0];

//       // Range of convolution integral
//       xlow = x[0] - sc * par[3];
//       xupp = x[0] + sc * par[3];

//       step = (xupp-xlow) / np;

//       // Convolution integral of Landau and Gaussian by sum
//       for(i=1.0; i<=np/2; i++)
//       {
//          xx = xlow + (i-.5) * step;
//          fland = TMath::Landau(xx,mpc,par[0]) / par[0];
//          sum += fland * TMath::Gaus(x[0],xx,par[3]);

//          xx = xupp - (i-.5) * step;
//          fland = TMath::Landau(xx,mpc,par[0]) / par[0];
//          sum += fland * TMath::Gaus(x[0],xx,par[3]);
//       }

//       return (par[2] * step * sum * invsq2pi / par[3]);

// }

namespace {
  const char* FPCUTFORMULA = "(x<=[0])*[1]*x^[2]+([0]<x)*[1]*[0]^([2]-[3])*x^[3]";
  TF1* fpcut[2][2] = { // [l/r][lo/hi]
    { new TF1("fprot_l_delo", FPCUTFORMULA),
      new TF1("fprot_l_dehi", FPCUTFORMULA) },
    { new TF1("fprot_r_delo", FPCUTFORMULA),
      new TF1("fprot_r_dehi", FPCUTFORMULA) },
  };
};

void init_pcut_fcns() {
  fpcut[0][0]->SetParameters(4e3, 35e5, -1.02, -0.85);
  fpcut[0][1]->SetParameters(4e3, 44e5, -1.00, -0.78);
  fpcut[1][0]->SetParameters(4e3, 60e5, -1.09, -0.95);
  fpcut[1][1]->SetParameters(4e3, 11e6, -1.10, -0.85);
}

std::pair<double,double> eff_corr(TF1* f, double elo, double ehi) {
  return std::make_pair(f->Integral(elo, ehi), f->IntegralError(elo, ehi));
}

void SaveCorrectionsToFile(const char* ofname, const double eff[2][20], const double err[2][20]) {
  TDirectory* cwd = gDirectory;
  TFile* ofile = new TFile(ofname, "RECREATE");
  TH1* hl = new TH1D("hlpidcorrerr", "SiL PID Proton Correction and Error", 20, 2e3, 12e3);
  TH1* hr = new TH1D("hrpidcorrerr", "SiR PID Proton Correction and Error", 20, 2e3, 12e3);
  for (int i = 0; i < 20; ++i) {
    hl->SetBinContent(i+1, eff[0][i]);
    hl->SetBinError  (i+1, err[0][i]);
    hr->SetBinContent(i+1, eff[1][i]);
    hr->SetBinError  (i+1, err[1][i]);
  }
  ofile->Write();
  ofile->Close();
  delete ofile;
  cwd->cd();
}

void r15b_al50_psel_eff() {
  init_pcut_fcns();
  TFile* ifile = new TFile("data/Al50/psel.3layer.pidpow2.root");
  TTree* trp[2] = { (TTree*)ifile->Get("PID_LP"),
                    (TTree*)ifile->Get("PID_RP") };

  TF1* f[2][20];
  TH1* h[2][20];
  TFitResultPtr res[2][20];
  int n0[2][20], n[2][20];
  double eff[2][20], err[2][20], errn[2][20];
  TLine* l[2][20][2]; // [lr][eslice][decut]
  TLegend* leg[20];
  for (int i = 0; i < 20; ++i) {
    char hlname[64], hrname[64], hltitle[128], hrtitle[128];
    sprintf(hlname, "hl%d", 2000+i*500);
    sprintf(hrname, "hr%d", 2000+i*500);
    sprintf(hltitle, "dE Spread of %dkeV<E<%dkeV Slice;dE [keV];Counts/5 keV", 2000+i*500, 2500+i*500);
    sprintf(hrtitle, "dE Spread of %dkeV<E<%dkeV Slice;dE [keV];Counts/5 keV", 2000+i*500, 2500+i*500);
    h[0][i] = new TH1D(hlname, hltitle, 600, 0, 3000);
    h[1][i] = new TH1D(hrname, hrtitle, 600, 0, 3000);
    f[0][i] = new TF1("", "gaus(0)");
    f[1][i] = new TF1("", "gaus(0)");
    f[0][i]->SetNpx(f[0][i]->GetNpx()*100);
    f[1][i]->SetNpx(f[1][i]->GetNpx()*100);
    h[0][i]->SetLineColor(ACStyle::kACBlue);
    h[1][i]->SetLineColor(ACStyle::kACRed);
    f[0][i]->SetLineColor(kBlue);
    f[1][i]->SetLineColor(kRed);

    double elim[2][2] = { // [l/r][lo/hi]
      { fpcut[0][0]->Eval(2250+i*500), fpcut[0][1]->Eval(2250+i*500) },
      { fpcut[1][0]->Eval(2250+i*500), fpcut[1][1]->Eval(2250+i*500) },
    };

    char cmd[64], cut[64];
    sprintf(cut, "%d<e && e<%d && e3==0", 2000+500*i, 2500+500*i);
    sprintf(cmd, "de>>+%s", hlname);
    trp[0]->Draw(cmd, cut, "goff");
    sprintf(cmd, "de>>+%s", hrname);
    trp[1]->Draw(cmd, cut, "goff");

    l[0][i][0] = new TLine(elim[0][0], 0, elim[0][0], h[0][i]->GetMaximum());
    l[0][i][1] = new TLine(elim[0][1], 0, elim[0][1], h[0][i]->GetMaximum());
    l[1][i][0] = new TLine(elim[1][0], 0, elim[1][0], h[1][i]->GetMaximum());
    l[1][i][1] = new TLine(elim[1][1], 0, elim[1][1], h[1][i]->GetMaximum());
    l[0][i][0]->SetLineColor(kBlue);
    l[0][i][1]->SetLineColor(kBlue);
    l[1][i][0]->SetLineColor(kRed);
    l[1][i][1]->SetLineColor(kRed);

    f[0][i]->SetParameters(h[0][i]->GetMaximum(),
                           h[0][i]->GetBinCenter(h[0][i]->GetMaximumBin()),
                           50., 1);
    f[1][i]->SetParameters(h[1][i]->GetMaximum(),
                           h[1][i]->GetBinCenter(h[1][i]->GetMaximumBin()),
                           50., 1);
    f[0][i]->SetParLimits(3, 0, 10);
    f[1][i]->SetParLimits(3, 0, 10);
    f[0][i]->SetRange(h[0][i]->GetMean()-400, h[0][i]->GetMean()+400);
    f[1][i]->SetRange(h[1][i]->GetMean()-400, h[1][i]->GetMean()+400);
    res[0][i] = h[0][i]->Fit(f[0][i], "SMELR");
    eff[0][i] = f[0][i]->Integral     (elim[0][0], elim[0][1]) / f[0][i]->Integral(-10e3,10e3);
    // err[0][i] = f[0][i]->IntegralError(elim[0][0], elim[0][1]) / f[0][i]->Integral(elim[0][0], elim[0][1]);
    err[0][i] = f[0][i]->GetParError(2)/f[0][i]->GetParameter(2);
    errn[0][i] = f[0][i]->IntegralError(elim[0][0], elim[0][1]);
    res[1][i] = h[1][i]->Fit(f[1][i], "SMELR");
    eff[1][i] = f[1][i]->Integral     (elim[1][0], elim[1][1]) / f[1][i]->Integral(-10e3,10e3);
    // err[1][i] = f[1][i]->IntegralError(elim[1][0], elim[1][1]) / f[1][i]->Integral(elim[1][0], elim[1][1]);
    err[1][i] = f[1][i]->GetParError(2)/f[1][i]->GetParameter(2);
    errn[1][i] = f[1][i]->IntegralError(elim[1][0], elim[1][1]);
    printf("L,R %d-%d (%f):\t%f-%f,\t%f-%f\n", 2000+i*500, 2500+i*500, 2250.+i*500., elim[0][0], elim[0][1], elim[1][0], elim[1][1]);

    h[0][i]->GetXaxis()->SetRangeUser(res[0][i]->Parameter(1)-5*res[0][i]->Parameter(2),
                                      res[0][i]->Parameter(1)+5*res[0][i]->Parameter(2));
    h[1][i]->GetXaxis()->SetRangeUser(res[1][i]->Parameter(1)-5*res[0][i]->Parameter(2),
                                      res[1][i]->Parameter(1)+5*res[0][i]->Parameter(2));

    n0[0][i] = h[0][i]->GetEntries();
    n0[1][i] = h[1][i]->GetEntries();
    n [0][i] = n0[0][i] / eff[0][i];
    n [1][i] = n0[1][i] / eff[1][i];

    char legentryl[64], legentryr[64];
    sprintf(legentryl, "SiL (%.2f%%)", 100.*eff[0][i]);
    sprintf(legentryr, "SiR (%.2f%%)", 100.*eff[1][i]);
    leg[i] = new TLegend(0.7, 0.7, 0.9, 0.9);
    leg[i]->SetHeader("Efficiency");
    leg[i]->AddEntry(h[0][i], legentryl);
    leg[i]->AddEntry(h[1][i], legentryr);
  }

  gStyle->SetOptStat(0);

  TCanvas* c1 = new TCanvas("c1", "c1", 2100, 1500);
  c1->Divide(3, 3);
  c1->cd(1); h[0][0]->Draw("SAME"); h[1][0]->Draw("SAME"); leg[0]->Draw(); // l[0][0][0]->Draw(); l[0][0][1]->Draw(); l[1][0][0]->Draw(); l[1][0][1]->Draw();
  c1->cd(2); h[0][1]->Draw("SAME"); h[1][1]->Draw("SAME"); leg[1]->Draw(); // l[0][1][0]->Draw(); l[0][1][1]->Draw(); l[1][1][0]->Draw(); l[1][1][1]->Draw();
  c1->cd(3); h[0][2]->Draw("SAME"); h[1][2]->Draw("SAME"); leg[2]->Draw(); // l[0][2][0]->Draw(); l[0][2][1]->Draw(); l[1][2][0]->Draw(); l[1][2][1]->Draw();
  c1->cd(4); h[0][3]->Draw("SAME"); h[1][3]->Draw("SAME"); leg[3]->Draw(); // l[0][3][0]->Draw(); l[0][3][1]->Draw(); l[1][3][0]->Draw(); l[1][3][1]->Draw();
  c1->cd(5); h[0][4]->Draw("SAME"); h[1][4]->Draw("SAME"); leg[4]->Draw(); // l[0][4][0]->Draw(); l[0][4][1]->Draw(); l[1][4][0]->Draw(); l[1][4][1]->Draw();
  c1->cd(6); h[0][5]->Draw("SAME"); h[1][5]->Draw("SAME"); leg[5]->Draw(); // l[0][5][0]->Draw(); l[0][5][1]->Draw(); l[1][5][0]->Draw(); l[1][5][1]->Draw();
  c1->cd(7); h[0][6]->Draw("SAME"); h[1][6]->Draw("SAME"); leg[6]->Draw(); // l[0][6][0]->Draw(); l[0][6][1]->Draw(); l[1][6][0]->Draw(); l[1][6][1]->Draw();
  c1->cd(8); h[0][7]->Draw("SAME"); h[1][7]->Draw("SAME"); leg[7]->Draw(); // l[0][7][0]->Draw(); l[0][7][1]->Draw(); l[1][7][0]->Draw(); l[1][7][1]->Draw();
  c1->cd(9); h[0][8]->Draw("SAME"); h[1][8]->Draw("SAME"); leg[8]->Draw(); // l[0][8][0]->Draw(); l[0][8][1]->Draw(); l[1][8][0]->Draw(); l[1][8][1]->Draw();
  TCanvas* c2 = new TCanvas("c2", "c2", 2100, 1500);
  c2->Divide(3, 3);
  c2->cd(1); h[0][9] ->Draw("SAME"); h[1][9] ->Draw("SAME"); leg[9] ->Draw(); // l[0][9] [0]->Draw(); l[0][9] [1]->Draw(); l[1][9] [0]->Draw(); l[1][9] [1]->Draw();
  c2->cd(2); h[0][10]->Draw("SAME"); h[1][10]->Draw("SAME"); leg[10]->Draw(); // l[0][10][0]->Draw(); l[0][10][1]->Draw(); l[1][10][0]->Draw(); l[1][10][1]->Draw();
  c2->cd(3); h[0][11]->Draw("SAME"); h[1][11]->Draw("SAME"); leg[11]->Draw(); // l[0][11][0]->Draw(); l[0][11][1]->Draw(); l[1][11][0]->Draw(); l[1][11][1]->Draw();
  c2->cd(4); h[0][12]->Draw("SAME"); h[1][12]->Draw("SAME"); leg[12]->Draw(); // l[0][12][0]->Draw(); l[0][12][1]->Draw(); l[1][12][0]->Draw(); l[1][12][1]->Draw();
  c2->cd(5); h[0][13]->Draw("SAME"); h[1][13]->Draw("SAME"); leg[13]->Draw(); // l[0][13][0]->Draw(); l[0][13][1]->Draw(); l[1][13][0]->Draw(); l[1][13][1]->Draw();
  c2->cd(6); h[0][14]->Draw("SAME"); h[1][14]->Draw("SAME"); leg[14]->Draw(); // l[0][14][0]->Draw(); l[0][14][1]->Draw(); l[1][14][0]->Draw(); l[1][14][1]->Draw();
  c2->cd(7); h[0][15]->Draw("SAME"); h[1][15]->Draw("SAME"); leg[15]->Draw(); // l[0][15][0]->Draw(); l[0][15][1]->Draw(); l[1][15][0]->Draw(); l[1][15][1]->Draw();
  c2->cd(8); h[0][16]->Draw("SAME"); h[1][16]->Draw("SAME"); leg[16]->Draw(); // l[0][16][0]->Draw(); l[0][16][1]->Draw(); l[1][16][0]->Draw(); l[1][16][1]->Draw();
  c2->cd(9); h[0][17]->Draw("SAME"); h[1][17]->Draw("SAME"); leg[17]->Draw(); // l[0][17][0]->Draw(); l[0][17][1]->Draw(); l[1][17][0]->Draw(); l[1][17][1]->Draw();
  TCanvas* c3 = new TCanvas("c3", "c3", 1400, 500);
  c3->Divide(2);
  c3->cd(1); h[0][18]->Draw("SAME"); h[1][18]->Draw("SAME"); leg[18]->Draw(); // l[0][18][0]->Draw(); l[0][18][1]->Draw(); l[1][18][0]->Draw(); l[1][18][1]->Draw();
  c3->cd(2); h[0][19]->Draw("SAME"); h[1][19]->Draw("SAME"); leg[19]->Draw(); // l[0][19][0]->Draw(); l[0][19][1]->Draw(); l[1][19][0]->Draw(); l[1][19][1]->Draw();
  c1->SaveAs("img/psel_eff_datadriven_1.png");
  c2->SaveAs("img/psel_eff_datadriven_2.png");
  c3->SaveAs("img/psel_eff_datadriven_3.png");

  Double_t x[20], y[2][20], ey[2][20];
  for (int i = 0; i < 20; ++i) {
    printf("%d-%dkeV:\t%f(%f)\t%f(%f)\n",
           2000+500*i, 2500+500*i, eff[0][i], err[0][i], eff[1][i], err[1][i]);
    x[i]    = 2250+500*i;
  }
  double N0[2] = {0., 0.}, N[2] = {0., 0.};
  double E[2] = {0., 0.}, EN[2] = {0., 0.};
  for (int i = 3; i <= 16; ++i) {
    N0[0] += n0[0][i];
    N[0]  += n[0][i];
    E[0]  += err[0][i]*err[0][i];
    EN[0] += errn[0][i]*errn[0][i];
    N0[1] += n0[1][i];
    N[1]  += n[1][i];
    E[1]  += err[1][i]*err[1][i];
    EN[1] += errn[1][i]*errn[1][i];
  }
  E[0] = std::sqrt(E[0]);
  E[1] = std::sqrt(E[1]);
  EN[0] = std::sqrt(EN[0]);
  EN[1] = std::sqrt(EN[1]);

  printf("Left N0/N: %g/%g (%g) [+-%f%%]\n",  N0[0], N[0], N0[0]/N[0], EN[0]/N[0]/*100*E[0]*/);
  printf("Right N0/N: %g/%g (%g) [+-%f%%]\n", N0[1], N[1], N0[1]/N[1], EN[1]/N[1]/*100*E[1]*/);
  // printf("Left  Chi2red: %g/%d\n", (double)res[0]->Chi2(), (int)res[0]->Ndf());
  // printf("Right Chi2red: %g/%d\n", (double)res[1]->Chi2(), (int)res[1]->Ndf());

  TGraphErrors* gr[2] = { new TGraphErrors(20, x, eff[0], nullptr, err[0]),
                          new TGraphErrors(20, x, eff[1], nullptr, err[1]) };
  gr[0]->SetLineColor(ACStyle::kACBlue);
  gr[1]->SetLineColor(ACStyle::kACRed);
  TMultiGraph* mg = new TMultiGraph("mg", "PID Efficiency;Energy [keV];Efficiency");
  mg->Add(gr[0]);
  mg->Add(gr[1]);
  TLegend* leggr = new TLegend(0.3, 0.2, 0.5, 0.4);
  leggr->AddEntry(gr[0], "SiL");
  leggr->AddEntry(gr[1], "SiR");
  TCanvas* cgr = new TCanvas("cgr", "cgr", 700, 500);
  mg->Draw("AL");
  leggr->Draw();
  cgr->SaveAs("img/psel_eff_datadriven_fcn.png");

  //SaveCorrectionsToFile("data/Al50/corranderr/pid.root", eff, err);
}
