// Compare TM from different sections of the target.

#include "util.h"

#include "RooUnfoldBayes.h"
#include "RooUnfoldResponse.h"

#include "TCanvas.h"
#include "TColor.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TLine.h"
#include "TMultiGraph.h"
#include "TStyle.h"

#include <stdio.h>
#include <utility>

void PrintLine(double x,
               const std::pair<double,double> l,
               const std::pair<double,double> r,
               const std::pair<double,double> a) {
  printf("%2dum:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n",
         x, l.first, l.second, r.first, r.second, a.first, a.second);
}

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
    x = ((y2b-y1b) + x1b*(s1-s2))/(s1-s2);
    if (((x1a<x && x<x1b) || (x1b<x && x<x1a)) &&
        ((x2a<x && x<x2b) || (x2b<x && x<x2a))) {
      y = y1a + s1*(x-x1a);
      std::cout << x1a << " " << x2a << " " << x1b << " " << x2b << " " << x << std::endl;
      return true;
    }
  }
  return false;
}

void r15b_al50_unfold_zslicestudy() {
  TH1::SetDefaultSumw2(true);

  TFile* fdata = new TFile("data/Al50/psel.3layer.pidpow2.root");
  TFile* f0 = new TFile("data/Al50/en.p.t400ns.dt200ns.rebin5.pp10us.elo2e3keV.ehi12e3keV.bay5.pidcorr.root");
  static const int N = 11;
  TFile* f[N] = {
    new TFile("data/Al50/momscan/tm/tm.p.2layer.mudist.25.6MeV.root", "READ"),
    new TFile("data/Al50/momscan/tm/tm.p.2layer.mudist.25.7MeV.root", "READ"),
    new TFile("data/Al50/momscan/tm/tm.p.2layer.mudist.25.8MeV.root", "READ"),
    new TFile("data/Al50/momscan/tm/tm.p.2layer.mudist.25.9MeV.root", "READ"),
    new TFile("data/Al50/momscan/tm/tm.p.2layer.mudist.26.0MeV.root", "READ"),
    new TFile("data/Al50/momscan/tm/tm.p.2layer.mudist.26.1MeV.root", "READ"),
    new TFile("data/Al50/momscan/tm/tm.p.2layer.mudist.26.2MeV.root", "READ"),
    new TFile("data/Al50/momscan/tm/tm.p.2layer.mudist.26.3MeV.root", "READ"),
    new TFile("data/Al50/momscan/tm/tm.p.2layer.mudist.26.4MeV.root", "READ"),
    new TFile("data/Al50/momscan/tm/tm.p.2layer.mudist.26.5MeV.root", "READ"),
    new TFile("data/Al50/momscan/tm/tm.p.2layer.mudist.26.6MeV.root", "READ")
  };

  TTree* trp[2] = {
    (TTree*)fdata->Get("PID_LP"), (TTree*)fdata->Get("PID_RP")
  };
  TH1* en0[3] = {
    (TH1*)f0->Get("hl_e_reco_bay"), (TH1*)f0->Get("hr_e_reco_bay"), nullptr
  };
  RooUnfoldResponse* resp[2][N];
  for (int i = 0; i < N; ++i) {
    resp[0][i] = (RooUnfoldResponse*)f[i]->Get("SiL_TM");
    resp[1][i] = (RooUnfoldResponse*)f[i]->Get("SiR_TM");
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
    std::cout << "UNFOLDING " << i << std::endl;
    // Unfold
    TMUtils::Rebin(resp[0][i], 5);
    TMUtils::Rebin(resp[1][i], 5);
    TMUtils::SetRange(resp[0][i], 2e3, 15e3);
    TMUtils::SetRange(resp[1][i], 2e3, 15e3);
    unf[0][i] = new RooUnfoldBayes(resp[0][i], hpid[0], 4);
    unf[1][i] = new RooUnfoldBayes(resp[1][i], hpid[1], 4);
    en [0][i] = unf[0][i]->Hreco();
    en [1][i] = unf[1][i]->Hreco();
    // Make L/R average energy spectra
    char hname[64];
    sprintf(hname, "enavg%d", i);
    en[2][i] = (TH1*)en[0][i]->Clone(hname);
    en[2][i]->Add(en[1][i]);
    en[2][i]->Scale(0.5);
  }
  // Make L/R average spectrum for canonical unfolded data
  en0[2] = (TH1*)en0[0]->Clone("en0avg");
  en0[2]->Add(en0[1]);
  en0[2]->Scale(0.5);


  //////////////////////////////////////////////////////////////////////////////
  // Calculate normalization and scale
  //////////////////////////////////////////////////////////////////////////////
  double nmu_hi = 161e6;  // All Al50
  double caprate = 0.609;
  TF1* ftime = new TF1("f", "exp([0]^2/(2*[1]^2)-x/[1])*TMath::Erfc(([0]^2-[1]*x)/(sqrt(2)*[0]*[1]))", 0, 10000);
  ftime->SetParameters(52.7, 864.);
  double tcuteff = ftime->Integral(400, 1e6)/ftime->Integral(0, 1e6); // Al50
  double norm = 1./(nmu_hi*caprate*tcuteff);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < N; ++j) {
      en[i][j]->Scale(norm);
    }
    en0[i]->Scale(norm);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Determine the rates, or at least just gross numbers at this point
  //////////////////////////////////////////////////////////////////////////////
  std::pair<double,double> nroi1[3][N], nroi2[3][N], n0roi1[3], n0roi2[3];
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < N; ++j) {
      nroi1[i][j] = HistUtils::IntegralByUserLimits(en[i][j], 3.5e3, 10e3);
      nroi2[i][j] = HistUtils::IntegralByUserLimits(en[i][j], 4e3,   8e3);
    }
    n0roi1[i] = HistUtils::IntegralByUserLimits(en0[i], 3.5e3, 10e3);
    n0roi2[i] = HistUtils::IntegralByUserLimits(en0[i], 4e3,   8e3);
  }

  double NROI1=0, NROI2=0;
  for (int i = 0; i < N; ++i) {
    NROI1 += nroi1[2][i].first;
    NROI2 += nroi2[2][i].first;
  }
  NROI1 /= N;
  NROI2 /= N;
  double SIG1=0, SIG2=0;
  for (int i = 0; i < N; ++i) {
    SIG1 += (nroi1[2][i].first-NROI1)*(nroi1[2][i].first-NROI1);
    SIG2 += (nroi2[2][i].first-NROI2)*(nroi2[2][i].first-NROI2);
  }
  SIG1 /= N-1;
  SIG2 /= N-1;
  SIG1 = std::sqrt(SIG1);
  SIG2 = std::sqrt(SIG2);
  std::cout << "SIG1: " << 100*SIG1/NROI1 << "%" << std::endl
            << "SIG2: " << 100*SIG1/NROI2 << "%" << std::endl;

  //////////////////////////////////////////////////////////////////////////////
  // Start prepping everything to draw
  //////////////////////////////////////////////////////////////////////////////
  int col[N] = {
    TColor::GetColor((Float_t)1.0, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.9, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.8, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.7, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.6, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.5, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.4, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.3, (Float_t)0,   (Float_t)0),
    TColor::GetColor((Float_t)0.2, (Float_t)0,   (Float_t)0)
  };
  for (int i = 0; i < N; ++i) {
    en[0][i]->SetLineColor(col[i]);
    en[1][i]->SetLineColor(col[i]);
    en[2][i]->SetLineColor(col[i]);
  }
  en[0][0]->SetTitle("Using TMs from different stopping depths (SiL);Energy [keV]");
  en[1][0]->SetTitle("Using TMs from different stopping depths (SiR);Energy [keV]");
  en[2][0]->SetTitle("Using TMs from different stopping depths (Avg);Energy [keV]");

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(en0[0],    "Canonical",       "l");
  leg->AddEntry(en [0][0], "Front of Target", "l");
  leg->AddEntry(en [0][8], "Back of Target",  "l");

  gStyle->SetOptStat(0);
  TCanvas* cen = new TCanvas("cen", "Unfolded Energies", 2100, 500);
  cen->Divide(3);
  for (int i = 0; i < 3; ++i) {
    cen->cd(i+1)->SetLogy();
    for (int j = 0; j < N; ++j)
      en[i][j]->Draw("SAME");
    en0[i]->Draw("SAME");
    leg   ->Draw();
  }
  cen->SaveAs("img/unfold_targdep_energies.png");

  TCanvas* ctm[2] = {
    new TCanvas("cltm", "Left Depth Tranfer Matrices",  2100, 1500),
    new TCanvas("crtm", "Right Depth Tranfer Matrices", 2100, 1500)
  };
  resp[0][0]->Hresponse()->SetTitle("SiL TM z=5#mum;E_{meas} [keV];E_{true} [keV]");
  resp[0][1]->Hresponse()->SetTitle("SiL TM z=10#mum;E_{meas} [keV];E_{true} [keV]");
  resp[0][2]->Hresponse()->SetTitle("SiL TM z=15#mum;E_{meas} [keV];E_{true} [keV]");
  resp[0][3]->Hresponse()->SetTitle("SiL TM z=20#mum;E_{meas} [keV];E_{true} [keV]");
  resp[0][4]->Hresponse()->SetTitle("SiL TM z=25#mum;E_{meas} [keV];E_{true} [keV]");
  resp[0][5]->Hresponse()->SetTitle("SiL TM z=30#mum;E_{meas} [keV];E_{true} [keV]");
  resp[0][6]->Hresponse()->SetTitle("SiL TM z=35#mum;E_{meas} [keV];E_{true} [keV]");
  resp[0][7]->Hresponse()->SetTitle("SiL TM z=40#mum;E_{meas} [keV];E_{true} [keV]");
  resp[0][8]->Hresponse()->SetTitle("SiL TM z=45#mum;E_{meas} [keV];E_{true} [keV]");
  resp[1][0]->Hresponse()->SetTitle("SiR TM z=5#mum;E_{meas} [keV];E_{true} [keV]");
  resp[1][1]->Hresponse()->SetTitle("SiR TM z=10#mum;E_{meas} [keV];E_{true} [keV]");
  resp[1][2]->Hresponse()->SetTitle("SiR TM z=15#mum;E_{meas} [keV];E_{true} [keV]");
  resp[1][3]->Hresponse()->SetTitle("SiR TM z=20#mum;E_{meas} [keV];E_{true} [keV]");
  resp[1][4]->Hresponse()->SetTitle("SiR TM z=25#mum;E_{meas} [keV];E_{true} [keV]");
  resp[1][5]->Hresponse()->SetTitle("SiR TM z=30#mum;E_{meas} [keV];E_{true} [keV]");
  resp[1][6]->Hresponse()->SetTitle("SiR TM z=35#mum;E_{meas} [keV];E_{true} [keV]");
  resp[1][7]->Hresponse()->SetTitle("SiR TM z=40#mum;E_{meas} [keV];E_{true} [keV]");
  resp[1][8]->Hresponse()->SetTitle("SiR TM z=45#mum;E_{meas} [keV];E_{true} [keV]");
  for (int i = 0; i < 2; ++i) {
    ctm[i]->Divide(3, 3);
    for (int j = 0; j < N; ++j) {
      ctm[i]->cd(j+1);
      resp[i][j]->Hresponse()->GetYaxis()->SetTitleOffset(1.4);
      resp[i][j]->Hresponse()->Draw("COL");
    }
  }
  ctm[0]->SaveAs("img/unfold_targdep_tm_l.png");
  ctm[1]->SaveAs("img/unfold_targdep_tm_r.png");

  //////////////////////////////////////////////////////////////////////////////
  // Print rates and graph
  //////////////////////////////////////////////////////////////////////////////
  printf("==================================================\n");
  printf("3.5-10 MeV Depth L/R/Avg (from stops in the front of target to back):\n");
  printf(" 5um:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][0].first, nroi1[0][0].second, nroi1[1][0].first, nroi1[1][0].second, nroi1[2][0].first, nroi1[2][0].second);
  printf("10um:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][1].first, nroi1[0][1].second, nroi1[1][1].first, nroi1[1][1].second, nroi1[2][1].first, nroi1[2][1].second);
  printf("15um:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][2].first, nroi1[0][2].second, nroi1[1][2].first, nroi1[1][2].second, nroi1[2][2].first, nroi1[2][2].second);
  printf("20um:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][3].first, nroi1[0][3].second, nroi1[1][3].first, nroi1[1][3].second, nroi1[2][3].first, nroi1[2][3].second);
  printf("25um:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][4].first, nroi1[0][4].second, nroi1[1][4].first, nroi1[1][4].second, nroi1[2][4].first, nroi1[2][4].second);
  printf("30um:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][5].first, nroi1[0][5].second, nroi1[1][5].first, nroi1[1][5].second, nroi1[2][5].first, nroi1[2][5].second);
  printf("35um:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][6].first, nroi1[0][6].second, nroi1[1][6].first, nroi1[1][6].second, nroi1[2][6].first, nroi1[2][6].second);
  printf("40um:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][7].first, nroi1[0][7].second, nroi1[1][7].first, nroi1[1][7].second, nroi1[2][7].first, nroi1[2][7].second);
  printf("45um:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi1[0][8].first, nroi1[0][8].second, nroi1[1][8].first, nroi1[1][8].second, nroi1[2][8].first, nroi1[2][8].second);
  printf("Canon: %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", n0roi1[0]  .first, n0roi1[0]  .second, n0roi1[1]  .first, n0roi1[1]  .second, n0roi1[2]  .first, n0roi1[2]  .second);
  printf("==================================================\n");
  printf("4-8 MeV Depth L/R/Avg (from stops in the front of target to back):\n");
  printf(" 5um:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][0].first, nroi2[0][0].second, nroi2[1][0].first, nroi2[1][0].second, nroi2[2][0].first, nroi2[2][0].second);
  printf("10um:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][1].first, nroi2[0][1].second, nroi2[1][1].first, nroi2[1][1].second, nroi2[2][1].first, nroi2[2][1].second);
  printf("15um:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][2].first, nroi2[0][2].second, nroi2[1][2].first, nroi2[1][2].second, nroi2[2][2].first, nroi2[2][2].second);
  printf("20um:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][3].first, nroi2[0][3].second, nroi2[1][3].first, nroi2[1][3].second, nroi2[2][3].first, nroi2[2][3].second);
  printf("25um:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][4].first, nroi2[0][4].second, nroi2[1][4].first, nroi2[1][4].second, nroi2[2][4].first, nroi2[2][4].second);
  printf("30um:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][5].first, nroi2[0][5].second, nroi2[1][5].first, nroi2[1][5].second, nroi2[2][5].first, nroi2[2][5].second);
  printf("35um:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][6].first, nroi2[0][6].second, nroi2[1][6].first, nroi2[1][6].second, nroi2[2][6].first, nroi2[2][6].second);
  printf("40um:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][7].first, nroi2[0][7].second, nroi2[1][7].first, nroi2[1][7].second, nroi2[2][7].first, nroi2[2][7].second);
  printf("45um:  %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", nroi2[0][8].first, nroi2[0][8].second, nroi2[1][8].first, nroi2[1][8].second, nroi2[2][8].first, nroi2[2][8].second);
  printf("Canon: %6.3g +/- %6.1g\t%6.3g +/- %6.1g\t%6.3g +/- %6.1g\n", n0roi2[0]  .first, n0roi2[0]  .second, n0roi2[1]  .first, n0roi2[1]  .second, n0roi2[2]  .first, n0roi2[2]  .second);

  Double_t x[9] = { 5, 10, 15, 20, 25, 30, 35, 40, 45 };
  Double_t      yroi1[3][N], eroi1[3][N];
  Double_t      yroi2[3][N], eroi2[3][N];
  TGraphErrors  *groi1[3],   *groi2[3];
  TLine         *l0roi1[3],  *l0roi2[3];
  TMultiGraph*  mgroi1  = new TMultiGraph("mgroi1", "3.5-10 MeV rates as function of muon stop position;Depth [#mum];Rate");
  TMultiGraph*  mgroi2  = new TMultiGraph("mgroi2", "4-8 MeV rates as function of muon stop position;Depth [#mum];Rate");
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
    l0roi1[i] = new TLine(x[0], n0roi1[i].first, x[8], n0roi1[i].first);
    l0roi2[i] = new TLine(x[0], n0roi2[i].first, x[8], n0roi2[i].first);
  }
  groi1[0]->SetLineColor(kBlue); groi1[1]->SetLineColor(kRed); groi1[2]->SetLineColor(kBlack);
  groi2[0]->SetLineColor(kBlue); groi2[1]->SetLineColor(kRed); groi2[2]->SetLineColor(kBlack);
  groi1[0]->SetTitle("SiL");     groi1[1]->SetTitle("SiR");    groi1[2]->SetTitle("Avg");
  groi2[0]->SetTitle("SiL");     groi2[1]->SetTitle("SiR");    groi2[2]->SetTitle("Avg");

  l0roi1[0]->SetLineColor(kBlue);  l0roi1[0]->SetLineStyle(9);
  l0roi1[1]->SetLineColor(kRed);   l0roi1[1]->SetLineStyle(9);
  l0roi1[2]->SetLineColor(kBlack); l0roi1[2]->SetLineStyle(9);
  l0roi2[0]->SetLineColor(kBlue);  l0roi2[0]->SetLineStyle(9);
  l0roi2[1]->SetLineColor(kRed);   l0roi2[1]->SetLineStyle(9);
  l0roi2[2]->SetLineColor(kBlack); l0roi2[2]->SetLineStyle(9);

  TLegend* lmg = new TLegend(0.7, 0.7, 0.9, 0.9);
  lmg->AddEntry(groi1 [0], "Study SiL",     "l");
  lmg->AddEntry(groi1 [1], "Study SiR",     "l");
  lmg->AddEntry(groi1 [2], "Study Avg",     "l");
  lmg->AddEntry(l0roi1[2], "Canonical Avg", "l");

  TCanvas* cmg = new TCanvas("cmg", "Rates", 1400, 500);
  cmg->Divide(2);
  cmg->cd(1); mgroi1->Draw("APL"); l0roi1[0]->Draw("SAME"); l0roi1[1]->Draw("SAME"); l0roi1[2]->Draw("SAME");
  cmg->cd(2); mgroi2->Draw("APL"); l0roi2[0]->Draw("SAME"); l0roi2[1]->Draw("SAME"); l0roi2[2]->Draw("SAME");
  cmg->SaveAs("img/unfold_targdep_rates.png");

  double x0, y0;
  if (IntersectionPoint(groi1[0], groi1[1], x0, y0)) {
    std::cout << "ROI1 Intersection: (" << x0 << ", " << y0 << ")" << std::endl;
    std::cout << "  Difference: " << y0-n0roi1[2].first << std::endl;
  } else {
    std::cout << "No intersection for ROI1: " << x0 << ", " << y0 << std::endl;
  }
  if (IntersectionPoint(groi2[0], groi2[1], x0, y0)) {
    std::cout << "ROI2 Intersection: (" << x0 << ", " << y0 << ")" << std::endl;
    std::cout << "  Difference: " << y0-n0roi2[2].first << std::endl;
  } else {
    std::cout << "No intersection for ROI2: " << x0 << ", " << y0 << std::endl;
  }
}
