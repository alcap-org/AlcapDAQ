#include "scripts/Al50/util.h"
using SiUtils::ECal;

#include "RooUnfoldBayes.h"
#include "RooUnfoldSvd.h"
#include "RooUnfoldResponse.h"

#include "TCanvas.h"
#include "TCut.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "TProfile.h"
#include "TStyle.h"
#include "TTree.h"

#include <cmath>
#include <utility>
#include <iostream>
using std::pow;
using std::sqrt;

static const int N = 11;

void siecal_error_propagated() {
  ECal sil1[14] = {
    ECal::ConstructECal("SiL1-2"),  ECal::ConstructECal("SiL1-3"),
    ECal::ConstructECal("SiL1-4"),  ECal::ConstructECal("SiL1-5"),
    ECal::ConstructECal("SiL1-6"),  ECal::ConstructECal("SiL1-7"),
    ECal::ConstructECal("SiL1-8"),  ECal::ConstructECal("SiL1-9"),
    ECal::ConstructECal("SiL1-10"), ECal::ConstructECal("SiL1-11"),
    ECal::ConstructECal("SiL1-12"), ECal::ConstructECal("SiL1-13"),
    ECal::ConstructECal("SiL1-14"), ECal::ConstructECal("SiL1-15")
  };
  SiUtils::ECal sir1[4] = {
    ECal::ConstructECal("SiR1-1"), ECal::ConstructECal("SiR1-2"),
    ECal::ConstructECal("SiR1-3"), ECal::ConstructECal("SiR1-4"),
  };
  ECal sir2 = ECal::ConstructECal("SiR2");
  ECal sil2 = ECal::ConstructECal("SiL3");


  double x[20] = {
    2250, 2750, 3250, 3750, 4250, 4750, 5250,  5750,  6250,  6750,
    7250, 7750, 8250, 8750, 9250, 9750, 10250, 10750, 11250, 11750
  };
  double yl[14][20], yr[4][20];
  for (int i = 0; i < 20; ++i) {
    double E = 2250+500*i;
    double err_l2 = sil2.Eerr(E);
    double err_r2 = sir2.Eerr(E);
    for (int j = 0; j < 14; ++j)
      yl[j][i] = sqrt(pow(sil1[j].Eerr(E), 2)+err_l2*err_l2)/E;
    for (int j = 0; j < 4; ++j)
      yr[j][i] = sqrt(pow(sir1[j].Eerr(E), 2)+err_r2*err_r2)/E;
  }
  TGraph* gl[14];
  TGraph* gr[4];
  for (int i = 0; i < 14; ++i) {
    gl[i] = new TGraph(20, x, yl[i]);
    gl[i]->SetMarkerStyle(6);
  }
  for (int i = 0; i < 4; ++i) {
    gr[i] = new TGraph(20, x, yr[i]);
    gr[i]->SetMarkerStyle(6);
  }

  TMultiGraph* mg[2] = {
    new TMultiGraph("mgl", "SiL Energy Calibration Error;Total Energy (E_{1}+E_{2}) [keV];Relative Error #DeltaE/E"),
    new TMultiGraph("mgr", "SiR Energy Calibration Error;Total Energy (E_{1}+E_{2}) [keV];Relative Error #DeltaE/E")
  };

  TLegend* leg[2] = {
    new TLegend(0.72, 0.40, 0.95, 0.95), new TLegend(0.72, 0.60, 0.95, 0.95)
  };
  leg[0]->SetHeader("Detector Segment");
  leg[1]->SetHeader("Detector Segment");
  for (int i = 0; i < 14; ++i) {
    char txt[16];
    std::sprintf(txt, "%d", i+2);
    leg[0]->AddEntry(gl[i], txt, "p");
    mg[0]->Add(gl[i]);
  }
  for (int i = 0; i < 4; ++i) {
    char txt[16];
    std::sprintf(txt, "%d", i+1);
    leg[1]->AddEntry(gr[i], txt, "p");
    mg[1]->Add(gr[i]);
  }

  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1); mg[0]->Draw("AP PMC"); leg[0]->Draw();
  c->cd(2); mg[1]->Draw("AP PMC"); leg[1]->Draw();
  c->SaveAs("img/systematics/siecal_effect_energy.png");

}

void siecal_error() {
  TFile* fdata = new TFile("data/Al50/psel.3layer.pidpow2.root");
  TFile* ftm   = new TFile("data/Al50/tms/tm.p.2layer.mudist.root");
  TTree* tr[2] = { (TTree*)fdata->Get("PID_LP"), (TTree*)fdata->Get("PID_RP") };
  TCut psel = "abs(dt)<200 && 400<t && t<10e3 && e3==0";
  double X[N] = { -500, -400, -300, -200, -100, 0, 100, 200, 300, 400, 500 };
  TH1* heraw[2][N] = {
    {
      new TH1D("hl095", "hl095", 20, 2e3, 12e3),
      new TH1D("hl096", "hl096", 20, 2e3, 12e3),
      new TH1D("hl097", "hl097", 20, 2e3, 12e3),
      new TH1D("hl098", "hl098", 20, 2e3, 12e3),
      new TH1D("hl099", "hl099", 20, 2e3, 12e3),
      new TH1D("hl100", "hl100", 20, 2e3, 12e3),
      new TH1D("hl101", "hl101", 20, 2e3, 12e3),
      new TH1D("hl102", "hl102", 20, 2e3, 12e3),
      new TH1D("hl103", "hl103", 20, 2e3, 12e3),
      new TH1D("hl104", "hl104", 20, 2e3, 12e3),
      new TH1D("hl105", "hl105", 20, 2e3, 12e3)
    }, {
      new TH1D("hr095", "hr095", 20, 2e3, 12e3),
      new TH1D("hr096", "hr096", 20, 2e3, 12e3),
      new TH1D("hr097", "hr097", 20, 2e3, 12e3),
      new TH1D("hr098", "hr098", 20, 2e3, 12e3),
      new TH1D("hr099", "hr099", 20, 2e3, 12e3),
      new TH1D("hr100", "hr100", 20, 2e3, 12e3),
      new TH1D("hr101", "hr101", 20, 2e3, 12e3),
      new TH1D("hr102", "hr102", 20, 2e3, 12e3),
      new TH1D("hr103", "hr103", 20, 2e3, 12e3),
      new TH1D("hr104", "hr104", 20, 2e3, 12e3),
      new TH1D("hr105", "hr105", 20, 2e3, 12e3)
    }
  };
  tr[0]->Draw("0.95*e>>hl095", psel, "goff");
  tr[0]->Draw("0.96*e>>hl096", psel, "goff");
  tr[0]->Draw("0.97*e>>hl097", psel, "goff");
  tr[0]->Draw("0.98*e>>hl098", psel, "goff");
  tr[0]->Draw("0.99*e>>hl099", psel, "goff");
  tr[0]->Draw("1.00*e>>hl100", psel, "goff");
  tr[0]->Draw("1.01*e>>hl101", psel, "goff");
  tr[0]->Draw("1.02*e>>hl102", psel, "goff");
  tr[0]->Draw("1.03*e>>hl103", psel, "goff");
  tr[0]->Draw("1.04*e>>hl104", psel, "goff");
  tr[0]->Draw("1.05*e>>hl105", psel, "goff");
  tr[1]->Draw("0.95*e>>hr095", psel, "goff");
  tr[1]->Draw("0.96*e>>hr096", psel, "goff");
  tr[1]->Draw("0.97*e>>hr097", psel, "goff");
  tr[1]->Draw("0.98*e>>hr098", psel, "goff");
  tr[1]->Draw("0.99*e>>hr099", psel, "goff");
  tr[1]->Draw("1.00*e>>hr100", psel, "goff");
  tr[1]->Draw("1.01*e>>hr101", psel, "goff");
  tr[1]->Draw("1.02*e>>hr102", psel, "goff");
  tr[1]->Draw("1.03*e>>hr103", psel, "goff");
  tr[1]->Draw("1.04*e>>hr104", psel, "goff");
  tr[1]->Draw("1.05*e>>hr105", psel, "goff");

  TProfile* hperaw[2] = {
    new TProfile("hlrawprof", "SiL energy profile of different Si ECal;Energy [keV];Unfolded proton hits/500 keV", 20, 2e3, 12e3, "s"),
    new TProfile("hrrawprof", "SiR energy profile of different Si ECal;Energy [keV];Unfolded proton hits/500 keV", 20, 2e3, 12e3, "s")
  };
  for (int i = 0; i < N; ++i) {
    for (int j = 1; j <= heraw[0][i]->GetNbinsX(); ++j)
      hperaw[0]->Fill(heraw[0][i]->GetBinCenter(j), heraw[0][i]->GetBinContent(j));
    for (int j = 1; j <= heraw[1][i]->GetNbinsX(); ++j)
      hperaw[1]->Fill(heraw[1][i]->GetBinCenter(j), heraw[1][i]->GetBinContent(j));
  }

  RooUnfoldResponse* resp[2] = {
    (RooUnfoldResponse*)ftm->Get("SiL_TM"),
    (RooUnfoldResponse*)ftm->Get("SiR_TM")
  };
  TMUtils::Rebin(resp[0], 5);
  TMUtils::Rebin(resp[1], 5);
  TMUtils::SetRange(resp[0], 2e3, 12e3);
  TMUtils::SetRange(resp[1], 2e3, 12e3);
  RooUnfoldBayes* unf[2][N];
  TH1* he[2][N];
  for (int i = 0; i < N; ++i) {
    unf[0][i] = new RooUnfoldBayes(resp[0], heraw[0][i], 4);
    unf[1][i] = new RooUnfoldBayes(resp[1], heraw[1][i], 4);
    he [0][i] = unf[0][i]->Hreco();
    he [1][i] = unf[1][i]->Hreco();
    he [0][i]->SetTitle("Left Si energy spectrum with uncertain energy calibration;Energy [keV]");
    he [1][i]->SetTitle("Right Si energy spectrum with uncertain energy calibration;Energy [keV]");
  }

  TProfile* hpe[2] = {
    new TProfile("hleprof", "SiL energy profile of different Si ECal;Energy [keV];Unfolded proton hits/500 keV", 20, 2e3, 12e3, "s"),
    new TProfile("hreprof", "SiR energy profile of different Si ECal;Energy [keV];Unfolded proton hits/500 keV", 20, 2e3, 12e3, "s")
  };
  for (int i = 0; i < N; ++i) {
    for (int j = 1; j <= he[0][i]->GetNbinsX(); ++j)
      hpe[0]->Fill(he[0][i]->GetBinCenter(j), he[0][i]->GetBinContent(j));
    for (int j = 1; j <= he[1][i]->GetNbinsX(); ++j)
      hpe[1]->Fill(he[1][i]->GetBinCenter(j), he[1][i]->GetBinContent(j));
  }

  TLegend* leg = new TLegend(0.75, 0.25, 0.95, 0.95);
  leg->SetHeader("Si Miscal @ 10 MeV");
  leg->AddEntry(he[0][0],  "-500 keV");
  leg->AddEntry(he[0][1],  "-400 keV");
  leg->AddEntry(he[0][2],  "-300 keV");
  leg->AddEntry(he[0][3],  "-200 keV");
  leg->AddEntry(he[0][4],  "-100 keV");
  leg->AddEntry(he[0][5],  "0 keV");
  leg->AddEntry(he[0][6],  "+100 keV");
  leg->AddEntry(he[0][7],  "+200 keV");
  leg->AddEntry(he[0][8],  "+300 keV");
  leg->AddEntry(he[0][9],  "+400 keV");
  leg->AddEntry(he[0][10], "+500 keV");

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1); for (int i = 0; i < N; ++i) he[0][i]->Draw("SAME PLC"); leg->Draw();
  c->cd(2); for (int i = 0; i < N; ++i) he[1][i]->Draw("SAME PLC"); leg->Draw();
  c->SaveAs("img/systematics/siecal_effect_en.png");

  hperaw[0]->SetLineColor(kBlack);
  hperaw[1]->SetLineColor(kBlack);
  hpe   [0]->SetLineColor(kBlack);
  hpe   [1]->SetLineColor(kBlack);

  TCanvas* cp = new TCanvas("cp", "cp", 1400, 500);
  cp->Divide(2);
  cp->cd(1); hpe[0]->Draw();
  cp->cd(2); hpe[1]->Draw();
  cp->SaveAs("img/systematics/siecal_effect_enprof.png");

  double n[2][2][N], en[2][2][N];
  for (int i = 0; i < N; ++i) {
    std::pair<double, double> p[2][2] = {
      {
        HistUtils::IntegralByUserLimits(he[0][i], 4e3,   8e3),
        HistUtils::IntegralByUserLimits(he[0][i], 3.5e3, 10e3)
      }, {
        HistUtils::IntegralByUserLimits(he[1][i], 4e3,   8e3),
        HistUtils::IntegralByUserLimits(he[1][i], 3.5e3, 10e3)
      }
    };
    n[0][0][i] = p[0][0].first; en[0][0][i] = p[0][0].second;
    n[0][1][i] = p[0][1].first; en[0][1][i] = p[0][1].second;
    n[1][0][i] = p[1][0].first; en[1][0][i] = p[1][0].second;
    n[1][1][i] = p[1][1].first; en[1][1][i] = p[1][1].second;
  }
  double n0[2][2];
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j)
      n0[i][j] = n[i][j][N/2];

  double navg[2][2] = {0.};
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      for (int k = 0; k < N; ++k)
        navg[i][j] += n[i][j][k];
      navg[i][j] /= N;
    }
  }
  double sig2[2][2] = {0.};
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      for (int k = 0; k < N; ++k)
        sig2[i][j] += std::pow(n[i][j][k]-navg[i][j], 2);
      sig2[i][j] /= (N-1);

    }
  }
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j)
      std::cout << "(" << n0[i][j] << ", " << navg[i][j] << ", " << std::sqrt(sig2[i][j]) << ")" << std::endl;

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j)
      for (int k = 0; k < N; ++k)
        n[i][j][k] /= n0[i][j];

  TGraphErrors* gr[2][2] = {
    {
      new TGraphErrors(N, X, n[0][0], nullptr, nullptr/*en[0][0]*/),
      new TGraphErrors(N, X, n[0][1], nullptr, nullptr/*en[0][1]*/)
    }, {
      new TGraphErrors(N, X, n[1][0], nullptr, nullptr/*en[1][0]*/),
      new TGraphErrors(N, X, n[1][1], nullptr, nullptr/*en[1][1]*/)
    }
  };

  gr[0][0]->SetMarkerStyle(8);
  gr[0][1]->SetMarkerStyle(8);
  gr[1][0]->SetMarkerStyle(8);
  gr[1][1]->SetMarkerStyle(8);
  gr[0][0]->SetName("gl1");
  gr[0][1]->SetName("gl2");
  gr[1][0]->SetName("gr1");
  gr[1][1]->SetName("gr2");
  gr[0][0]->SetTitle("Si ECal Dependence (4-8 MeV);Si Gain Miscalibration @ 10 MeV [keV];Change in Capture Rate");
  gr[0][1]->SetTitle("Si ECal Dependence (3.5-10 MeV);Si Gain Miscalibration @ 10 MeV [keV];Change in Capture Rates");
  gr[1][0]->SetTitle("Si ECal Dependence (4-8 MeV);Si Gain Miscalibration @ 10 MeV [keV];Change in Capture Rates");
  gr[1][1]->SetTitle("Si ECal Dependence (3.5-10 MeV);Si Gain Miscalibration @ 10 MeV [keV];Change in Capture Rates");
  gr[0][0]->SetMarkerColor(ACStyle::kACBlue);
  gr[0][1]->SetMarkerColor(ACStyle::kACBlue);
  gr[1][0]->SetMarkerColor(ACStyle::kACRed);
  gr[1][1]->SetMarkerColor(ACStyle::kACRed);

  TLegend* leg2 = new TLegend(0.75, 0.75, 0.95, 0.95);
  leg2->AddEntry(gr[0][0], "SiL", "p");
  leg2->AddEntry(gr[1][0], "SiR", "p");

  TCanvas* c2 = new TCanvas("c2", "c2", 1400, 500);
  c2->Divide(2);
  c2->cd(1); gr[0][0]->Draw("AP"); gr[1][0]->Draw("P SAME"); leg2->Draw();
  c2->cd(2); gr[0][1]->Draw("AP"); gr[1][1]->Draw("P SAME"); leg2->Draw();
  c2->SaveAs("img/systematics/siecal_effect.png");


  std::cout << "    \t" << "4-8 MeV\t"                      << "\t" << "3.5-10 MeV"                   << std::endl;
  std::cout << "SiL:\t" << std::sqrt(sig2[0][0])/n0[0][0] << "\t" << std::sqrt(sig2[0][1])/n0[0][1] << std::endl;
  std::cout << "SiR:\t" << std::sqrt(sig2[1][0])/n0[1][0] << "\t" << std::sqrt(sig2[1][1])/n0[1][1] << std::endl;

  TFile* fout = new TFile("data/Al50/corranderr/siecal.root", "RECREATE");
  TH1* hout[2] = {
    new TH1D("hl", "hl", 20, 2e3, 12e3),
    new TH1D("hr", "hr", 20, 2e3, 12e3)
  };
  for (int i = 1; i <= 20; ++i) {
    hout[0]->SetBinContent(i, 1);
    hout[1]->SetBinContent(i, 1);
    hout[0]->SetBinError(i, hpe[0]->GetBinError(i)/hpe[0]->GetBinContent(i));
    hout[1]->SetBinError(i, hpe[1]->GetBinError(i)/hpe[1]->GetBinContent(i));
  }
  fout->Write();
}

void ecal_effect() {
  siecal_error_propagated();
}
