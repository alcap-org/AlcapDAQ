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
                 hs[4][0]->GetNbinsX(),
                 hs[4][0]->GetXaxis()->GetXmin(),
                 hs[4][0]->GetXaxis()->GetXmax(),
                 "g"),
    new TProfile("hrmean", "hrmean",
                 hs[4][1]->GetNbinsX(),
                 hs[4][1]->GetXaxis()->GetXmin(),
                 hs[4][1]->GetXaxis()->GetXmax(),
                 "g")
  };
  if (hmean[0]->GetNbinsX()           != hmean[1]->GetNbinsX()           ||
      hmean[0]->GetXaxis()->GetXmin() != hmean[1]->GetXaxis()->GetXmin() ||
      hmean[0]->GetXaxis()->GetXmax() != hmean[1]->GetXaxis()->GetXmax()  )
    throw "HISTS DON'T MATCH";

  for (auto& H : hs) {
    for (int b = 1; b <= H[0]->GetNbinsX(); ++b) {
      hmean[0]->Fill(H[0]->GetBinCenter(b), H[0]->GetBinContent(b), H[0]->GetBinWidth(b)/100);
      hmean[1]->Fill(H[1]->GetBinCenter(b), H[1]->GetBinContent(b), H[1]->GetBinWidth(b)/100);
    }
  }

  vector<TH1*> herr = { (TH1*)hs[4][0]->Clone("hl"),
                        (TH1*)hs[4][1]->Clone("hr") };
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
  TFile* ofile = new TFile("data/Al50/corranderr/binwidth.root", "RECREATE");
  herr[0]->SetDirectory(ofile);
  herr[1]->SetDirectory(ofile);
  ofile->Write();
  ofile->Close();
  delete ofile;
}

void make_unfolded_spectra() {
  double pp =10e3, tcut = 400, dtcut = 200, elocut = 2e3, ehicut = 14e3, bayiter = 5;
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/binwidth_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2000keV.ehi14000keV.bay5.root", pp, tcut, dtcut, 1, elocut, ehicut, bayiter);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/binwidth_study/en.p.2layer.t400ns.dt200ns.rebin2.pp10us.elo2000keV.ehi14000keV.bay5.root", pp, tcut, dtcut, 2, elocut, ehicut, bayiter);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/binwidth_study/en.p.2layer.t400ns.dt200ns.rebin3.pp10us.elo2000keV.ehi14000keV.bay5.root", pp, tcut, dtcut, 3, elocut, ehicut, bayiter);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/binwidth_study/en.p.2layer.t400ns.dt200ns.rebin4.pp10us.elo2000keV.ehi14000keV.bay5.root", pp, tcut, dtcut, 4, elocut, ehicut, bayiter);
  R15b_Al50_Unfold('p', "data/Al50/tm.p.2layer.mudist.root", "data/Al50/psel.3layer.pidpow2.root", "data/Al50/binwidth_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi14000keV.bay5.root", pp, tcut, dtcut, 5, elocut, ehicut, bayiter);
}

void unfolding_binwidth(bool unfold_first=false) {
  if (unfold_first)
    make_unfolded_spectra();

  vector<TFile*> fs = {
    new TFile("data/Al50/binwidth_study/en.p.2layer.t400ns.dt200ns.rebin1.pp10us.elo2000keV.ehi14000keV.bayiter4.root"),
    new TFile("data/Al50/binwidth_study/en.p.2layer.t400ns.dt200ns.rebin2.pp10us.elo2000keV.ehi14000keV.bayiter4.root"),
    new TFile("data/Al50/binwidth_study/en.p.2layer.t400ns.dt200ns.rebin3.pp10us.elo2000keV.ehi14000keV.bayiter4.root"),
    new TFile("data/Al50/binwidth_study/en.p.2layer.t400ns.dt200ns.rebin4.pp10us.elo2000keV.ehi14000keV.bayiter4.root"),
    new TFile("data/Al50/binwidth_study/en.p.2layer.t400ns.dt200ns.rebin5.pp10us.elo2000keV.ehi14000keV.bayiter4.root")
  };
  const int NF = fs.size();


  vector<vector<TH1*>> hs;
  for (auto f : fs) {
    hs.push_back(vector<TH1*>{ (TH1*)f->Get("hl_e_reco_bay"),
                               (TH1*)f->Get("hr_e_reco_bay") });
  }

  // double norm = Normalization::Al50();
  double norm = 1;

  vector<vector<vector<double>>>
    n (NF, vector<vector<double>>(2, vector<double>(2))),
    en(NF, vector<vector<double>>(2, vector<double>(2)));
  hs[4][0]->SetTitle("Left;E [keV];Unfolded proton count/keV");
  hs[4][1]->SetTitle("Right;E [keV];Unfolded proton count/keV");
  for (int i = 0; i < NF; ++i) {
    for (int j = 0; j < 2; ++j) {
      hs[i][j]->Scale(1./norm);
      n[i][j][0] = hs[i][j]->IntegralAndError(hs[i][j]->FindFixBin(6e3),
                                              hs[i][j]->FindFixBin(12e3-1),
                                              en[i][j][0]);
      n[i][j][1] = hs[i][j]->IntegralAndError(hs[i][j]->FindFixBin(4e3),
                                              hs[i][j]->FindFixBin(8e3-1),
                                              en[i][j][1]);
      hs[i][j]->Scale(1./hs[i][j]->GetBinWidth(1));
    }
  }
  vector<double> x = {100., 200., 300., 400., 500.};
  vector<vector<double>> y(3, vector<double>(NF)), ey(2, vector<double>(NF));
  for (int i = 0; i < NF; ++i) {
    y[0][i] = n[i][0][0];
    y[1][i] = n[i][1][0];
    y[2][i] = (n[i][0][0]+n[i][1][0])*0.5;
    ey[0][i] = en[i][0][0];
    ey[1][i] = en[i][1][0];
  }
  vector<TGraphErrors*> grrates = {
    new TGraphErrors(NF, x.data(), y[0].data(), nullptr, ey[0].data()),
    new TGraphErrors(NF, x.data(), y[1].data(), nullptr, ey[1].data())
  };
  TMultiGraph* mgrates = new TMultiGraph("mgrates", "mgrates");
  mgrates->Add(grrates[0]);
  mgrates->Add(grrates[1]);
  grrates[0]->SetLineColor(ACStyle::kACBlue);
  grrates[1]->SetLineColor(ACStyle::kACRed);
  grrates[0]->SetTitle("SiL");
  grrates[1]->SetTitle("SiR");
  mgrates->SetTitle("6-12 MeV Proton Rates;Bin Width;Unfolded proton count");


  TLegend* leg = new TLegend(0.7, 0.4, 0.9, 0.9);
  leg->SetHeader("Bin Width");
  leg->AddEntry(hs[0][0], "100 keV");
  leg->AddEntry(hs[1][0], "200 keV");
  leg->AddEntry(hs[2][0], "300 keV");
  leg->AddEntry(hs[3][0], "400 keV");
  leg->AddEntry(hs[4][0], "500 keV");

  // hs[0][0]->GetYaxis()->SetRangeUser(0, 3e-3);
  // hs[0][1]->GetYaxis()->SetRangeUser(0, 3e-3);
  hs[4][0]->GetYaxis()->SetTitleOffset(1.6);
  hs[4][1]->GetYaxis()->SetTitleOffset(1.6);
  mgrates->GetYaxis()->SetTitleOffset(1.3);

  // hs[0][0]->Scale(5.);
  // hs[1][0]->Scale(5./2.);
  // hs[2][0]->Scale(5./3.);
  // hs[3][0]->Scale(5./4.);
  // hs[0][1]->Scale(5.);
  // hs[1][1]->Scale(5./2.);
  // hs[2][1]->Scale(5./3.);
  // hs[3][1]->Scale(5./4.);

  hs[4][0]->GetYaxis()->SetRangeUser(0, 200e3/500);
  hs[4][1]->GetYaxis()->SetRangeUser(0, 200e3/500);

  gStyle->SetOptStat(false);
  gStyle->SetPalette(kSolar);
  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1);
  hs[4][0] ->Draw("SAME PLC");
  hs[3][0] ->Draw("SAME PLC");
  hs[2][0] ->Draw("SAME PLC");
  hs[1][0] ->Draw("SAME PLC");
  hs[0][0] ->Draw("SAME PLC");
  leg     ->Draw();
  c->cd(2);
  hs[4][1] ->Draw("SAME PLC");
  hs[3][1] ->Draw("SAME PLC");
  hs[2][1] ->Draw("SAME PLC");
  hs[1][1] ->Draw("SAME PLC");
  hs[0][1] ->Draw("SAME PLC");
  leg     ->Draw();
  c->SaveAs("img/systematics/compare_unfolding_binwidth_spec.png");

  TLegend* lrates = new TLegend(0.50, 0.4, 0.65, 0.6);
  lrates->AddEntry(grrates[0], "SiL");
  lrates->AddEntry(grrates[1], "SiR");
  TCanvas* crates = new TCanvas("crates", "crates", 700, 500);
  mgrates->Draw("APL");
  lrates->Draw();
  crates->SaveAs("img/systematics/compare_unfolding_binwidth_rates.png");

  printf("Bin Width\t6-12 MeV\t\t\t4-8 MeV\n");
  printf("\t\tLeft\t\tRight\t\tLeft\t\tRight\n");
  printf("100 keV\t%g\t%g\t%g\t%g\n",   n[0][0][0],   n[0][1][0],   n[0][0][1],   n[0][1][1]);
  printf("200 keV\t%g\t%g\t%g\t%g\n",   n[1][0][0],   n[1][1][0],   n[1][0][1],   n[1][1][1]);
  printf("300 keV\t%g\t%g\t%g\t%g\n",   n[2][0][0],   n[2][1][0],   n[2][0][1],   n[2][1][1]);
  printf("400 keV\t%g\t%g\t%g\t%g\n",   n[3][0][0],   n[3][1][0],   n[3][0][1],   n[3][1][1]);
  printf("500 keV\t%g\t%g\t%g\t%g\n",   n[4][0][0],   n[4][1][0],   n[4][0][1],   n[4][1][1]);

  double ymean[3] = {
    std::accumulate(y[0].begin(), y[0].end(), 0.)/y[0].size(),
    std::accumulate(y[1].begin(), y[1].end(), 0.)/y[1].size(),
    std::accumulate(y[2].begin(), y[2].end(), 0.)/y[2].size()
  };
  double ysig[3]  = { 0, 0, 0 };
  for (int i = 0; i < NF; ++i) {
    ysig[0] += (y[0][i]-ymean[0])*(y[0][i]-ymean[0]);
    ysig[1] += (y[1][i]-ymean[1])*(y[1][i]-ymean[1]);
    ysig[2] += (y[2][i]-ymean[2])*(y[2][i]-ymean[2]);
  }
  ysig[0] /= NF - 1;
  ysig[1] /= NF - 1;
  ysig[2] /= NF - 1;
  ysig[0] = std::sqrt(ysig[0]);
  ysig[1] = std::sqrt(ysig[1]);
  ysig[2] = std::sqrt(ysig[2]);
  printf("Left Std. Dev:  %g (%g%%)\n", ysig[0], 100*ysig[0]/ymean[0]);
  printf("Right Std. Dev: %g (%g%%)\n", ysig[1], 100*ysig[1]/ymean[1]);
  printf("Avg Std. Dev:   %g (%g%%)\n", ysig[2], 100*ysig[2]/ymean[2]);

  SaveErrorsToFile(hs);

}
