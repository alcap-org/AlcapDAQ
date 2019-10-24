#include "./scripts/Al50/util.h"

#include "RooUnfoldBayes.h"
#include "RooUnfoldResponse.h"

#include "TCanvas.h"
#include "TColor.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "TRandom3.h"
#include "TStyle.h"
#include "TTree.h"

#include <cstdio>
#include <iostream>
#include <utility>
#include <vector>
using std::pair;
using std::vector;

void FillMeasuredHists(RooUnfoldResponse* r[2], TTree* tr[2],
                       TH1*& hle, TH1*& hre, double pp, double t, double dt) {
  const TAxis* ax[2] = { r[0]->Htruth()->GetXaxis(),
                         r[1]->Htruth()->GetXaxis() };
  char cmd[2][64];
  char cut[64];
  std::sprintf(cmd[0], "e1+e2>>hle(%d,%f,%f)", ax[0]->GetNbins(),
               ax[0]->GetBinLowEdge(1), ax[0]->GetBinUpEdge(ax[0]->GetNbins()));
  std::sprintf(cmd[1], "e1+e2>>hre(%d,%f,%f)", ax[1]->GetNbins(),
               ax[1]->GetBinLowEdge(1), ax[1]->GetBinUpEdge(ax[1]->GetNbins()));
  std::sprintf(cut, "%g<t && t<%g && abs(dt)<%g && e3==0", t, pp, dt);
  std::printf("%s\n", cut);
  tr[0]->Draw(cmd[0], cut, "goff");
  tr[1]->Draw(cmd[1], cut, "goff");
  hle = (TH1*)gDirectory->Get("hle");
  hre = (TH1*)gDirectory->Get("hre");
}

pair<TH1*,TH1*> Unfold(TH1* hraw[2], RooUnfoldResponse* resp[2], int n,
                       const char* name) {
  RooUnfoldBayes* unf[2] = { new RooUnfoldBayes(resp[0], hraw[0], n, true),
                             new RooUnfoldBayes(resp[1], hraw[1], n, true) };
  char lname[64], rname[64];
  std::sprintf(lname, "%s_%d_l", name, n);
  std::sprintf(rname, "%s_%d_r", name, n);
  return std::make_pair((TH1*)unf[0]->Hreco()->Clone(lname),
                        (TH1*)unf[1]->Hreco()->Clone(rname));
}

vector< pair< pair<double,double>,pair<double,double> > > Count(const vector< pair<TH1*,TH1*> >& vhe, double elo, double ehi) {
  vector< pair< pair<double,double>,pair<double,double> > > vn;
  for (int i = 0; i < vhe.size(); ++i) {
    vn.push_back(std::make_pair(HistUtils::IntegralByUserLimits(vhe[i].first,  elo, ehi),
                                HistUtils::IntegralByUserLimits(vhe[i].second, elo, ehi)));
  }
  return vn;
}

void Draw(vector< pair<TH1*,TH1*> >& vhe, vector<int>& vnit) {
  gStyle->SetOptStat(0);
  gStyle->SetTitleSize(0.04, "xyz");
  gStyle->SetTitleSize(0.08, "");
  TLegend* leg = new TLegend(0.7, 0.5, 0.9, 0.9);
  TCanvas* c = new TCanvas("cspec", "cspec", 1400, 500);
  c->Divide(2);
  for (int i = 0; i < vhe.size(); ++i) {
    char legentry[16];
    std::sprintf(legentry, "%d", vnit[i]);
    Float_t blue = Float_t(i+1)/vhe.size();
    vhe[i].first ->SetLineColor(TColor::GetColor(0., 0., blue));
    vhe[i].second->SetLineColor(TColor::GetColor(0., 0., blue));
    vhe[i].first ->SetTitle("SiL Unfolded Proton Spectrum;Energy [keV];Count/500 keV");
    vhe[i].second->SetTitle("SiR Unfolded Proton Spectrum;Energy [keV];Count/500 keV");
    vhe[i].first ->GetYaxis()->SetTitleOffset(1.5);
    vhe[i].second->GetYaxis()->SetTitleOffset(1.5);
    vhe[i].first ->GetYaxis()->SetRangeUser(0, 250e3);
    vhe[i].second->GetYaxis()->SetRangeUser(0, 250e3);
    c->cd(1);
    vhe[i].first ->Draw("SAME");
    c->cd(2);
    vhe[i].second->Draw("SAME");
    leg->AddEntry(vhe[i].first, legentry);
  }
  c->cd(1);
  leg->Draw("SAME");
  c->cd(2);
  leg->Draw("SAME");
  c->SaveAs("img/stability_unfolding_iterations.png");
}

void DrawRatesGraph(vector< pair<TH1*,TH1*> >& vhe, const vector<int>& vnit) {
  TGraphErrors* gr[2] = { new TGraphErrors(vnit.size()),
                          new TGraphErrors(vnit.size()) };
  for (int i = 0; i < vnit.size(); ++i) {
    pair<double,double> y[2] = {
      HistUtils::IntegralByUserLimits(vhe[i].first,  3.5e3, 10e3),
      HistUtils::IntegralByUserLimits(vhe[i].second, 3.5e3, 10e3)
    };
    gr[0]->SetPoint     (i, vnit[i], y[0].first);
    gr[0]->SetPointError(i, 0,       y[0].second);
    gr[1]->SetPoint     (i, vnit[i], y[1].first);
    gr[1]->SetPointError(i, 0,       y[1].second);
  }
  gr[0]->SetLineColor(ACStyle::kACBlue);
  gr[1]->SetLineColor(ACStyle::kACRed);
  TMultiGraph* mg = new TMultiGraph();
  mg->Add(gr[0]);
  mg->Add(gr[1]);
  mg->SetTitle("Unfolded 3.5-10 MeV Proton Count;Number of iterations;Proton count");
  TLegend* l = new TLegend(0.7, 0.1, 0.9, 0.3);
  l->AddEntry(gr[0], "Left");
  l->AddEntry(gr[1], "Right");
  TCanvas* c = new TCanvas("crates", "crates", 700, 500);
  mg->Draw("AL");
  l->Draw();
  c->SaveAs("img/stability_unfolding_iterations_rates.png");
}

void stability_unfolding_iterations() {
  TH1::SetDefaultSumw2(kTRUE);
  TFile* fdata = new TFile("data/Al50/psel.3layer.pidpow2.root");
  TFile* fmctm = new TFile("data/Al50/tm.p.2layer.mudist.root");
  RooUnfoldResponse* resp[2] = { (RooUnfoldResponse*)fmctm->Get("SiL_TM"),
                                 (RooUnfoldResponse*)fmctm->Get("SiR_TM") };
  TMUtils::Rebin(resp[0], 5);
  TMUtils::Rebin(resp[1], 5);
  TMUtils::SetRange(resp[0], 2e3, 15e3);
  TMUtils::SetRange(resp[1], 2e3, 15e3);
  TTree* tr[2] = { (TTree*)fdata->Get("PID_LP"), (TTree*)fdata->Get("PID_RP") };
  TH1* hraw[2] = {nullptr, nullptr};
  FillMeasuredHists(resp, tr, hraw[0], hraw[1], 10e3, 400, 200);

  vector<int> vnit = {1, 5, 10, 20, 50, 100};
  vector< pair<TH1*, TH1*> > vhe;
  for (int i = 0; i < vnit.size(); ++i)
    vhe.push_back(Unfold(hraw, resp, vnit[i], "hebay"));
  vector< pair< pair<double,double>,pair<double,double> > > vnroi1 = Count(vhe, 3.5e3, 10e3);
  vector< pair< pair<double,double>,pair<double,double> > > vnroi2 = Count(vhe, 4e3,   8e3);

  Draw(vhe, vnit);

  vector<int> vnit2;
  vector< pair<TH1*, TH1*> > vhe2;
  for (int i = 1; i <= 100; ++i) {
    vnit2.push_back(i);
    vhe2.push_back(Unfold(hraw, resp, i, "hebay"));
  }
  DrawRatesGraph(vhe2, vnit2);


  for (int i = 0; i < vhe.size(); ++i) {
  std::cout << vnroi1[i].first.first  << "(" << vnroi1[i].first.second  << ") "
            << vnroi1[i].second.first << "(" << vnroi1[i].second.second << ") "
            << vnroi2[i].first.first  << "(" << vnroi2[i].first.second  << ") "
            << vnroi2[i].second.first << "(" << vnroi2[i].second.second << ") "
            << std::endl;
  }
  std::cout << std::endl;
  std::cout << (vnroi1[5].first.first-vnroi1[1].first.first)/(double)vnroi1[5].first.first
            << (vnroi1[5].first.first-vnroi1[1].first.first)/(double)vnroi1[5].first.first
            << std::endl;
  // TFile* ofile = new TFile("out/stability_unfoldinng_iterations.root", "RECREATE");
  // he_raw[0]->SetDirectory(ofile);
  // he_raw[1]->SetDirectory(ofile);
  // he_bay[0]->SetDirectory(ofile);
  // he_bay[1]->SetDirectory(ofile);
  // he_raw[0]->SetName("hl_e_raw");
  // he_raw[1]->SetName("hr_e_raw");

  // ofile->Write();
  // ofile->Close();
}
