#include "util.h"
#include "R15b_Al50_Unfold.C"

#include "TCanvas.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "TProfile.h"
#include "TStyle.h"

#include <cmath>
#include <cstdio>
#include <numeric>
#include <vector>

using std::printf;
using std::vector;

vector<TH1*> make_average(vector<vector<TH1*>> hs) { //[Number of files][l/r]
  vector<TProfile*> hmean = {
    new TProfile("hlmean", "hlmean",
                 hs[0][0]->GetNbinsX(),
                 hs[0][0]->GetXaxis()->GetXmin(),
                 hs[0][0]->GetXaxis()->GetXmax(),
                 "s"),
    new TProfile("hrmean", "hrmean",
                 hs[0][1]->GetNbinsX(),
                 hs[0][1]->GetXaxis()->GetXmin(),
                 hs[0][1]->GetXaxis()->GetXmax(),
                 "s")
  };
  if (hmean[0]->GetNbinsX()           != hmean[1]->GetNbinsX()           ||
      hmean[0]->GetXaxis()->GetXmin() != hmean[1]->GetXaxis()->GetXmin() ||
      hmean[0]->GetXaxis()->GetXmax() != hmean[1]->GetXaxis()->GetXmax()  )
    throw "HISTS DON'T MATCH";

  for (int i = 0; i < hs.size(); ++i) {
    for (int b = 1; b <= hs[i][0]->GetNbinsX(); ++b) {
      hmean[0]->Fill(hs[i][0]->GetBinCenter(b), hs[i][0]->GetBinContent(b));
      hmean[1]->Fill(hs[i][1]->GetBinCenter(b), hs[i][1]->GetBinContent(b));
    }
  }

  vector<TH1*> herr = { (TH1*)hs[0][0]->Clone("hl"),
                        (TH1*)hs[0][1]->Clone("hr") };
  herr[0]->Reset();
  herr[1]->Reset();
  herr[0]->GetXaxis()->UnZoom(); herr[0]->GetYaxis()->UnZoom();
  herr[1]->GetXaxis()->UnZoom(); herr[1]->GetYaxis()->UnZoom();
  herr[0]->ResetAttLine();
  herr[1]->ResetAttLine();
  for (int b = 1; b <= herr[0]->GetNbinsX(); ++b) {
    herr[0]->SetBinContent(b, 1);
    herr[1]->SetBinContent(b, 1);
    herr[0]->SetBinError(b, hmean[0]->GetBinError(b)/hmean[0]->GetBinContent(b));
    herr[1]->SetBinError(b, hmean[1]->GetBinError(b)/hmean[1]->GetBinContent(b));
  }
  // The first bin won't have an "error" since there's only one entry
  // in the profile
  herr[0]->SetBinError(1, herr[0]->GetBinError(2));
  herr[1]->SetBinError(1, herr[1]->GetBinError(2));
  return herr;
}

void SaveErrorsToFile(vector<vector<TH1*>> hs) {
  vector<TH1*> herr = make_average(hs);
  TFile* ofile = new TFile("data/Al50/corranderr/cutoff.root", "RECREATE");
  herr[0]->SetDirectory(ofile);
  herr[1]->SetDirectory(ofile);
  ofile->Write();
  ofile->Close();
  delete ofile;
}

void make_unfolded_spectra() {
  double pp =10e3, tcut = 400, dtcut = 200, rebin = 1, ehicut = 12e3, bayiter = 4;
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2000keV.ehi12000keV.root", pp, tcut, dtcut, rebin, 2.0e3, ehicut, bayiter);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2100keV.ehi12000keV.root", pp, tcut, dtcut, rebin, 2.1e3, ehicut, bayiter);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2200keV.ehi12000keV.root", pp, tcut, dtcut, rebin, 2.2e3, ehicut, bayiter);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2300keV.ehi12000keV.root", pp, tcut, dtcut, rebin, 2.3e3, ehicut, bayiter);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2400keV.ehi12000keV.root", pp, tcut, dtcut, rebin, 2.4e3, ehicut, bayiter);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2500keV.ehi12000keV.root", pp, tcut, dtcut, rebin, 2.5e3, ehicut, bayiter);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2600keV.ehi12000keV.root", pp, tcut, dtcut, rebin, 2.6e3, ehicut, bayiter);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2700keV.ehi12000keV.root", pp, tcut, dtcut, rebin, 2.7e3, ehicut, bayiter);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2800keV.ehi12000keV.root", pp, tcut, dtcut, rebin, 2.8e3, ehicut, bayiter);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2900keV.ehi12000keV.root", pp, tcut, dtcut, rebin, 2.9e3, ehicut, bayiter);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo3000keV.ehi12000keV.root", pp, tcut, dtcut, rebin, 3.0e3, ehicut, bayiter);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo3100keV.ehi12000keV.root", pp, tcut, dtcut, rebin, 3.1e3, ehicut, bayiter);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo3200keV.ehi12000keV.root", pp, tcut, dtcut, rebin, 3.2e3, ehicut, bayiter);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo3300keV.ehi12000keV.root", pp, tcut, dtcut, rebin, 3.3e3, ehicut, bayiter);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo3400keV.ehi12000keV.root", pp, tcut, dtcut, rebin, 3.4e3, ehicut, bayiter);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo3500keV.ehi12000keV.root", pp, tcut, dtcut, rebin, 3.5e3, ehicut, bayiter);
}

void compare_unfolding_cutoff(bool unfold_first=false) {
  if (unfold_first)
    make_unfolded_spectra();

  // vector<TFile*> fs = {
  //   new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2000keV.ehi12000keV.root"),
  //   new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2100keV.ehi12000keV.root"),
  //   new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2200keV.ehi12000keV.root"),
  //   new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2300keV.ehi12000keV.root"),
  //   new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2400keV.ehi12000keV.root"),
  //   new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2500keV.ehi12000keV.root"),
  //   new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2600keV.ehi12000keV.root"),
  //   new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2700keV.ehi12000keV.root"),
  //   new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2800keV.ehi12000keV.root"),
  //   new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2900keV.ehi12000keV.root"),
  //   new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo3000keV.ehi12000keV.root"),
  //   new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo3100keV.ehi12000keV.root"),
  //   new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo3200keV.ehi12000keV.root"),
  //   new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo3300keV.ehi12000keV.root"),
  //   new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo3400keV.ehi12000keV.root"),
  //   new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo3500keV.ehi12000keV.root")
  // };

  vector<TFile*> fs = {
    new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bay5.root"),
    new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2500keV.ehi12000keV.bay5.root"),
    new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo3000keV.ehi12000keV.bay5.root"),
    new TFile("data/Al50/elo_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo3500keV.ehi12000keV.bay5.root")
  };
  const int N = fs.size();

  vector<vector<TH1*>> hs;
  for (auto f : fs) {
    hs.push_back(vector<TH1*>{ (TH1*)f->Get("hl_e_reco_bay"),
                               (TH1*)f->Get("hr_e_reco_bay") });
  }

  // vector<vector<TH1*>> hs = {
  //   { (TH1*)fs[0] ->Get("hl_e_reco_bay"), (TH1*)fs[0] ->Get("hr_e_reco_bay") },
  //   { (TH1*)fs[1] ->Get("hl_e_reco_bay"), (TH1*)fs[1] ->Get("hr_e_reco_bay") },
  //   { (TH1*)fs[2] ->Get("hl_e_reco_bay"), (TH1*)fs[2] ->Get("hr_e_reco_bay") },
  //   { (TH1*)fs[3] ->Get("hl_e_reco_bay"), (TH1*)fs[3] ->Get("hr_e_reco_bay") },
  //   { (TH1*)fs[4] ->Get("hl_e_reco_bay"), (TH1*)fs[4] ->Get("hr_e_reco_bay") },
  //   { (TH1*)fs[5] ->Get("hl_e_reco_bay"), (TH1*)fs[5] ->Get("hr_e_reco_bay") },
  //   { (TH1*)fs[6] ->Get("hl_e_reco_bay"), (TH1*)fs[6] ->Get("hr_e_reco_bay") },
  //   { (TH1*)fs[7] ->Get("hl_e_reco_bay"), (TH1*)fs[7] ->Get("hr_e_reco_bay") },
  //   { (TH1*)fs[8] ->Get("hl_e_reco_bay"), (TH1*)fs[8] ->Get("hr_e_reco_bay") },
  //   { (TH1*)fs[9] ->Get("hl_e_reco_bay"), (TH1*)fs[9] ->Get("hr_e_reco_bay") },
  //   { (TH1*)fs[10]->Get("hl_e_reco_bay"), (TH1*)fs[10]->Get("hr_e_reco_bay") },
  //   { (TH1*)fs[11]->Get("hl_e_reco_bay"), (TH1*)fs[11]->Get("hr_e_reco_bay") },
  //   { (TH1*)fs[12]->Get("hl_e_reco_bay"), (TH1*)fs[12]->Get("hr_e_reco_bay") },
  //   { (TH1*)fs[13]->Get("hl_e_reco_bay"), (TH1*)fs[13]->Get("hr_e_reco_bay") },
  //   { (TH1*)fs[14]->Get("hl_e_reco_bay"), (TH1*)fs[14]->Get("hr_e_reco_bay") },
  //   { (TH1*)fs[15]->Get("hl_e_reco_bay"), (TH1*)fs[15]->Get("hr_e_reco_bay") }
  // };

  double norm = 1;//Normalization::Al50();

  vector<vector<vector<double>>>
    n (N, vector<vector<double>>(2, vector<double>(2))),
    en(N, vector<vector<double>>(2, vector<double>(2)));
  for (int i = 0; i < N; ++i) {
    // hs[i][0]->SetTitle("Left;E [keV];Unfolded proton count/100 keV");
    // hs[i][1]->SetTitle("Right;E [keV];Unfolded proton count/100 keV");
    hs[i][0]->SetTitle("Left;E [keV];Unfolded proton count/500 keV");
    hs[i][1]->SetTitle("Right;E [keV];Unfolded proton count/500 keV");
    for (int j = 0; j < 2; ++j) {
      hs[i][j]->Scale(1./norm);
      n[i][j][0] = hs[i][j]->IntegralAndError(hs[i][j]->FindFixBin(3.5e3),
                                              hs[i][j]->FindFixBin(10e3-1),
                                              en[i][j][0]);
      n[i][j][1] = hs[i][j]->IntegralAndError(hs[i][j]->FindFixBin(4e3),
                                              hs[i][j]->FindFixBin(8e3-1),
                                              en[i][j][1]);
    }
  }

  vector<vector<double>> x(2, vector<double>(N)),
    y(3, vector<double>(N)), ey(2, vector<double>(N));
  for (int i = 0; i < N; ++i) {
    x[0][i] = x[1][i]  = 2.+i*0.1;
    y[0][i] = n[i][0][0];
    y[1][i] = n[i][1][0];
    y[2][i] = (n[i][0][0]+n[i][1][0])*0.5;
    ey[0][i] = en[i][0][0];
    ey[1][i] = en[i][1][0];
  }
  TGraphErrors* grrates[2] = {
    new TGraphErrors(N, x[0].data(), y[0].data(), nullptr, ey[0].data()),
    new TGraphErrors(N, x[1].data(), y[1].data(), nullptr, ey[1].data())
  };
  TMultiGraph* mgrates = new TMultiGraph("mgrates", "mgrates");
  mgrates->Add(grrates[0]);
  mgrates->Add(grrates[1]);
  grrates[0]->SetLineColor(ACStyle::kACBlue);
  grrates[1]->SetLineColor(ACStyle::kACRed);
  grrates[0]->SetTitle("SiL");
  grrates[1]->SetTitle("SiR");
  mgrates->SetTitle("3.5-10 MeV Proton Rates;Lower Unfolding Bound [keV];Unfolded proton count");


  // TLegend* leg = new TLegend(0.7, 0.25, 0.9, 0.9);
  TLegend* leg = new TLegend(0.6, 0.6, 0.9, 0.9);
  leg->SetHeader("Unfolding Range");
  // leg->AddEntry(hs[0][0],  "2.0-12 MeV");
  // leg->AddEntry(hs[1][0],  "2.1-12 MeV");
  // leg->AddEntry(hs[2][0],  "2.2-12 MeV");
  // leg->AddEntry(hs[3][0],  "2.3-12 MeV");
  // leg->AddEntry(hs[4][0],  "2.4-12 MeV");
  // leg->AddEntry(hs[5][0],  "2.5-12 MeV");
  // leg->AddEntry(hs[6][0],  "2.6-12 MeV");
  // leg->AddEntry(hs[7][0],  "2.7-12 MeV");
  // leg->AddEntry(hs[8][0],  "2.8-12 MeV");
  // leg->AddEntry(hs[9][0],  "2.9-12 MeV");
  // leg->AddEntry(hs[10][0], "3.0-12 MeV");
  // leg->AddEntry(hs[11][0], "3.1-12 MeV");
  // leg->AddEntry(hs[12][0], "3.2-12 MeV");
  // leg->AddEntry(hs[13][0], "3.3-12 MeV");
  // leg->AddEntry(hs[14][0], "3.4-12 MeV");
  // leg->AddEntry(hs[15][0], "3.5-12 MeV");
  leg->AddEntry(hs[0][0],  "2.0-12 MeV");
  leg->AddEntry(hs[1][0],  "2.5-12 MeV");
  leg->AddEntry(hs[2][0],  "3.0-12 MeV");
  leg->AddEntry(hs[3][0],  "3.5-12 MeV");


  // hs[0][0]->GetYaxis()->SetRangeUser(0, 200e3);
  // hs[0][1]->GetYaxis()->SetRangeUser(0, 200e3);
  hs[0][0]->GetYaxis()->SetRangeUser(0, 220e3);
  hs[0][1]->GetYaxis()->SetRangeUser(0, 220e3);
  hs[0][0]->GetYaxis()->SetTitleOffset(1.6);
  hs[0][1]->GetYaxis()->SetTitleOffset(1.6);
  mgrates->GetYaxis()->SetTitleOffset(1.6);

  gStyle->SetOptStat(false);
  gStyle->SetPalette(kSolar);
  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1);
  hs[0][0] ->DrawClone("SAME PLC");
  hs[1][0] ->Draw("SAME PLC");
  hs[2][0] ->Draw("SAME PLC");
  hs[3][0] ->Draw("SAME PLC");
  // hs[4][0] ->Draw("SAME PLC");
  // hs[5][0] ->Draw("SAME PLC");
  // hs[6][0] ->Draw("SAME PLC");
  // hs[7][0] ->Draw("SAME PLC");
  // hs[8][0] ->Draw("SAME PLC");
  // hs[9][0] ->Draw("SAME PLC");
  // hs[10][0]->Draw("SAME PLC");
  // hs[11][0]->Draw("SAME PLC");
  // hs[12][0]->Draw("SAME PLC");
  // hs[13][0]->Draw("SAME PLC");
  // hs[14][0]->Draw("SAME PLC");
  // hs[15][0]->Draw("SAME PLC");
  leg     ->Draw();
  c->cd(2);
  hs[0][1] ->DrawClone("SAME PLC");
  hs[1][1] ->Draw("SAME PLC");
  hs[2][1] ->Draw("SAME PLC");
  hs[3][1] ->Draw("SAME PLC");
  // hs[4][1] ->Draw("SAME PLC");
  // hs[5][1] ->Draw("SAME PLC");
  // hs[6][1] ->Draw("SAME PLC");
  // hs[7][1] ->Draw("SAME PLC");
  // hs[8][1] ->Draw("SAME PLC");
  // hs[9][1] ->Draw("SAME PLC");
  // hs[10][1]->Draw("SAME PLC");
  // hs[11][1]->Draw("SAME PLC");
  // hs[12][1]->Draw("SAME PLC");
  // hs[13][1]->Draw("SAME PLC");
  // hs[14][1]->Draw("SAME PLC");
  // hs[15][1]->Draw("SAME PLC");
  leg     ->Draw();
  c->SaveAs("img/compare_unfolding_cutoff_elo_spec.png");

  hs[0][0]->GetXaxis()->SetRangeUser(2e3, 5e3);
  hs[0][1]->GetXaxis()->SetRangeUser(2e3, 5e3);
  hs[0][0]->SetTitle("Left (Zoom)");
  hs[0][1]->SetTitle("Right (Zoom)");
  TCanvas* cspeczoom = new TCanvas("cspeczoom", "cspeczoom", 1400, 500);
  cspeczoom->Divide(2);
  cspeczoom->cd(1);
  hs[0][0] ->Draw("SAME PLC");
  hs[1][0] ->Draw("SAME PLC");
  hs[2][0] ->Draw("SAME PLC");
  hs[3][0] ->Draw("SAME PLC");
  // hs[4][0] ->Draw("SAME PLC");
  // hs[5][0] ->Draw("SAME PLC");
  // hs[6][0] ->Draw("SAME PLC");
  // hs[7][0] ->Draw("SAME PLC");
  // hs[8][0] ->Draw("SAME PLC");
  // hs[9][0] ->Draw("SAME PLC");
  // hs[10][0]->Draw("SAME PLC");
  // hs[11][0]->Draw("SAME PLC");
  // hs[12][0]->Draw("SAME PLC");
  // hs[13][0]->Draw("SAME PLC");
  // hs[14][0]->Draw("SAME PLC");
  // hs[15][0]->Draw("SAME PLC");
  cspeczoom->cd(2);
  hs[0][1] ->Draw("SAME PLC");
  hs[1][1] ->Draw("SAME PLC");
  hs[2][1] ->Draw("SAME PLC");
  hs[3][1] ->Draw("SAME PLC");
  // hs[4][1] ->Draw("SAME PLC");
  // hs[5][1] ->Draw("SAME PLC");
  // hs[6][1] ->Draw("SAME PLC");
  // hs[7][1] ->Draw("SAME PLC");
  // hs[8][1] ->Draw("SAME PLC");
  // hs[9][1] ->Draw("SAME PLC");
  // hs[10][1]->Draw("SAME PLC");
  // hs[11][1]->Draw("SAME PLC");
  // hs[12][1]->Draw("SAME PLC");
  // hs[13][1]->Draw("SAME PLC");
  // hs[14][1]->Draw("SAME PLC");
  // hs[15][1]->Draw("SAME PLC");
  cspeczoom->SaveAs("img/compare_unfolding_cutoff_elo_spec_zoom.png");

  TCanvas* crates = new TCanvas("crates", "crates", 700, 500);
  mgrates->Draw("APL");
  crates->BuildLegend();
  crates->SaveAs("img/compare_unfolding_cutoff_elo_rates.png");

  printf("Unfold range\t3.5-10 MeV\t\t\t4-8 MeV\n");
  printf("\t\tLeft\t\tRight\t\tLeft\t\tRight\n");
  // printf("2.0-12 MeV\t%g\t%g\t%g\t%g\n", n[0][0][0],  n[0][1][0],  n[0][0][1],  n[0][1][1]);
  // printf("2.1-12 MeV\t%g\t%g\t%g\t%g\n", n[1][0][0],  n[1][1][0],  n[1][0][1],  n[1][1][1]);
  // printf("2.2-12 MeV\t%g\t%g\t%g\t%g\n", n[2][0][0],  n[2][1][0],  n[2][0][1],  n[2][1][1]);
  // printf("2.3-12 MeV\t%g\t%g\t%g\t%g\n", n[3][0][0],  n[3][1][0],  n[3][0][1],  n[3][1][1]);
  // printf("2.4-12 MeV\t%g\t%g\t%g\t%g\n", n[4][0][0],  n[4][1][0],  n[4][0][1],  n[4][1][1]);
  // printf("2.5-12 MeV\t%g\t%g\t%g\t%g\n", n[5][0][0],  n[5][1][0],  n[5][0][1],  n[5][1][1]);
  // printf("2.6-12 MeV\t%g\t%g\t%g\t%g\n", n[6][0][0],  n[6][1][0],  n[6][0][1],  n[6][1][1]);
  // printf("2.7-12 MeV\t%g\t%g\t%g\t%g\n", n[7][0][0],  n[7][1][0],  n[7][0][1],  n[7][1][1]);
  // printf("2.8-12 MeV\t%g\t%g\t%g\t%g\n", n[8][0][0],  n[8][1][0],  n[8][0][1],  n[8][1][1]);
  // printf("2.9-12 MeV\t%g\t%g\t%g\t%g\n", n[9][0][0],  n[9][1][0],  n[9][0][1],  n[9][1][1]);
  // printf("3.0-12 MeV\t%g\t%g\t%g\t%g\n", n[10][0][0], n[10][1][0], n[10][0][1], n[10][1][1]);
  // printf("3.1-12 MeV\t%g\t%g\t%g\t%g\n", n[11][0][0], n[11][1][0], n[11][0][1], n[11][1][1]);
  // printf("3.2-12 MeV\t%g\t%g\t%g\t%g\n", n[12][0][0], n[12][1][0], n[12][0][1], n[12][1][1]);
  // printf("3.3-12 MeV\t%g\t%g\t%g\t%g\n", n[13][0][0], n[13][1][0], n[13][0][1], n[13][1][1]);
  // printf("3.4-12 MeV\t%g\t%g\t%g\t%g\n", n[14][0][0], n[14][1][0], n[14][0][1], n[14][1][1]);
  // printf("3.5-12 MeV\t%g\t%g\t%g\t%g\n", n[15][0][0], n[15][1][0], n[15][0][1], n[15][1][1]);
  printf("2.0-12 MeV\t%g\t%g\t%g\t%g\n", n[0][0][0],  n[0][1][0],  n[0][0][1],  n[0][1][1]);
  printf("2.5-12 MeV\t%g\t%g\t%g\t%g\n", n[1][0][0],  n[1][1][0],  n[1][0][1],  n[1][1][1]);
  printf("3.0-12 MeV\t%g\t%g\t%g\t%g\n", n[2][0][0],  n[2][1][0],  n[2][0][1],  n[2][1][1]);
  printf("3.5-12 MeV\t%g\t%g\t%g\t%g\n", n[3][0][0],  n[3][1][0],  n[3][0][1],  n[3][1][1]);

  double ymean = std::accumulate(y[2].begin(), y[2].end(), 0.)/N;
  double ysig  = 0;
  for (int i = 0; i < N; ++i)
    ysig += (y[2][i]-ymean)*(y[2][i]-ymean);
  ysig /= N - 1;
  ysig = std::sqrt(ysig);
  printf("Avg Std. Dev: %g (%g%%)\n", ysig, 100*ysig/ymean);

  SaveErrorsToFile(hs);

}
