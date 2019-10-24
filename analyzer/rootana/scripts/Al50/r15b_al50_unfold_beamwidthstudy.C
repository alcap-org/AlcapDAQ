// Compare TM from different sections of the target.

#include "util.h"

#include "RooUnfoldBayes.h"
#include "RooUnfoldResponse.h"

#include "TCanvas.h"
#include "TColor.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2.h"
#include "TLegend.h"
#include "TLine.h"
#include "TMultiGraph.h"
#include "TStyle.h"
#include "TTree.h"

#include <cmath>
#include <cstdio>
#include <utility>

void PrintLine(double x,
               const std::pair<double,double> l,
               const std::pair<double,double> r,
               const std::pair<double,double> a) {
  printf("%2gum:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n",
         x, l.first, l.second, r.first, r.second, a.first, a.second);
}

std::pair<double,double> AvgAndStd(const TGraph* g) {
  int n         = g->GetN();
  Double_t* ys  = g->GetY();
  Double_t* eys = g->GetEY();
  double y = 0, ey2 = 0;
  for (int i = 0; i < n; ++i) {
    y   += ys[i];
    ey2 += eys[i]*eys[i];
  }
  return std::make_pair(y/n, std::sqrt(ey2)/n);
}

// Check if lines intersect, where sa is the slope of line (x1,y1)(x2,y2) and
// sb the slope of (x3,y3)(x4,y4)
// x = ((y4-y2)+(sax2-sbx4))/(sa-sb)
// if sa==sb, obviously parallel. Solve, then if solution (x, y) is such that
// x1<x<x2 and x3<x<x4, or reverse of either, we're good. For here, x1=x3 and
// x2=x4, so
// x = ((y4-y2)+(sa-sb)x2)/(sa-sb)
bool IntersectionPoint(const TGraph* g1, const TGraph* g2,
                       double& x, double& y) {
  if (g1->GetN() != g2->GetN())
    return false;
  for (int i = 0; i < g1->GetN()-1; ++i) {
    double x1a, x1b, x2a, x2b, y1a, y1b, y2a, y2b;
    g1->GetPoint(i,   x1a, y1a);
    g1->GetPoint(i+1, x1b, y1b);
    g2->GetPoint(i,   x2a, y2a);
    g2->GetPoint(i+1, x2b, y2b);
    if (x1a != x2a || x1b != x2b)
      return false;
    double s1 = (y1b-y1a)/(x1b-x1a), s2 = (y2b-y2a)/(x2b-x2a);
    if (s1 == s2)
      return false;
    x = ((y2b-y1b) + (s1*x1b-s2*x2b))/(s1-s2);
    if ( (x1a<x<x1b || x1b<x<x1a) && (x2a<x<x2b || x2b<x<x2a) ) {
      y = y1a + s1*(x-x1a);
      return true;
    }
  }
  return false;
}

void r15b_al50_unfold_beamwidthstudy() {
  TH1::SetDefaultSumw2(true);
  static const int N = 16;

  TFile* fdata = new TFile( "data/Al50/psel.3layer.root");
  TFile* f[N] = {
    new TFile("data/Al50/beamwidthstudy/tm.p.2layer.pbite0.50.root", "READ"),
    new TFile("data/Al50/beamwidthstudy/tm.p.2layer.pbite0.60.root", "READ"),
    new TFile("data/Al50/beamwidthstudy/tm.p.2layer.pbite0.70.root", "READ"),
    new TFile("data/Al50/beamwidthstudy/tm.p.2layer.pbite0.80.root", "READ"),
    new TFile("data/Al50/beamwidthstudy/tm.p.2layer.pbite0.90.root", "READ"),
    new TFile("data/Al50/tm.p.2layer.root",                          "READ"),
    new TFile("data/Al50/beamwidthstudy/tm.p.2layer.pbite1.10.root", "READ"),
    new TFile("data/Al50/beamwidthstudy/tm.p.2layer.pbite1.20.root", "READ"),
    new TFile("data/Al50/beamwidthstudy/tm.p.2layer.pbite1.30.root", "READ"),
    new TFile("data/Al50/beamwidthstudy/tm.p.2layer.pbite1.40.root", "READ"),
    new TFile("data/Al50/beamwidthstudy/tm.p.2layer.pbite1.50.root", "READ"),
    new TFile("data/Al50/beamwidthstudy/tm.p.2layer.pbite1.60.root", "READ"),
    new TFile("data/Al50/beamwidthstudy/tm.p.2layer.pbite1.70.root", "READ"),
    new TFile("data/Al50/beamwidthstudy/tm.p.2layer.pbite1.80.root", "READ"),
    new TFile("data/Al50/beamwidthstudy/tm.p.2layer.pbite1.90.root", "READ"),
    new TFile("data/Al50/beamwidthstudy/tm.p.2layer.pbite2.00.root", "READ")
  };

  TTree* trp[2] = {
    (TTree*)fdata->Get("PID_LP"), (TTree*)fdata->Get("PID_RP")
  };
  RooUnfoldResponse* resp[2][N];
  TTree*             trtm[2][N];
  for (int i = 0; i < N; ++i) {
    resp[0][i] = (RooUnfoldResponse*)f[i]->Get("SiL_TM");
    resp[1][i] = (RooUnfoldResponse*)f[i]->Get("SiR_TM");
    trtm[0][i] = (TTree*)f[i]->Get("SiL_TransTree");
    trtm[1][i] = (TTree*)f[i]->Get("SiR_TransTree");
  }
  trp[0]->Draw("e>>hlpid(26,2e3,15e3)", "t > 400 && abs(dt) < 200 && e3 == 0.", "goff");
  trp[1]->Draw("e>>hrpid(26,2e3,15e3)", "t > 400 && abs(dt) < 200 && e3 == 0.", "goff");
  TH1* hpid[2] = {
    (TH1*)gDirectory->Get("hlpid"), (TH1*)gDirectory->Get("hrpid")
  };


  //////////////////////////////////////////////////////////////////////////////
  // Unfold energy spectra
  //////////////////////////////////////////////////////////////////////////////
  RooUnfoldBayes* unf[2][N];
  TH1* en[3][N];
  for (int i = 0; i < N; ++i) {
    // Unfold
    TMUtils::Rebin   (resp[0][i], 5);
    TMUtils::Rebin   (resp[1][i], 5);
    TMUtils::SetRange(resp[0][i], 2e3, 15e3);
    TMUtils::SetRange(resp[1][i], 2e3, 15e3);
    unf[0][i] = new RooUnfoldBayes(resp[0][i], hpid[0], 4);
    unf[1][i] = new RooUnfoldBayes(resp[1][i], hpid[1], 4);
    en [0][i] = unf[0][i]->Hreco();
    en [1][i] = unf[1][i]->Hreco();
    // Make L/R average energy spectra
    char hname[64];
    std::sprintf(hname, "enavg%d", i);
    en[2][i] = (TH1*)en[0][i]->Clone(hname);
    en[2][i]->Add(en[1][i]);
    en[2][i]->Scale(0.5);
  }


  //////////////////////////////////////////////////////////////////////////////
  // Normalization and scale
  //////////////////////////////////////////////////////////////////////////////
  double norm = Normalization::Al50();
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < N; ++j) {
      en[i][j]->Scale(norm);
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // Determine the rates, or at least just gross numbers at this point
  //////////////////////////////////////////////////////////////////////////////
  std::pair<double,double> nroi1[3][N], nroi2[3][N];
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < N; ++j) {
      nroi1[i][j] = HistUtils::IntegralByUserLimits(en[i][j], 3.5e3, 10e3);
      nroi2[i][j] = HistUtils::IntegralByUserLimits(en[i][j], 4e3,   8e3);
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // Start prepping everything to draw
  //////////////////////////////////////////////////////////////////////////////
  Color_t col[N];
  for (int i = 0; i < N; ++i) {
    col[i] = TColor::GetColor(1-Float_t(i)/N, 0., 0.);
    en[0][i]->SetLineColor(col[i]);
    en[1][i]->SetLineColor(col[i]);
    en[2][i]->SetLineColor(col[i]);
  }
  en[0][0]->SetTitle("Using TMs from different stopping depths (SiL);Energy [keV]");
  en[1][0]->SetTitle("Using TMs from different stopping depths (SiR);Energy [keV]");
  en[2][0]->SetTitle("Using TMs from different stopping depths (Avg);Energy [keV]");

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(en [0][0],   "Narrow p-bite", "l");
  leg->AddEntry(en [0][N-1], "Widest p-bite", "l");

  gStyle->SetOptStat(0);
  TCanvas* cen = new TCanvas("cen", "Unfolded Energies", 2100, 500);
  cen->Divide(3);
  for (int i = 0; i < 3; ++i) {
    cen->cd(i+1)->SetLogy();
    for (int j = 0; j < N; ++j)
      en[i][j]->Draw("SAME");
    leg->Draw();
  }
  cen->SaveAs("img/unfold_beamwidth_energies.png");

  TCanvas* ctm[2] = {
    new TCanvas("cltm", "Left Transfer Matrices",  2800, 2000),
    new TCanvas("crtm", "Right Transfer Matrices", 2800, 2000)
  };
  resp[0][0] ->Hresponse()->SetTitle("SiL TM pbite -50%;E_{meas} [keV];E_{true} [keV]");
  resp[0][1] ->Hresponse()->SetTitle("SiL TM pbite -40%;E_{meas} [keV];E_{true} [keV]");
  resp[0][2] ->Hresponse()->SetTitle("SiL TM pbite -30%;E_{meas} [keV];E_{true} [keV]");
  resp[0][3] ->Hresponse()->SetTitle("SiL TM pbite -20%;E_{meas} [keV];E_{true} [keV]");
  resp[0][4] ->Hresponse()->SetTitle("SiL TM pbite -10%;E_{meas} [keV];E_{true} [keV]");
  resp[0][5] ->Hresponse()->SetTitle("SiL TM pbite  +0%;E_{meas} [keV];E_{true} [keV]");
  resp[0][6] ->Hresponse()->SetTitle("SiL TM pbite +10%;E_{meas} [keV];E_{true} [keV]");
  resp[0][7] ->Hresponse()->SetTitle("SiL TM pbite +20%;E_{meas} [keV];E_{true} [keV]");
  resp[0][8] ->Hresponse()->SetTitle("SiL TM pbite +30%;E_{meas} [keV];E_{true} [keV]");
  resp[0][9] ->Hresponse()->SetTitle("SiL TM pbite +40%;E_{meas} [keV];E_{true} [keV]");
  resp[0][10]->Hresponse()->SetTitle("SiL TM pbite +50%;E_{meas} [keV];E_{true} [keV]");
  resp[0][11]->Hresponse()->SetTitle("SiL TM pbite +60%;E_{meas} [keV];E_{true} [keV]");
  resp[0][12]->Hresponse()->SetTitle("SiL TM pbite +70%;E_{meas} [keV];E_{true} [keV]");
  resp[0][13]->Hresponse()->SetTitle("SiL TM pbite +80%;E_{meas} [keV];E_{true} [keV]");
  resp[0][14]->Hresponse()->SetTitle("SiL TM pbite +90%;E_{meas} [keV];E_{true} [keV]");
  resp[0][15]->Hresponse()->SetTitle("SiL TM pbite +100%;E_{meas} [keV];E_{true} [keV]");
  resp[1][0] ->Hresponse()->SetTitle("SiL TM pbite -50%;E_{meas} [keV];E_{true} [keV]");
  resp[1][1] ->Hresponse()->SetTitle("SiL TM pbite -40%;E_{meas} [keV];E_{true} [keV]");
  resp[1][2] ->Hresponse()->SetTitle("SiL TM pbite -30%;E_{meas} [keV];E_{true} [keV]");
  resp[1][3] ->Hresponse()->SetTitle("SiL TM pbite -20%;E_{meas} [keV];E_{true} [keV]");
  resp[1][4] ->Hresponse()->SetTitle("SiL TM pbite -10%;E_{meas} [keV];E_{true} [keV]");
  resp[1][5] ->Hresponse()->SetTitle("SiL TM pbite  +0%;E_{meas} [keV];E_{true} [keV]");
  resp[1][6] ->Hresponse()->SetTitle("SiL TM pbite +10%;E_{meas} [keV];E_{true} [keV]");
  resp[1][7] ->Hresponse()->SetTitle("SiL TM pbite +20%;E_{meas} [keV];E_{true} [keV]");
  resp[1][8] ->Hresponse()->SetTitle("SiL TM pbite +30%;E_{meas} [keV];E_{true} [keV]");
  resp[1][9] ->Hresponse()->SetTitle("SiL TM pbite +40%;E_{meas} [keV];E_{true} [keV]");
  resp[1][10]->Hresponse()->SetTitle("SiL TM pbite +50%;E_{meas} [keV];E_{true} [keV]");
  resp[1][11]->Hresponse()->SetTitle("SiL TM pbite +60%;E_{meas} [keV];E_{true} [keV]");
  resp[1][12]->Hresponse()->SetTitle("SiL TM pbite +70%;E_{meas} [keV];E_{true} [keV]");
  resp[1][13]->Hresponse()->SetTitle("SiL TM pbite +80%;E_{meas} [keV];E_{true} [keV]");
  resp[1][14]->Hresponse()->SetTitle("SiL TM pbite +90%;E_{meas} [keV];E_{true} [keV]");
  resp[1][15]->Hresponse()->SetTitle("SiL TM pbite +100%;E_{meas} [keV];E_{true} [keV]");
  for (int i = 0; i < 2; ++i) {
    ctm[i]->Divide(4, 4);
    for (int j = 0; j < N; ++j) {
      ctm[i]->cd(j+1);
      resp[i][j]->Hresponse()->GetYaxis()->SetTitleOffset(1.4);
      resp[i][j]->Hresponse()->Draw("COL");
    }
  }
  ctm[0]->SaveAs("img/unfold_beamwidth_tm_l.png");
  ctm[1]->SaveAs("img/unfold_beamwidth_tm_r.png");

  //////////////////////////////////////////////////////////////////////////////
  // Print rates and graph
  //////////////////////////////////////////////////////////////////////////////
  printf("==================================================\n");
  printf("3.5-10 MeV Beam p-bite L/R/Avg:\n");
  printf("- 50%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][0].first, nroi1[0][0].second, nroi1[1][0].first, nroi1[1][0].second, nroi1[2][0].first, nroi1[2][0].second);
  printf("- 40%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][1].first, nroi1[0][1].second, nroi1[1][1].first, nroi1[1][1].second, nroi1[2][1].first, nroi1[2][1].second);
  printf("- 30%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][2].first, nroi1[0][2].second, nroi1[1][2].first, nroi1[1][2].second, nroi1[2][2].first, nroi1[2][2].second);
  printf("- 20%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][3].first, nroi1[0][3].second, nroi1[1][3].first, nroi1[1][3].second, nroi1[2][3].first, nroi1[2][3].second);
  printf("- 10%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][4].first, nroi1[0][4].second, nroi1[1][4].first, nroi1[1][4].second, nroi1[2][4].first, nroi1[2][4].second);
  printf("+  0%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][4].first, nroi1[0][4].second, nroi1[1][4].first, nroi1[1][4].second, nroi1[2][4].first, nroi1[2][4].second);
  printf("+ 10%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][4].first, nroi1[0][4].second, nroi1[1][4].first, nroi1[1][4].second, nroi1[2][4].first, nroi1[2][4].second);
  printf("+ 20%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][4].first, nroi1[0][4].second, nroi1[1][4].first, nroi1[1][4].second, nroi1[2][4].first, nroi1[2][4].second);
  printf("+ 30%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][4].first, nroi1[0][4].second, nroi1[1][4].first, nroi1[1][4].second, nroi1[2][4].first, nroi1[2][4].second);
  printf("+ 40%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][4].first, nroi1[0][4].second, nroi1[1][4].first, nroi1[1][4].second, nroi1[2][4].first, nroi1[2][4].second);
  printf("+ 50%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][4].first, nroi1[0][4].second, nroi1[1][4].first, nroi1[1][4].second, nroi1[2][4].first, nroi1[2][4].second);
  printf("+ 60%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][4].first, nroi1[0][4].second, nroi1[1][4].first, nroi1[1][4].second, nroi1[2][4].first, nroi1[2][4].second);
  printf("+ 70%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][4].first, nroi1[0][4].second, nroi1[1][4].first, nroi1[1][4].second, nroi1[2][4].first, nroi1[2][4].second);
  printf("+ 80%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][4].first, nroi1[0][4].second, nroi1[1][4].first, nroi1[1][4].second, nroi1[2][4].first, nroi1[2][4].second);
  printf("+ 90%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][4].first, nroi1[0][4].second, nroi1[1][4].first, nroi1[1][4].second, nroi1[2][4].first, nroi1[2][4].second);
  printf("+100%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][4].first, nroi1[0][4].second, nroi1[1][4].first, nroi1[1][4].second, nroi1[2][4].first, nroi1[2][4].second);
  printf("==================================================\n");
  printf("4-8 MeV Beam p-bite L/R/Avg:\n");
  printf("- 50%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][0].first, nroi2[0][0].second, nroi2[1][0].first, nroi2[1][0].second, nroi2[2][0].first, nroi2[2][0].second);
  printf("- 40%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][1].first, nroi2[0][1].second, nroi2[1][1].first, nroi2[1][1].second, nroi2[2][1].first, nroi2[2][1].second);
  printf("- 30%%:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][2].first, nroi2[0][2].second, nroi2[1][2].first, nroi2[1][2].second, nroi2[2][2].first, nroi2[2][2].second);
  printf("- 20%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][3].first, nroi2[0][3].second, nroi2[1][3].first, nroi2[1][3].second, nroi2[2][3].first, nroi2[2][3].second);
  printf("- 10%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][4].first, nroi2[0][4].second, nroi2[1][4].first, nroi2[1][4].second, nroi2[2][4].first, nroi2[2][4].second);
  printf("+  0%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][4].first, nroi2[0][4].second, nroi2[1][4].first, nroi2[1][4].second, nroi2[2][4].first, nroi2[2][4].second);
  printf("+ 10%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][4].first, nroi2[0][4].second, nroi2[1][4].first, nroi2[1][4].second, nroi2[2][4].first, nroi2[2][4].second);
  printf("+ 20%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][4].first, nroi2[0][4].second, nroi2[1][4].first, nroi2[1][4].second, nroi2[2][4].first, nroi2[2][4].second);
  printf("+ 30%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][4].first, nroi2[0][4].second, nroi2[1][4].first, nroi2[1][4].second, nroi2[2][4].first, nroi2[2][4].second);
  printf("+ 40%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][4].first, nroi2[0][4].second, nroi2[1][4].first, nroi2[1][4].second, nroi2[2][4].first, nroi2[2][4].second);
  printf("+ 50%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][4].first, nroi2[0][4].second, nroi2[1][4].first, nroi2[1][4].second, nroi2[2][4].first, nroi2[2][4].second);
  printf("+ 60%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][4].first, nroi2[0][4].second, nroi2[1][4].first, nroi2[1][4].second, nroi2[2][4].first, nroi2[2][4].second);
  printf("+ 70%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][4].first, nroi2[0][4].second, nroi2[1][4].first, nroi2[1][4].second, nroi2[2][4].first, nroi2[2][4].second);
  printf("+ 80%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][4].first, nroi2[0][4].second, nroi2[1][4].first, nroi2[1][4].second, nroi2[2][4].first, nroi2[2][4].second);
  printf("+ 90%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][4].first, nroi2[0][4].second, nroi2[1][4].first, nroi2[1][4].second, nroi2[2][4].first, nroi2[2][4].second);
  printf("+100%%:\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][4].first, nroi2[0][4].second, nroi2[1][4].first, nroi2[1][4].second, nroi2[2][4].first, nroi2[2][4].second);

  Double_t x[N] = {
    -50, -40, -30, -20, -10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100
  };
  Double_t      yroi1[3][N], eroi1[3][N];
  Double_t      yroi2[3][N], eroi2[3][N];
  TGraphErrors  *groi1[3],   *groi2[3];
  TLine         *l0roi1[3],  *l0roi2[3];
  TMultiGraph*  mgroi1  = new TMultiGraph("mgroi1", "3.5-10 MeV rates as function of muon beam p-bite;p-bite change [+%];Rate");
  TMultiGraph*  mgroi2  = new TMultiGraph("mgroi2", "4-8 MeV rates as function of muon beam p-bite;p-bite change [+%];Rate");
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < N; ++j) {
      yroi1[i][j] = nroi1[i][j].first;
      eroi1[i][j] = nroi1[i][j].second;
      yroi2[i][j] = nroi2[i][j].first;
      eroi2[i][j] = nroi2[i][j].second;
    }
    groi1[i] = new TGraphErrors(N, x, yroi1[i], nullptr, eroi1[i]);
    groi2[i] = new TGraphErrors(N, x, yroi2[i], nullptr, eroi2[i]);
    mgroi1->Add(groi1[i]);
    mgroi2->Add(groi2[i]);
  }
  groi1[0]->SetLineColor(kBlue); groi1[1]->SetLineColor(kRed); groi1[2]->SetLineColor(kBlack);
  groi2[0]->SetLineColor(kBlue); groi2[1]->SetLineColor(kRed); groi2[2]->SetLineColor(kBlack);
  groi1[0]->SetTitle("SiL");     groi1[1]->SetTitle("SiR");    groi1[2]->SetTitle("Avg");
  groi2[0]->SetTitle("SiL");     groi2[1]->SetTitle("SiR");    groi2[2]->SetTitle("Avg");

  TLegend* lmg = new TLegend(0.7, 0.7, 0.9, 0.9);
  lmg->AddEntry(groi1 [0], "SiL", "l");
  lmg->AddEntry(groi1 [1], "SiR", "l");
  lmg->AddEntry(groi1 [2], "Avg", "l");

  TCanvas* cmg = new TCanvas("cmg", "Rates", 1400, 500);
  cmg->Divide(2);
  cmg->cd(1); mgroi1->Draw("APL");
  cmg->cd(2); mgroi2->Draw("APL");
  cmg->SaveAs("img/unfold_beamwidth_rates.png");

  std::pair<double,double> roi1_avg = AvgAndStd(groi1[2]);
  std::pair<double,double> roi2_avg = AvgAndStd(groi2[2]);
  printf("ROI1: %g +/- %g\n", roi1_avg.first, roi1_avg.second);
  printf("ROI2: %g +/- %g\n", roi2_avg.first, roi2_avg.second);

  // double x0, y0;
  // if (IntersectionPoint(groi1[0], groi1[1], x0, y0)) {
  //   std::cout << "ROI1 Intersection: (" << x0 << ", " << y0 << ")" << std::endl;
  // } else {
  //   std::cout << "No intersection for ROI1: " << x0 << ", " << y0 << std::endl;
  // }
  // if (IntersectionPoint(groi2[0], groi2[1], x0, y0)) {
  //   std::cout << "ROI1 Intersection: (" << x0 << ", " << y0 << ")" << std::endl;
  // } else {
  //   std::cout << "No intersection for ROI1: " << x0 << ", " << y0 << std::endl;
  // }


  // Plot z stopping distribution
  TH1* hdep[N];
  TF1* fdep[N];
  TCanvas* cdep = new TCanvas("cdep", "Depths", 700, 500);
  TLegend* ldep = new TLegend(0.1, 0.5, 0.5, 0.9);
  for (int i = 0; i < N; ++i) {
    char hname[64], fname[64], cmd[64];
    std::sprintf(hname, "hdep%d", i);
    std::sprintf(fname, "fdep%d", i);
    std::sprintf(cmd,   "1e4*z0>>%s", hname);
    hdep[i] = new TH1D(hname, "Stopping Depth;Depth [#mum];Norm. Rate",
                       60, -30., 30.);
    fdep[i] = new TF1(fname, "gaus(0)", -25, 25);
    trtm[0][i]->Draw(cmd, "", "goff");
    hdep[i]->Scale(1./hdep[i]->GetEntries());
    hdep[i]->SetLineColor(col[i]);
    fdep[i]->SetLineColor(col[i]);
    TFitResultPtr res = hdep[i]->Fit(fdep[i], "0S");
    hdep[i]->Draw("SAME");
    fdep[i]->Draw("SAME");
    char lentry[64];
    std::sprintf(lentry, "%+2.0f%% #sigma=%2.0f (#chi^{2}_{red}=%.0f)", x[i],
                 fdep[i]->GetParameter(2), res->Chi2()/res->Ndf());
    ldep->AddEntry(fdep[i], lentry);
  }
  ldep->Draw();
  cdep->SaveAs("img/unfold_beamwidth_depth.png");
}
