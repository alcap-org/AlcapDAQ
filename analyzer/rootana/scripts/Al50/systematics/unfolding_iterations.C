#include "./scripts/Al50/util.h"
#include "./scripts/Al50/R15b_Al50_Unfold.C"

#include "TCanvas.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TMultiGraph.h"
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
  return herr;
}

void SaveErrorsToFile(vector<vector<TH1*>> hs) {
  vector<TH1*> herr = make_average(hs);
  TFile* ofile = new TFile("data/Al50/corranderr/iterations.root", "RECREATE");
  herr[0]->SetDirectory(ofile);
  herr[1]->SetDirectory(ofile);
  ofile->Write();
  ofile->Close();
  delete ofile;
}

void make_unfolded_spectra() {
  double pp =10e3, tcut = 400, dtcut = 200, rebin = 5, elocut = 2e3, ehicut = 12e3;
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter1.root",   pp, tcut, dtcut, rebin, elocut, ehicut, 1);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter2.root",   pp, tcut, dtcut, rebin, elocut, ehicut, 2);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter3.root",   pp, tcut, dtcut, rebin, elocut, ehicut, 3);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter4.root",   pp, tcut, dtcut, rebin, elocut, ehicut, 4);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter5.root",   pp, tcut, dtcut, rebin, elocut, ehicut, 5);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter6.root",   pp, tcut, dtcut, rebin, elocut, ehicut, 6);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter7.root",   pp, tcut, dtcut, rebin, elocut, ehicut, 7);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter8.root",   pp, tcut, dtcut, rebin, elocut, ehicut, 8);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter9.root",   pp, tcut, dtcut, rebin, elocut, ehicut, 9);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter10.root",  pp, tcut, dtcut, rebin, elocut, ehicut, 10);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter15.root",  pp, tcut, dtcut, rebin, elocut, ehicut, 15);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter20.root",  pp, tcut, dtcut, rebin, elocut, ehicut, 20);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter25.root",  pp, tcut, dtcut, rebin, elocut, ehicut, 25);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter50.root",  pp, tcut, dtcut, rebin, elocut, ehicut, 50);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter75.root",  pp, tcut, dtcut, rebin, elocut, ehicut, 75);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter100.root", pp, tcut, dtcut, rebin, elocut, ehicut, 100);
}

void unfolding_iterations(bool unfold_first=false) {
  if (unfold_first)
    make_unfolded_spectra();

  vector<TFile*> fs = {
    new TFile("data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter1.root"),
    new TFile("data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter2.root"),
    new TFile("data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter3.root"),
    new TFile("data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter4.root"),
    new TFile("data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter5.root"),
    new TFile("data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter6.root"),
    new TFile("data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter7.root"),
    new TFile("data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter8.root"),
    new TFile("data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter9.root"),
    new TFile("data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter10.root"),
    new TFile("data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter15.root"),
    new TFile("data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter20.root"),
    new TFile("data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter25.root"),
    new TFile("data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter50.root"),
    new TFile("data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter75.root"),
    new TFile("data/Al50/bayiter_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi12000keV.bayiter100.root")
  };
  const int NF = fs.size();

  vector<vector<TH1*>> hs = {
    { (TH1*)fs[0] ->Get("hl_e_reco_bay"), (TH1*)fs[0] ->Get("hr_e_reco_bay") },
    { (TH1*)fs[1] ->Get("hl_e_reco_bay"), (TH1*)fs[1] ->Get("hr_e_reco_bay") },
    { (TH1*)fs[2] ->Get("hl_e_reco_bay"), (TH1*)fs[2] ->Get("hr_e_reco_bay") },
    { (TH1*)fs[3] ->Get("hl_e_reco_bay"), (TH1*)fs[3] ->Get("hr_e_reco_bay") },
    { (TH1*)fs[4] ->Get("hl_e_reco_bay"), (TH1*)fs[4] ->Get("hr_e_reco_bay") },
    { (TH1*)fs[5] ->Get("hl_e_reco_bay"), (TH1*)fs[5] ->Get("hr_e_reco_bay") },
    { (TH1*)fs[6] ->Get("hl_e_reco_bay"), (TH1*)fs[6] ->Get("hr_e_reco_bay") },
    { (TH1*)fs[7] ->Get("hl_e_reco_bay"), (TH1*)fs[7] ->Get("hr_e_reco_bay") },
    { (TH1*)fs[8] ->Get("hl_e_reco_bay"), (TH1*)fs[8] ->Get("hr_e_reco_bay") },
    { (TH1*)fs[9] ->Get("hl_e_reco_bay"), (TH1*)fs[9] ->Get("hr_e_reco_bay") },
    { (TH1*)fs[10]->Get("hl_e_reco_bay"), (TH1*)fs[10]->Get("hr_e_reco_bay") },
    { (TH1*)fs[11]->Get("hl_e_reco_bay"), (TH1*)fs[11]->Get("hr_e_reco_bay") },
    { (TH1*)fs[12]->Get("hl_e_reco_bay"), (TH1*)fs[12]->Get("hr_e_reco_bay") },
    { (TH1*)fs[13]->Get("hl_e_reco_bay"), (TH1*)fs[13]->Get("hr_e_reco_bay") },
    { (TH1*)fs[14]->Get("hl_e_reco_bay"), (TH1*)fs[14]->Get("hr_e_reco_bay") },
    { (TH1*)fs[15]->Get("hl_e_reco_bay"), (TH1*)fs[15]->Get("hr_e_reco_bay") }
  };

  double norm = 1;//Normalization::Al50();

  vector<vector<vector<double>>>
    n (NF, vector<vector<double>>(2, vector<double>(2))),
    en(NF, vector<vector<double>>(2, vector<double>(2)));
  for (int i = 0; i < NF; ++i) {
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
  vector<double> x = {1., 2.,  3.,  4.,  5.,  6.,  7.,  8.,
                      9., 10., 15., 20., 25., 50., 75., 100.};
  vector<vector<double>> y(3, vector<double>(NF)), ey(2, vector<double>(NF));
  for (int i = 0; i < NF; ++i) {
    y[0][i] = n[i][0][0];
    y[1][i] = n[i][1][0];
    y[2][i] = (n[i][0][0]+n[i][1][0])*0.5;
    ey[0][i] = en[i][0][0];
    ey[1][i] = en[i][1][0];
  }
  TGraphErrors* grrates[2] = {
    new TGraphErrors(NF, x.data(), y[0].data(), nullptr, ey[0].data()),
    new TGraphErrors(NF, x.data(), y[1].data(), nullptr, ey[1].data())
  };
  TMultiGraph* mgrates = new TMultiGraph("mgrates", "mgrates");
  mgrates->Add(grrates[0]);
  mgrates->Add(grrates[1]);
  grrates[0]->SetLineColor(kBlue);
  grrates[1]->SetLineColor(kRed);
  grrates[0]->SetTitle("SiL");
  grrates[1]->SetTitle("SiR");
  mgrates->SetTitle("3.5-10 MeV Proton Rates;Unfolding Iterations;Unfolded proton count");


  TLegend* leg = new TLegend(0.7, 0.4, 0.9, 0.9);
  leg->SetHeader("Unfolding Iterations");
  leg->AddEntry(hs[0][0],  "1");
  leg->AddEntry(hs[1][0],  "2");
  leg->AddEntry(hs[2][0],  "3");
  leg->AddEntry(hs[3][0],  "4");
  leg->AddEntry(hs[4][0],  "5");
  leg->AddEntry(hs[5][0],  "6");
  leg->AddEntry(hs[6][0],  "7");
  leg->AddEntry(hs[7][0],  "8");
  leg->AddEntry(hs[8][0],  "9");
  leg->AddEntry(hs[9][0],  "10");
  leg->AddEntry(hs[10][0], "15");
  leg->AddEntry(hs[11][0], "20");
  leg->AddEntry(hs[12][0], "25");
  leg->AddEntry(hs[13][0], "50");
  leg->AddEntry(hs[14][0], "75");
  leg->AddEntry(hs[15][0], "100");

  // hs[0][0]->GetYaxis()->SetRangeUser(0, 3e-3);
  // hs[0][1]->GetYaxis()->SetRangeUser(0, 3e-3);
  hs[0][0]->GetYaxis()->SetTitleOffset(1.6);
  hs[0][1]->GetYaxis()->SetTitleOffset(1.6);
  mgrates->GetYaxis()->SetTitleOffset(1.6);

  gStyle->SetOptStat(false);
  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1);
  hs[0][0] ->Draw("SAME PLC");
  hs[1][0] ->Draw("SAME PLC");
  hs[2][0] ->Draw("SAME PLC");
  hs[3][0] ->Draw("SAME PLC");
  hs[4][0] ->Draw("SAME PLC");
  hs[5][0] ->Draw("SAME PLC");
  hs[6][0] ->Draw("SAME PLC");
  hs[7][0] ->Draw("SAME PLC");
  hs[8][0] ->Draw("SAME PLC");
  hs[9][0] ->Draw("SAME PLC");
  hs[10][0]->Draw("SAME PLC");
  hs[11][0]->Draw("SAME PLC");
  hs[12][0]->Draw("SAME PLC");
  hs[13][0]->Draw("SAME PLC");
  hs[14][0]->Draw("SAME PLC");
  hs[15][0]->Draw("SAME PLC");
  leg     ->Draw();
  c->cd(2);
  hs[0][1] ->Draw("SAME PLC");
  hs[1][1] ->Draw("SAME PLC");
  hs[2][1] ->Draw("SAME PLC");
  hs[3][1] ->Draw("SAME PLC");
  hs[4][1] ->Draw("SAME PLC");
  hs[5][1] ->Draw("SAME PLC");
  hs[6][1] ->Draw("SAME PLC");
  hs[7][1] ->Draw("SAME PLC");
  hs[8][1] ->Draw("SAME PLC");
  hs[9][1] ->Draw("SAME PLC");
  hs[10][1]->Draw("SAME PLC");
  hs[11][1]->Draw("SAME PLC");
  hs[12][1]->Draw("SAME PLC");
  hs[13][1]->Draw("SAME PLC");
  hs[14][1]->Draw("SAME PLC");
  hs[15][1]->Draw("SAME PLC");
  leg     ->Draw();
  c->SaveAs("img/systematics/compare_unfolding_bayiter_spec.png");

  TLegend* lrates = new TLegend(0.55, 0.4, 0.65, 0.6);
  lrates->AddEntry(grrates[0], "SiL");
  lrates->AddEntry(grrates[1], "SiR");
  TCanvas* crates = new TCanvas("crates", "crates", 700, 500);
  mgrates->Draw("APL");
  lrates->Draw();
  crates->SaveAs("img/systematics/compare_unfolding_bayiter_rates.png");

  printf("Unfold iterations\t3.5-10 MeV\t\t\t4-8 MeV\n");
  printf("\tLeft\t\tRight\t\tLeft\t\tRight\n");
  printf("1\t%g\t%g\t%g\t%g\n",   n[0][0][0],   n[0][1][0],   n[0][0][1],   n[0][1][1]);
  printf("2\t%g\t%g\t%g\t%g\n",   n[1][0][0],   n[1][1][0],   n[1][0][1],   n[1][1][1]);
  printf("3\t%g\t%g\t%g\t%g\n",   n[2][0][0],   n[2][1][0],   n[2][0][1],   n[2][1][1]);
  printf("4\t%g\t%g\t%g\t%g\n",   n[3][0][0],   n[3][1][0],   n[3][0][1],   n[3][1][1]);
  printf("5\t%g\t%g\t%g\t%g\n",   n[4][0][0],   n[4][1][0],   n[4][0][1],   n[4][1][1]);
  printf("6\t%g\t%g\t%g\t%g\n",   n[5][0][0],   n[5][1][0],   n[5][0][1],   n[5][1][1]);
  printf("7\t%g\t%g\t%g\t%g\n",   n[6][0][0],   n[6][1][0],   n[6][0][1],   n[6][1][1]);
  printf("8\t%g\t%g\t%g\t%g\n",   n[7][0][0],   n[7][1][0],   n[7][0][1],   n[7][1][1]);
  printf("9\t%g\t%g\t%g\t%g\n",   n[8][0][0],   n[8][1][0],   n[8][0][1],   n[8][1][1]);
  printf("10\t%g\t%g\t%g\t%g\n",  n[9][0][0],   n[9][1][0],   n[9][0][1],   n[9][1][1]);
  printf("15\t%g\t%g\t%g\t%g\n",  n[10][0][0],  n[10][1][0],  n[10][0][1],  n[10][1][1]);
  printf("20\t%g\t%g\t%g\t%g\n",  n[11][0][0],  n[11][1][0],  n[11][0][1],  n[11][1][1]);
  printf("25\t%g\t%g\t%g\t%g\n",  n[12][0][0],  n[12][1][0],  n[12][0][1],  n[12][1][1]);
  printf("50\t%g\t%g\t%g\t%g\n",  n[13][0][0],  n[13][1][0],  n[13][0][1],  n[13][1][1]);
  printf("75\t%g\t%g\t%g\t%g\n",  n[14][0][0],  n[14][1][0],  n[14][0][1],  n[14][1][1]);
  printf("100\t%g\t%g\t%g\t%g\n", n[15][0][0],  n[15][1][0],  n[15][0][1],  n[15][1][1]);

  double ymean[3] = {
    std::accumulate(y[0].begin(), y[0].end(), 0.)/NF,
    std::accumulate(y[1].begin(), y[1].end(), 0.)/NF,
    std::accumulate(y[2].begin(), y[2].end(), 0.)/NF
  };
  // double ysig[3]  = { 0, 0, 0 };
  // for (int i = 0; i < NF; ++i) {
  //   ysig[0] += (y[0][i]-ymean[0])*(y[0][i]-ymean[0]);
  //   ysig[1] += (y[1][i]-ymean[1])*(y[1][i]-ymean[1]);
  //   ysig[2] += (y[2][i]-ymean[2])*(y[2][i]-ymean[2]);
  // }
  // ysig[0] /= NF - 1;
  // ysig[1] /= NF - 1;
  // ysig[2] /= NF - 1;
  // ysig[0] = std::sqrt(ysig[0]);
  // ysig[1] = std::sqrt(ysig[1]);
  // ysig[2] = std::sqrt(ysig[2]);
  // printf("Left Std. Dev:  %g (%g%%)\n", ysig[0], 100*ysig[0]/ymean[0]);
  // printf("Right Std. Dev: %g (%g%%)\n", ysig[1], 100*ysig[1]/ymean[1]);
  // printf("Avg Std. Dev:   %g (%g%%)\n", ysig[2], 100*ysig[2]/ymean[2]);

  double dy[3] = {
    y[0][3] - ymean[0],
    y[1][3] - ymean[1],
    y[2][3] - ymean[2]
  };
  printf("Steady state difference of left: %g (%g%%)\n",
        dy[0], 100*dy[0]/ymean[0]);
  printf("Steady state difference of right: %g (%g%%)\n",
        dy[1], 100*dy[1]/ymean[1]);
  printf("Steady state difference of average: %g (%g%%)\n",
        dy[2], 100*dy[2]/ymean[2]);


  SaveErrorsToFile(hs);

}
