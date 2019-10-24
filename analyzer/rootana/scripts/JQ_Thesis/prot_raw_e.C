#include "./scripts/Al50/util.h"

#include "TCanvas.h"
#include "TColor.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TTree.h"

void prot_raw_e() {
  TFile* f = new TFile("data/Al50/psel.3layer.pidpow2.root");
  TTree* PID_LP = (TTree*)f->Get("PID_LP");
  TTree* PID_RP = (TTree*)f->Get("PID_RP");
  PID_LP->Draw("e>>hl(52,2e3,15e3)",     "e3==0",          "goff");
  PID_RP->Draw("e>>hr(52,2e3,15e3)",     "e3==0",          "goff");
  PID_LP->Draw("e>>hltcut(52,2e3,15e3)", "e3==0 && t>400", "goff");
  PID_RP->Draw("e>>hrtcut(52,2e3,15e3)", "e3==0 && t>400", "goff");

  TH1* hl     = (TH1*)gDirectory->Get("hl");
  TH1* hr     = (TH1*)gDirectory->Get("hr");
  TH1* hltcut = (TH1*)gDirectory->Get("hltcut");
  TH1* hrtcut = (TH1*)gDirectory->Get("hrtcut");
  hl    ->SetLineColor(ACStyle::kACBlue);
  hr    ->SetLineColor(ACStyle::kACRed);
  hltcut->SetLineColor(ACStyle::kACBlue);
  hrtcut->SetLineColor(ACStyle::kACRed);
  hltcut->SetLineStyle(2);
  hrtcut->SetLineStyle(2);

  hl    ->SetTitle("Raw proton energy (Al50);E [keV]");
  hr    ->SetTitle("Raw proton energy (Al50);E [keV]");
  hltcut->SetTitle("Raw proton energy (Al50);E [keV]");
  hrtcut->SetTitle("Raw proton energy (Al50);E [keV]");

  TLegend* l = new TLegend(0.6, 0.6, 0.9, 0.9);
  l->AddEntry(hl,     "Left (t > 0ns)");
  l->AddEntry(hr,     "Right (t > 0ns)");
  l->AddEntry(hltcut, "Left (t > 400ns)");
  l->AddEntry(hrtcut, "Right (t > 400ns)");


  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas;
  hr    ->Draw("SAME");
  hl    ->Draw("SAME");
  hltcut->Draw("SAME");
  hrtcut->Draw("SAME");
  l->Draw();
  c->SaveAs("img/prot_raw_e.png");

}
