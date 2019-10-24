#include "TCanvas.h"
#include "TFile.h"
#include "TH2.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TTree.h"


namespace {
  TTree* tr[4][2];
  TH2*   hdeve[4][2];
}

void LoadTrees() {
  TFile* f[4] = {
    new TFile("data/Al50/Proton_EvdE_MC.root"),
    new TFile("data/Al50/Deuteron_EvdE_MC.root"),
    new TFile("data/Al50/Triton_EvdE_MC.root"),
    new TFile("data/Al50/Alpha_EvdE_MC.root")
  };
  tr[0][0] = (TTree*)f[0]->Get("PID_L");
  tr[0][1] = (TTree*)f[0]->Get("PID_R");
  tr[1][0] = (TTree*)f[1]->Get("PID_L");
  tr[1][1] = (TTree*)f[1]->Get("PID_R");
  tr[2][0] = (TTree*)f[2]->Get("PID_L");
  tr[2][1] = (TTree*)f[2]->Get("PID_R");
  tr[3][0] = (TTree*)f[3]->Get("PID_L");
  tr[3][1] = (TTree*)f[3]->Get("PID_R");
}

void FillHists(const char* c) {
  tr[0][0]->Draw("de:e>>hpl", c, "goff");
  tr[0][1]->Draw("de:e>>hpr", c, "goff");
  tr[1][0]->Draw("de:e>>hdl", c, "goff");
  tr[1][1]->Draw("de:e>>hdr", c, "goff");
  tr[2][0]->Draw("de:e>>htl", c, "goff");
  tr[2][1]->Draw("de:e>>htr", c, "goff");
  tr[3][0]->Draw("de:e>>hal", c, "goff");
  tr[3][1]->Draw("de:e>>har", c, "goff");
}

void FillHistsWithVeto() {
  FillHists("e1>0 && e2>0 && e3==0 && abs(dt)<200");
}

void FillHistsWithoutVeto() {
  FillHists("e1>0 && e2>0 && abs(dt)<200");
}

void BookHistograms() {
  hdeve[0][0] = new TH2D("hpl", "Particle ID in MC;E [keV];dE [keV]", 200, 0, 20e3, 100, 0, 10e3);
  hdeve[0][1] = new TH2D("hpr", "Particle ID in MC;E [keV];dE [keV]", 200, 0, 20e3, 100, 0, 10e3);
  hdeve[1][0] = new TH2D("hdl", "Particle ID in MC;E [keV];dE [keV]", 200, 0, 20e3, 100, 0, 10e3);
  hdeve[1][1] = new TH2D("hdr", "Particle ID in MC;E [keV];dE [keV]", 200, 0, 20e3, 100, 0, 10e3);
  hdeve[2][0] = new TH2D("htl", "Particle ID in MC;E [keV];dE [keV]", 200, 0, 20e3, 100, 0, 10e3);
  hdeve[2][1] = new TH2D("htr", "Particle ID in MC;E [keV];dE [keV]", 200, 0, 20e3, 100, 0, 10e3);
  hdeve[3][0] = new TH2D("hal", "Particle ID in MC;E [keV];dE [keV]", 200, 0, 20e3, 100, 0, 10e3);
  hdeve[3][1] = new TH2D("har", "Particle ID in MC;E [keV];dE [keV]", 200, 0, 20e3, 100, 0, 10e3);
}

void SetHistsAttributes() {
  hdeve[0][0]->SetMarkerColor(2), hdeve[0][1]->SetMarkerColor(2);
  hdeve[1][0]->SetMarkerColor(3), hdeve[1][1]->SetMarkerColor(3);
  hdeve[2][0]->SetMarkerColor(4), hdeve[2][1]->SetMarkerColor(4);
  hdeve[3][0]->SetMarkerColor(6), hdeve[3][1]->SetMarkerColor(6);
  hdeve[0][0]->SetLineColor(2),   hdeve[0][1]->SetLineColor(2);
  hdeve[1][0]->SetLineColor(3),   hdeve[1][1]->SetLineColor(3);
  hdeve[2][0]->SetLineColor(4),   hdeve[2][1]->SetLineColor(4);
  hdeve[3][0]->SetLineColor(6),   hdeve[3][1]->SetLineColor(6);
  for (int i = 0; i < 4; ++i) {
    hdeve[i][0]->GetYaxis()->SetTitleOffset(1.3);
    hdeve[i][1]->GetYaxis()->SetTitleOffset(1.3);
  }
}

TLegend* BuildLegend(TH2* h[4][2]) {
  TLegend* l = new TLegend(0.7, 0.7, 0.9, 0.9);
  l->AddEntry(hdeve[0][0], "Protons",   "l");
  l->AddEntry(hdeve[1][0], "Deuterons", "l");
  l->AddEntry(hdeve[2][0], "Tritons",   "l");
  l->AddEntry(hdeve[3][0], "Alpha",     "l");
  return l;
}

void Draw(TCanvas* c, TH2* h[4][2], TLegend* l) {
  c->cd(1);
  hdeve[0][0]->DrawClone("SAME");
  hdeve[1][0]->DrawClone("SAME");
  hdeve[2][0]->DrawClone("SAME");
  hdeve[3][0]->DrawClone("SAME");
  l->Draw();
  c->cd(2);
  hdeve[0][1]->DrawClone("SAME");
  hdeve[1][1]->DrawClone("SAME");
  hdeve[2][1]->DrawClone("SAME");
  hdeve[3][1]->DrawClone("SAME");
  l->Draw();
}

void mc_deve() {
  LoadTrees();
  BookHistograms();
  SetHistsAttributes();

  FillHistsWithVeto();
  gStyle->SetOptStat(0);
  TLegend* l = BuildLegend(hdeve);
  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  Draw(c, hdeve, l);
  c->SaveAs("img/mc_deve_veto.png");
  FillHistsWithoutVeto();
  TCanvas* c2 = new TCanvas("c2", "c2", 1400, 500);
  c2->Divide(2);
  Draw(c2, hdeve, l);
  c2->SaveAs("img/mc_deve_noveto.png");
}
