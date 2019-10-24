#include "TBox.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TCut.h"
#include "TFile.h"
#include "TH2.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TTree.h"

#include <cstdio>

void mc_stop_ratio() {
  TChain* tral50 = new TChain("tree");
  TChain* trsi16 = new TChain("tree");
  tral50->Add("data/mc/R15bAl50MuPreSiT_*.root");
  trsi16->Add("data/mc/R15bSi16bMuPreSiT_*.root");
  // tral50->Add("data/mc/R15bAl50MuPreSiT_1.root");
  // trsi16->Add("data/mc/R15bSi16bMuPreSiT_1.root");
  TCut isstoppedmu("M_stopped==1 && M_volName==\"Target\" && M_particleName==\"mu-\"");
  TCut isdeadstrip("M_volID==0 || M_volID==15");
  double nal50all       = tral50->GetEntries();
  double nsi16all       = trsi16->GetEntries();
  double nal50stops     = tral50->GetEntries(isstoppedmu);
  double nsi16stops     = trsi16->GetEntries(isstoppedmu);
  double nsi16deadstops = trsi16->GetEntries(isstoppedmu && isdeadstrip);
  double ral50 = nal50stops/nal50all;
  double rsi16 = nsi16stops/nsi16all;

  std::printf("     \tAll\tStopped\tStop %%\n");
  std::printf("Al50 \t%d\t%d\t\t%g\n", (int)nal50all, (int)nal50stops, ral50);
  std::printf("Si16b\t%d\t%d\t\t%g\n", (int)nsi16all, (int)nsi16stops, rsi16);
  std::printf("Al50 stop increase:       %g\n", ral50/rsi16 - 1.);
  std::printf("Al50 background decrease: %g\n", (1.-ral50)/(1.-rsi16));
  std::printf("Fraction dead stops:      %g\n", double(nsi16deadstops)/nsi16stops);
}

void geometric_stops() {
  TFile* f = new TFile("data/Al50/tm.p.2layer.root");
  TTree* tr[2] = { (TTree*)f->Get("SiL_TransTree"), (TTree*)f->Get("SiR_TransTree") };
  TCut hitsi16all  = "-2.5 < y0 && y0 < 2.5 && -2.5   < x0 && x0 < 2.5";
  TCut hitsi16active = "-2.5 < y0 && y0 < 2.5 && -1.875 < x0 && x0 < 1.875";
  TCut ismeas      = "emeas>0";
  TH2* hposal50[2]           = { new TH2D("hposal50l",           "Al50 Hit Distribution;x [cm];local y [cm]",                                         100, -5., 5., 100, -5., 5.),
                                 new TH2D("hposal50r",           "Al50 Hit Distribution;x [cm];local y [cm]",                                         100, -5., 5., 100, -5., 5.) };
  TH2* hpossi16all[2]        = { new TH2D("hpossi16alll",        "Equivilent Si16b Hit Distribution;local x [cm];local y [cm]",                       100, -5., 5., 100, -5., 5.),
                                 new TH2D("hpossi16allr",        "Equivilent Si16b Hit Distribution;local x [cm];local y [cm]",                       100, -5., 5., 100, -5., 5.) };
  TH2* hpossi16active[2]     = { new TH2D("hpossi16activel",     "Equivilent Si16b Hit Distribution;local x [cm];local y [cm]",                       100, -5., 5., 100, -5., 5.),
                                 new TH2D("hpossi16activer",     "Equivilent Si16b Hit Distribution;local x [cm];local y [cm]",                       100, -5., 5., 100, -5., 5.) };
  TH2* hposal50meas[2]       = { new TH2D("hposal50measl",       "SiL Measured Hits (Al50);local x [cm];local y [cm]",                                100, -5., 5., 100, -5., 5.),
                                 new TH2D("hposal50measr",       "SiR Measured Hits (Al50);local x [cm];local y [cm]",                                100, -5., 5., 100, -5., 5.) };
  TH2* hpossi16allmeas[2]    = { new TH2D("hpossi16allmeasl",    "SiL Measured Hits (Al50 pared down to Si16b equivilent);local x [cm];local y [cm]", 100, -5., 5., 100, -5., 5.),
                                 new TH2D("hpossi16allmeasr",    "SiR Measured Hits (Al50 pared down to Si16b equivilent);local x [cm];local y [cm]", 100, -5., 5., 100, -5., 5.) };
  TH2* hpossi16activemeas[2] = { new TH2D("hpossi16activemeasl", "SiL Measured Hits (Al50 pared down to Si16b equivilent);local x [cm];local y [cm]", 100, -5., 5., 100, -5., 5.),
                                 new TH2D("hpossi16activemeasr", "SiR Measured Hits (Al50 pared down to Si16b equivilent);local x [cm];local y [cm]", 100, -5., 5., 100, -5., 5.) };
  tr[0]->Draw("y0:x0>>hposal50l",           "",                      "goff"); // This and the next plot are exactly the same
  tr[1]->Draw("y0:x0>>hposal50r",           "",                      "goff"); // presumably.
  tr[0]->Draw("y0:x0>>hpossi16alll",        hitsi16all,              "goff");
  tr[1]->Draw("y0:x0>>hpossi16allr",        hitsi16all,              "goff");
  tr[0]->Draw("y0:x0>>hpossi16activel",     hitsi16active,           "goff");
  tr[1]->Draw("y0:x0>>hpossi16activer",     hitsi16active,           "goff");
  tr[0]->Draw("y0:x0>>hposal50measl",       ismeas,                  "goff");
  tr[1]->Draw("y0:x0>>hposal50measr",       ismeas,                  "goff");
  tr[0]->Draw("y0:x0>>hpossi16allmeasl",    hitsi16all    && ismeas, "goff");
  tr[1]->Draw("y0:x0>>hpossi16allmeasr",    hitsi16all    && ismeas, "goff");
  tr[0]->Draw("y0:x0>>hpossi16activemeasl", hitsi16active && ismeas, "goff");
  tr[1]->Draw("y0:x0>>hpossi16activemeasr", hitsi16active && ismeas, "goff");
  double nal50[2]           = { hposal50[0]          ->GetEntries(), hposal50[1]          ->GetEntries() };
  double nsi16all[2]        = { hpossi16all[0]       ->GetEntries(), hpossi16all[1]       ->GetEntries() };
  double nsi16active[2]     = { hpossi16active[0]    ->GetEntries(), hpossi16active[1]    ->GetEntries() };
  double nal50meas[2]       = { hposal50meas[0]      ->GetEntries(), hposal50meas[1]      ->GetEntries() };
  double nsi16allmeas[2]    = { hpossi16allmeas[0]   ->GetEntries(), hpossi16allmeas[1]   ->GetEntries() };
  double nsi16activemeas[2] = { hpossi16activemeas[0]->GetEntries(), hpossi16activemeas[1]->GetEntries() };
  TBox* boxgood = new TBox(-2.5,   -2.5, 2.5,   2.5);
  TBox* boxmeas = new TBox(-1.875, -2.5, 1.875, 2.5);
  boxgood->SetFillStyle(0);
  boxmeas->SetFillStyle(0);
  boxgood->SetLineWidth(5);
  boxmeas->SetLineWidth(1);
  boxgood->SetLineColor(kRed);
  boxmeas->SetLineColor(kGreen);

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(boxgood, "Si16b (All 16 strips)",    "l");
  leg->AddEntry(boxmeas, "Si16b (Active 14 strips)", "l");

  hposal50meas[0]->SetMaximum(hposal50[0]->GetMaximum());
  hposal50meas[1]->SetMaximum(hposal50[1]->GetMaximum());

  gStyle->SetOptStat(0);
  TCanvas* call = new TCanvas("call", "call", 700, 500);
  hposal50[0]->Draw();
  boxgood->Draw("SAME");
  boxmeas->Draw("SAME");
  leg->Draw("SAME");
  call->SaveAs("img/al50_si16b_mu_stop_diff_all.png");

  TCanvas* cal50meas = new TCanvas("cal50meas", "cal50meas", 1400, 500);
  cal50meas->Divide(2);
  cal50meas->cd(1);
  hposal50meas[0]->Draw();
  boxgood->Draw("SAME");
  boxmeas->Draw("SAME");
  leg->Draw("SAME");
  cal50meas->cd(2);
  hposal50meas[1]->Draw();
  boxgood->Draw("SAME");
  boxmeas->Draw("SAME");
  leg->Draw("SAME");
  cal50meas->SaveAs("img/al50_si16b_mu_stop_diff_meas.png");

  printf("     NAl50Meas\tNSi16MeasAll\tNSi16MeasActive\n");
  printf("SiL: %g\t%g\t\t%g\n", nal50meas[0], nsi16allmeas[0], nsi16activemeas[0]);
  printf("SiR: %g\t%g\t\t%g\n", nal50meas[1], nsi16allmeas[1], nsi16activemeas[1]);
}


void al50_si16b_mu_stop_diff() {
  geometric_stops();
  // mc_stop_ratio();
}
