#include "./scripts/Al50/util.h"

#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "TStyle.h"

#include <cmath>
#include <vector>

using std::vector;


vector<double> SiEquivalentRangeFromEloss(const vector<double> e0/*MeV*/,
                                          const vector<double> e /*MeV*/) {
  double rho = 2.3290; // Si g/cm3
  vector<double> m;
  for (int i = 0; i < e0.size(); ++i) {
    double x = 0, dx = 1e-6;
    while (e0[i]-e[i]>SiUtils::dE(x+=dx, e0[i]*1e3, 5)/1e3);
    m.push_back(100*x*rho); // x m->cm
  }
  return m;
}

vector<double> ExpectedEnergies(const vector<double>& ys/*keV*/) {
  static TF1* fBB = new TF1("fBB", SiUtils::dEfcn, 1e3, 3e3, 2);
  fBB->SetParameters(58e-6, 5);
  vector<double> xs;
  for (auto y : ys)
    xs.push_back(fBB->GetX(y));
  return xs;
}

vector<double> E2P(const vector<double>& es/*keV*/) {
  // Inputs are kinetic energies, not total energies
  double M = 105658.3745; // Muon mass, keV
  vector<double> ps;
  for (auto e : es)
    ps.push_back(std::sqrt((M+e)*(M+e)-M*M));
  return ps;
}

vector<double> P2E(const vector<double>& ps/*MeV/c*/) {
  // Outputs are kinetic energies, not total energies
  double M = 105.6583745;
  vector<double> es;
  for (auto p : ps)
    es.push_back(std::sqrt(p*p+M*M)-M);
  return es;
}

void sit_e_different_p() {
  vector<TFile*> ifile50{
    new TFile("data/Al50/nmup102.pp10us.root"),
    new TFile("data/Al50/nmup103.pp10us.root")
  };
  vector<TFile*> ifile100{
    new TFile("data/Al100/nmu_10uspp/nmu.p26.2MeV.pp10us.root"),
    new TFile("data/Al100/nmu_10uspp/nmu.p26.3MeV.pp10us.root"),
    new TFile("data/Al100/nmu_10uspp/nmu.p26.4MeV.pp10us.root"),
    new TFile("data/Al100/nmu_10uspp/nmu.p26.5MeV.pp10us.root"),
    new TFile("data/Al100/nmu_10uspp/nmu.p29.9MeV.pp10us.root")
  };
  static const int N50  = ifile50 .size();
  static const int N100 = ifile100.size();
  vector<TH1*> h50, h100;
  for (auto f : ifile50)
    h50.push_back((TH1*)f->Get("hemu"));
  for (auto f : ifile100)
    h100.push_back((TH1*)f->Get("hemu"));
  for (auto h : h50) {
    h->SetTitle("Al50 SiT Energy;E [keV];Cts/10 keV");
    h->Rebin(4);
    h->GetXaxis()->SetRangeUser(200, 600);
    h->Scale(1./h->GetMaximum());
  }
  for (auto h : h100) {
    h->SetTitle("Al100 SiT Energy;E [keV];Cts/10 keV");
    h->Rebin(4);
    h->GetXaxis()->SetRangeUser(200, 600);
    h->Scale(1./h->GetMaximum());
  }
  vector<TF1*> f50 {
    new TF1("f50_1", "gaus(0)", 300, 450),
    new TF1("f50_2", "gaus(0)", 300, 450)
  };
  vector<TF1*> f100 {
    new TF1("f100_1", "gaus(0)", 300, 450),
    new TF1("f100_2", "gaus(0)", 300, 450),
    new TF1("f100_3", "gaus(0)", 300, 450),
    new TF1("f100_4", "gaus(0)", 300, 450),
    new TF1("f100_5", "gaus(0)", 300, 450)
  };
  vector<double> x50{25.9, 26.2}, x100{26.2, 26.3, 26.4, 26.5, 29.9};
  vector<double> y50, y100, ey50, ey100;
  vector<TFitResultPtr> res50, res100;
  for (int i = 0; i < N50; ++i) {
    res50.push_back(h50[i]->Fit(f50[0], "MESRNQ"));
    y50 .push_back(res50.back()->Parameter(1));
    ey50.push_back(res50.back()->ParError(1));
  }
  for (int i = 0; i < N100; ++i) {
    res100.push_back(h100[i]->Fit(f100[0], "MESRNQ"));
    y100  .push_back(res100.back()->Parameter(1));
    ey100 .push_back(res100.back()->ParError(1));
  }
  vector<double> E050  = ExpectedEnergies(y50);
  vector<double> E0100 = ExpectedEnergies(y100);
  vector<double> P050  = E2P(E050);
  vector<double> P0100 = E2P(E0100);
  for (auto& e : E050)  e /= 1e3;
  for (auto& e : E0100) e /= 1e3;
  for (auto& p : P050)  p /= 1e3;
  for (auto& p : P0100) p /= 1e3;
  vector<double> E50, E100;
  for (int i = 0; i < N50; ++i)
    E50.push_back(E050[i]-y50[i]/1e3);
  for (int i = 0; i < N100; ++i)
    E100.push_back(E0100[i]-y100[i]/1e3);
  vector<double> R50  = SiEquivalentRangeFromEloss(P2E(x50),  E0100);
  vector<double> R100 = SiEquivalentRangeFromEloss(P2E(x100), E0100);
  TGraphErrors* g50  = new TGraphErrors(N50,  x50  .data(), y50  .data(), nullptr, ey50 .data());
  TGraphErrors* g100 = new TGraphErrors(N100, x100 .data(), y100 .data(), nullptr, ey100.data());
  TGraph* G50        = new TGraph      (N50,  P050 .data(), y50  .data());
  TGraph* G100       = new TGraph      (N100, P0100.data(), y100 .data());
  TGraph* Ge50       = new TGraph      (N50,  x50  .data(), E50  .data());
  TGraph* Ge100      = new TGraph      (N100, x100 .data(), E100 .data());
  TGraph* Gm50       = new TGraph      (N50,  x50  .data(), R50  .data());
  TGraph* Gm100      = new TGraph      (N100-1, x100 .data(), R100 .data());
  TGraph* Gp0vp50    = new TGraph      (N50,  x50  .data(), P050 .data());
  TGraph* Gp0vp100   = new TGraph      (N100, x100 .data(), P0100.data());
  g50     ->SetMarkerStyle(20);
  g100    ->SetMarkerStyle(20);
  G50     ->SetMarkerStyle(20);
  G100    ->SetMarkerStyle(20);
  Ge50    ->SetMarkerStyle(20);
  Ge100   ->SetMarkerStyle(20);
  Gm50    ->SetMarkerStyle(20);
  Gm100   ->SetMarkerStyle(20);
  Gp0vp50 ->SetMarkerStyle(20);
  Gp0vp100->SetMarkerStyle(20);
  g50     ->SetMarkerColor(kRed);
  g100    ->SetMarkerColor(kBlack);
  G50     ->SetMarkerColor(kRed);
  G100    ->SetMarkerColor(kBlack);
  Ge50    ->SetMarkerColor(kRed);
  Ge100   ->SetMarkerColor(kBlack);
  Gm50    ->SetMarkerColor(kRed);
  Gm100   ->SetMarkerColor(kBlack);
  Gp0vp50 ->SetMarkerColor(kRed);
  Gp0vp100->SetMarkerColor(kBlack);

  TMultiGraph* mg  = new TMultiGraph("mg",  "SiT Energy;Beam Set Momentum [MeV/c];SiT Energy [keV]");
  TMultiGraph* mg0 = new TMultiGraph("mg0", "Bethe Prediction;Expected Momentum [MeV/c];SiT Energy [keV]");
  TMultiGraph* mge = new TMultiGraph("mge", "Expected Energy Post-SiT;Beam Set Momentum [MeV/c];Post-SiT Energy [MeV]");
  TMultiGraph* mgm = new TMultiGraph("mgm", "Amount of Upstream Material (Si equivalent);Beam Set Momentum [MeV/c];Length of Material [g/cm^{2}]");
  TMultiGraph* mgp = new TMultiGraph("mgp", "Momentum Change;Beam Set Momentum [MeV/c];Calculated Pre-SiT Momentum [MeV/c]");
  mg ->Add(g50,   "P");
  mg ->Add(g100,  "P");
  mg0->Add(G50,   "P");
  mg0->Add(G100,  "P");
  mge->Add(Ge50,  "P");
  mge->Add(Ge100, "P");
  mgm->Add(Gm50,  "P");
  mgm->Add(Gm100, "P");
  mgp->Add(Gp0vp50);
  mgp->Add(Gp0vp100);

  TLegend* leg50  = new TLegend(0.7, 0.7, 0.9, 0.9);
  TLegend* leg100 = new TLegend(0.7, 0.7, 0.9, 0.9);
  TLegend* leggr  = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg50 ->AddEntry(h50 [0], "25.9 MeV/c", "l");
  leg50 ->AddEntry(h50 [1], "26.2 MeV/c", "l");
  leg100->AddEntry(h100[0], "26.2 MeV/c", "l");
  leg100->AddEntry(h100[1], "26.3 MeV/c", "l");
  leg100->AddEntry(h100[2], "26.4 MeV/c", "l");
  leg100->AddEntry(h100[3], "26.5 MeV/c", "l");
  leg100->AddEntry(h100[4], "29.9 MeV/c", "l");
  leggr ->AddEntry(G50,     "Al50",       "p");
  leggr ->AddEntry(G100,    "Al100",      "p");

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1); for (auto h : h50)  h->Draw("SAME PLC"); leg50 ->Draw();
  c->cd(2); for (auto h : h100) h->Draw("SAME PLC"); leg100->Draw();
  c->SaveAs("img/sit_e_different_p_sit.png");
  TCanvas* cg = new TCanvas("cg", "cg", 1400, 1000);
  cg->Divide(2, 2);
  cg->cd(1); mg ->Draw("AP"); leggr->Draw();
  cg->cd(2); mg0->Draw("AP"); leggr->Draw();
  cg->cd(3); mge->Draw("AP"); leggr->Draw();
  cg->SaveAs("img/sit_e_different_p_betheprediction.png");
  TCanvas* cp = new TCanvas("cp", "cp", 1400, 500);
  cp->Divide(2);
  cp->cd(1); mgp->Draw("AP"); leggr->Draw();
  cp->cd(2); mgm->Draw("AP"); //leggr->Draw();
  cp->SaveAs("img/sit_e_different_p_upstreammaterial.png");
}
