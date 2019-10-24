#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TMultiGraph.h"

void show_systematics() {
  TFile* fspec = new TFile("data/Al50/en.p.t400ns.dt200ns.rebin5.pp10us.elo2e3keV.ehi12e3keV.bay5.pidcorr.root");
  TH1* hspec[2] = {
    (TH1*)fspec->Get("hl_e_reco_bay"),
    (TH1*)fspec->Get("hr_e_reco_bay")
  };
  TFile* ferr_bw  = new TFile("data/Al50/corranderr/binwidth.root");
  TFile* ferr_co  = new TFile("data/Al50/corranderr/cutoff.root");
  TFile* ferr_it  = new TFile("data/Al50/corranderr/iterations.root");
  TFile* ferr_si  = new TFile("data/Al50/corranderr/siecal.root");
  TFile* ferr_pid = new TFile("data/Al50/corranderr/pid.root");
  TH1* herr_bw [2] = { (TH1*)ferr_bw ->Get("hl"),           (TH1*)ferr_bw ->Get("hr") };
  TH1* herr_co [2] = { (TH1*)ferr_co ->Get("hl"),           (TH1*)ferr_co ->Get("hr") };
  TH1* herr_it [2] = { (TH1*)ferr_it ->Get("hl"),           (TH1*)ferr_it ->Get("hr") };
  TH1* herr_si [2] = { (TH1*)ferr_si ->Get("hl"),           (TH1*)ferr_si ->Get("hr") };
  TH1* herr_pid[2] = { (TH1*)ferr_pid->Get("hlpidcorrerr"), (TH1*)ferr_pid->Get("hrpidcorrerr") };
  double X[20] = { 2250, 2750, 3250, 3750, 4250, 4750, 5250, 5750, 6250, 6750, 7250, 7750, 8250, 8750, 9250, 9750, 10250, 10750, 11250, 11750 };
  double Y_bw[2][20], Y_co[2][20], Y_it[2][20], Y_si[2][20], Y_pid[2][20];
  double Yabs_bw[2][20], Yabs_co[2][20], Yabs_it[2][20], Yabs_si[2][20], Yabs_pid[2][20];
  for (int i = 0; i < 20; ++i) {
    for (int j = 0; j < 2; ++j) {
      Y_bw    [j][i] = herr_bw [j]->GetBinError(i+1);
      Y_co    [j][i] = herr_co [j]->GetBinError(i+1);
      Y_it    [j][i] = herr_it [j]->GetBinError(i+1);
      Y_si    [j][i] = herr_si [j]->GetBinError(i+1);
      Y_pid   [j][i] = herr_pid[j]->GetBinError(i+1);
      Yabs_bw [j][i] = herr_bw [j]->GetBinError(i+1)*hspec[j]->GetBinContent(i+1);
      Yabs_co [j][i] = herr_co [j]->GetBinError(i+1)*hspec[j]->GetBinContent(i+1);
      Yabs_it [j][i] = herr_it [j]->GetBinError(i+1)*hspec[j]->GetBinContent(i+1);
      Yabs_si [j][i] = herr_si [j]->GetBinError(i+1)*hspec[j]->GetBinContent(i+1);
      Yabs_pid[j][i] = herr_pid[j]->GetBinError(i+1)*hspec[j]->GetBinContent(i+1);
    }
  }
  TGraph* gr_bw    [2] = { new TGraph(20, X, Y_bw    [0]), new TGraph(20, X, Y_bw    [1]) };
  TGraph* gr_co    [2] = { new TGraph(20, X, Y_co    [0]), new TGraph(20, X, Y_co    [1]) };
  TGraph* gr_it    [2] = { new TGraph(20, X, Y_it    [0]), new TGraph(20, X, Y_it    [1]) };
  TGraph* gr_si    [2] = { new TGraph(20, X, Y_si    [0]), new TGraph(20, X, Y_si    [1]) };
  TGraph* gr_pid   [2] = { new TGraph(20, X, Y_pid   [0]), new TGraph(20, X, Y_pid   [1]) };
  TGraph* grabs_bw [2] = { new TGraph(20, X, Yabs_bw [0]), new TGraph(20, X, Yabs_bw [1]) };
  TGraph* grabs_co [2] = { new TGraph(20, X, Yabs_co [0]), new TGraph(20, X, Yabs_co [1]) };
  TGraph* grabs_it [2] = { new TGraph(20, X, Yabs_it [0]), new TGraph(20, X, Yabs_it [1]) };
  TGraph* grabs_si [2] = { new TGraph(20, X, Yabs_si [0]), new TGraph(20, X, Yabs_si [1]) };
  TGraph* grabs_pid[2] = { new TGraph(20, X, Yabs_pid[0]), new TGraph(20, X, Yabs_pid[1]) };

  TMultiGraph* mg[2] = {
    new TMultiGraph("mgl", "Left Systematics (Fractional);Energy [keV];Fractional Error"),
    new TMultiGraph("mgr", "Right Systematics (Fractional);Energy [keV];Fractional Error")
  };
  TMultiGraph* mgabs[2] = {
    new TMultiGraph("mglabs", "Left Systematics (Absolute);Energy [keV];Unfolded Proton Count"),
    new TMultiGraph("mgrabs", "Right Systematics (Absolute);Energy [keV];Unfolded Proton Count")
  };
  mg   [0]->Add(gr_bw    [0], "P PMC");
  mg   [0]->Add(gr_co    [0], "P PMC");
  mg   [0]->Add(gr_it    [0], "P PMC");
  mg   [0]->Add(gr_si    [0], "P PMC");
  mg   [0]->Add(gr_pid   [0], "P PMC");
  mg   [1]->Add(gr_bw    [1], "P PMC");
  mg   [1]->Add(gr_co    [1], "P PMC");
  mg   [1]->Add(gr_it    [1], "P PMC");
  mg   [1]->Add(gr_si    [1], "P PMC");
  mg   [1]->Add(gr_pid   [1], "P PMC");
  mgabs[0]->Add(grabs_bw [0], "P PMC");
  mgabs[0]->Add(grabs_co [0], "P PMC");
  mgabs[0]->Add(grabs_it [0], "P PMC");
  mgabs[0]->Add(grabs_si [0], "P PMC");
  mgabs[0]->Add(grabs_pid[0], "P PMC");
  mgabs[1]->Add(grabs_bw [1], "P PMC");
  mgabs[1]->Add(grabs_co [1], "P PMC");
  mgabs[1]->Add(grabs_it [1], "P PMC");
  mgabs[1]->Add(grabs_si [1], "P PMC");
  mgabs[1]->Add(grabs_pid[1], "P PMC");

  TLegend* leg = new TLegend(0.65, 0.55, 0.95, 0.95);
  leg->AddEntry(gr_bw [0], "Binwidth");
  leg->AddEntry(gr_co [0], "Lower Energy Cutoff");
  leg->AddEntry(gr_it [0], "# Unfolding Iterations");
  leg->AddEntry(gr_si [0], "Si ECal");
  leg->AddEntry(gr_pid[0], "PID");

  gr_bw [0]->SetMarkerColor(kRed);
  gr_co [0]->SetMarkerColor(kGreen);
  gr_it [0]->SetMarkerColor(kBlue);
  gr_si [0]->SetMarkerColor(kYellow);
  gr_pid[0]->SetMarkerColor(kBlack);
  gr_bw [1]->SetMarkerColor(kRed);
  gr_co [1]->SetMarkerColor(kGreen);
  gr_it [1]->SetMarkerColor(kBlue);
  gr_si [1]->SetMarkerColor(kYellow);
  gr_pid[1]->SetMarkerColor(kBlack);
  gr_bw [0]->SetLineColor(kRed);
  gr_co [0]->SetLineColor(kGreen);
  gr_it [0]->SetLineColor(kBlue);
  gr_si [0]->SetLineColor(kYellow);
  gr_pid[0]->SetLineColor(kBlack);
  gr_bw [1]->SetLineColor(kRed);
  gr_co [1]->SetLineColor(kGreen);
  gr_it [1]->SetLineColor(kBlue);
  gr_si [1]->SetLineColor(kYellow);
  gr_pid[1]->SetLineColor(kBlack);
  grabs_bw [0]->SetMarkerColor(kRed);
  grabs_co [0]->SetMarkerColor(kGreen);
  grabs_it [0]->SetMarkerColor(kBlue);
  grabs_si [0]->SetMarkerColor(kYellow);
  grabs_pid[0]->SetMarkerColor(kBlack);
  grabs_bw [1]->SetMarkerColor(kRed);
  grabs_co [1]->SetMarkerColor(kGreen);
  grabs_it [1]->SetMarkerColor(kBlue);
  grabs_si [1]->SetMarkerColor(kYellow);
  grabs_pid[1]->SetMarkerColor(kBlack);
  grabs_bw [0]->SetLineColor(kRed);
  grabs_co [0]->SetLineColor(kGreen);
  grabs_it [0]->SetLineColor(kBlue);
  grabs_si [0]->SetLineColor(kYellow);
  grabs_pid[0]->SetLineColor(kBlack);
  grabs_bw [1]->SetLineColor(kRed);
  grabs_co [1]->SetLineColor(kGreen);
  grabs_it [1]->SetLineColor(kBlue);
  grabs_si [1]->SetLineColor(kYellow);
  grabs_pid[1]->SetLineColor(kBlack);

  TCanvas* cerr = new TCanvas("cerr", "cerr", 1400, 500);
  cerr->Divide(2);
  cerr->cd(1); mg[0]->Draw("AP PMC"); leg->Draw();
  cerr->cd(2); mg[1]->Draw("AP PMC"); leg->Draw();
  cerr->SaveAs("img/systematics/show_syst_frac.png");

  TCanvas* cabs = new TCanvas("cabs", "cabs", 1400, 500);
  cabs->Divide(2);
  cabs->cd(1); mgabs[0]->Draw("AP PMC"); leg->Draw();
  cabs->cd(2); mgabs[1]->Draw("AP PMC"); leg->Draw();
  cabs->SaveAs("img/systematics/show_syst_abs.png");
}
