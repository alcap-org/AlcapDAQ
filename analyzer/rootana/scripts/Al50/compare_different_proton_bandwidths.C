#include "Rtypes.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TLine.h"
#include "TString.h"
#include "TStyle.h"

class Contour {
  TF1 *top, *bottom;
  TLine *right, *left;
public:
  Contour(const TString& name, double delo, double dehi, double p) {
    double elo = 1.9e3, ehi = 17e3;
    TString name_top    = name + "_top";
    TString name_bottom = name + "_bottom";
    top    = new TF1(name_top,    "[0]*x^[1]", elo, ehi);
    bottom = new TF1(name_bottom, "[0]*x^[1]", elo, ehi);
    top   ->SetParameters(dehi, p);
    bottom->SetParameters(delo, p);
    right = new TLine(elo, top->Eval(elo), elo, bottom->Eval(elo));
    left  = new TLine(ehi, top->Eval(ehi), ehi, bottom->Eval(ehi));
  }
  void SetLineColor(Color_t col) {
    top   ->SetLineColor(col);
    bottom->SetLineColor(col);
    right ->SetLineColor(col);
    left  ->SetLineColor(col);
  }
  void Draw() {
    top   ->Draw("LSAME");
    bottom->Draw("LSAME");
    right ->Draw("LSAME");
    left  ->Draw("LSAME");
  }
};

void BuildChain(TChain*& cl, TChain*& cr, const TString& fname) {
  cl = new TChain();
  cl->Add(fname+"/PID_LP");
  cl->Add(fname+"/PID_LD");
  cl->Add(fname+"/PID_LT");
  cl->Add(fname+"/PID_LA");
  cl->Add(fname+"/PID_LU");
  cr = new TChain();
  cr->Add(fname+"/PID_RP");
  cr->Add(fname+"/PID_RD");
  cr->Add(fname+"/PID_RT");
  cr->Add(fname+"/PID_RA");
  cr->Add(fname+"/PID_RU");
}

TH1* MakedEvEHistogram() {
  //
}

void compare_different_proton_bandwidths() {
  TChain* deve[6][2];
  BuildChain(deve[0][0], deve[0][1], "data/Al50/psel.pow.bandx1.3layer.root");
  BuildChain(deve[1][0], deve[1][1], "data/Al50/psel.pow.bandx1.1.3layer.root");
  BuildChain(deve[2][0], deve[2][1], "data/Al50/psel.pow.bandx1.2.3layer.root");
  BuildChain(deve[3][0], deve[3][1], "data/Al50/psel.pow.bandx1.3.3layer.root");
  BuildChain(deve[4][0], deve[4][1], "data/Al50/psel.pow.bandx1.4.3layer.root");
  BuildChain(deve[5][0], deve[5][1], "data/Al50/psel.pow.bandx1.5.3layer.root");
  TFile* fens[6] = {
    new TFile("data/Al50/en.pow.bandx1.2layer.t400.dt200.rebin5.elo2e3keV.ehi15e3keV.root"),
    new TFile("data/Al50/en.pow.bandx1.1.2layer.t400.dt200.rebin5.elo2e3keV.ehi15e3keV.root"),
    new TFile("data/Al50/en.pow.bandx1.2.2layer.t400.dt200.rebin5.elo2e3keV.ehi15e3keV.root"),
    new TFile("data/Al50/en.pow.bandx1.3.2layer.t400.dt200.rebin5.elo2e3keV.ehi15e3keV.root"),
    new TFile("data/Al50/en.pow.bandx1.4.2layer.t400.dt200.rebin5.elo2e3keV.ehi15e3keV.root"),
    new TFile("data/Al50/en.pow.bandx1.5.2layer.t400.dt200.rebin5.elo2e3keV.ehi15e3keV.root")
  };
  TH1* hens[6][2] = {
    {(TH1*)fens[0]->Get("hl_e_reco_bay"), (TH1*)fens[0]->Get("hr_e_reco_bay")},
    {(TH1*)fens[1]->Get("hl_e_reco_bay"), (TH1*)fens[1]->Get("hr_e_reco_bay")},
    {(TH1*)fens[2]->Get("hl_e_reco_bay"), (TH1*)fens[2]->Get("hr_e_reco_bay")},
    {(TH1*)fens[3]->Get("hl_e_reco_bay"), (TH1*)fens[3]->Get("hr_e_reco_bay")},
    {(TH1*)fens[4]->Get("hl_e_reco_bay"), (TH1*)fens[4]->Get("hr_e_reco_bay")},
    {(TH1*)fens[5]->Get("hl_e_reco_bay"), (TH1*)fens[5]->Get("hr_e_reco_bay")}
  };
  Contour cons[6][2] = {
    {Contour("con10l", 85e4,    15e5,     -0.85), Contour("con10r", 12e5,   22e5,   -0.9)},
    {Contour("con11l", 81.75e4, 15.325e5, -0.85), Contour("con11r", 11.5e5, 22.5e5, -0.9)},
    {Contour("con12l", 78.5e4,  15.65e5,  -0.85), Contour("con12r", 11e5,   23e5,   -0.9)},
    {Contour("con13l", 75.25e4, 15.975e5, -0.85), Contour("con13r", 10.5e5, 23.5e5, -0.9)},
    {Contour("con14l", 72e4,    16.3e5,   -0.85), Contour("con14r", 10e5,   24e5,   -0.9)},
    {Contour("con15l", 68.75e4, 16.625e5, -0.85), Contour("con15r", 9.5e5,  24.5e5, -0.9)}
  };


  deve[0][0]->Draw("de:e>>hdevel(2000,0,20e3,300,0,3e3)", "", "goff");
  deve[0][1]->Draw("de:e>>hdever(2000,0,20e3,300,0,3e3)", "", "goff");
  TH2* hdeve[2] = { (TH2*)gDirectory->Get("hdevel"),
                    (TH2*)gDirectory->Get("hdever") };

  cons[0][0].SetLineColor(1);  cons[0][1].SetLineColor(1);
  cons[1][0].SetLineColor(2);  cons[1][1].SetLineColor(2);
  cons[2][0].SetLineColor(3);  cons[2][1].SetLineColor(3);
  cons[3][0].SetLineColor(4);  cons[3][1].SetLineColor(4);
  cons[4][0].SetLineColor(5);  cons[4][1].SetLineColor(5);
  cons[5][0].SetLineColor(6);  cons[5][1].SetLineColor(6);
  hens[0][0]->SetLineColor(1); hens[0][1]->SetLineColor(1);
  hens[1][0]->SetLineColor(2); hens[1][1]->SetLineColor(2);
  hens[2][0]->SetLineColor(3); hens[2][1]->SetLineColor(3);
  hens[3][0]->SetLineColor(4); hens[3][1]->SetLineColor(4);
  hens[4][0]->SetLineColor(5); hens[4][1]->SetLineColor(5);
  hens[5][0]->SetLineColor(6); hens[5][1]->SetLineColor(6);

  hdeve[0]  ->SetTitle("SiL;E [keV];dE [keV]");
  hdeve[1]  ->SetTitle("SiR;E [keV];dE [keV]");
  hens[0][0]->SetTitle("SiL;E [keV];cts/500 keV");
  hens[0][1]->SetTitle("SiL;E [keV];cts/500 keV");


  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->SetHeader("Bandwidth Multiplier");
  leg->AddEntry(hens[0][0], "1");
  leg->AddEntry(hens[1][0], "1.1");
  leg->AddEntry(hens[2][0], "1.2");
  leg->AddEntry(hens[3][0], "1.3");
  leg->AddEntry(hens[4][0], "1.4");
  leg->AddEntry(hens[5][0], "1.5");

  gStyle->SetOptStat(0);
  TCanvas* cdeve = new TCanvas("cdeve", "deve", 1400, 500);
  cdeve->Divide(2);
  cdeve->cd(1);
  hdeve[0]->Draw();
  cons[0][0].Draw();
  cons[1][0].Draw();
  cons[2][0].Draw();
  cons[3][0].Draw();
  cons[4][0].Draw();
  cons[5][0].Draw();
  leg->Draw();
  cdeve->cd(2);
  hdeve[1]->Draw();
  cons[0][1].Draw();
  cons[1][1].Draw();
  cons[2][1].Draw();
  cons[3][1].Draw();
  cons[4][1].Draw();
  cons[5][1].Draw();
  leg->Draw();
  cdeve->SaveAs("img/proton_bandwidths_deve.png");
  TCanvas* cen = new TCanvas("cen", "en", 1400, 500);
  cen->Divide(2);
  cen->cd(1);
  hens[0][0]->Draw("SAME");
  hens[1][0]->Draw("SAME");
  hens[2][0]->Draw("SAME");
  hens[3][0]->Draw("SAME");
  hens[4][0]->Draw("SAME");
  hens[5][0]->Draw("SAME");
  leg->Draw();
  cen->cd(2);
  hens[0][1]->Draw("SAME");
  hens[1][1]->Draw("SAME");
  hens[2][1]->Draw("SAME");
  hens[3][1]->Draw("SAME");
  hens[4][1]->Draw("SAME");
  hens[5][1]->Draw("SAME");
  leg->Draw();
  cen->SaveAs("img/proton_bandwidths_en.png");
}
