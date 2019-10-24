#include "./scripts/Al50/util.h"

#include "TCanvas.h"
#include "TChain.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TH1.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TStyle.h"

#include <stdio.h>

void dt() {
  TChain* tr[2] = { new TChain(), new TChain() };
  tr[0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LP");
  // tr[0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LD");
  // tr[0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LT");
  // tr[0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LA");
  // tr[0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LU");
  tr[1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RP");
  // tr[1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RD");
  // tr[1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RT");
  // tr[1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RA");
  // tr[1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RU");

  TH1* h[2] = {
    new TH1D("hl", "Thin/Thick Proton Time Resolution;t_{Si2}-t_{Si1} [ns];Counts/10ns",
             40, -200, 200),
    new TH1D("hr", "Thin/Thick Proton Time Resolution;t_{Si2}-t_{Si1} [ns];Counts/10ns",
             40, -200, 200),
  };

  tr[0]->Draw("t2-t1>>+hl", "t>400", "goff");
  tr[1]->Draw("t2-t1>>+hr", "t>400", "goff");

  TF1* f[2] = { new TF1("fl", "gaus(0)"),
                new TF1("fr", "gaus(0)") };
  // TF1* f[2] = { new TF1("fl", "[0]*exp(-(abs(x-[1])/[2])^[3])+[4]"),
  // //               new TF1("fr", "[0]*exp(-(abs(x-[1])/[2])^[3])+[4]") };
  // f[0]->SetParameters(h[0]->GetMaximum(), 0, 70, 2, 10);//h[0]->GetBinContent(1));
  // f[1]->SetParameters(h[1]->GetMaximum(), 0, 70, 2, 10);//h[1]->GetBinContent(1));
  f[0]->SetParameters(h[0]->GetMaximum(), 0, 70);//h[0]->GetBinContent(1));
  f[1]->SetParameters(h[1]->GetMaximum(), 0, 70);//h[1]->GetBinContent(1));
  f[0]->SetNpx(1000);
  f[1]->SetNpx(1000);
  // f[0]->SetParLimits(3, 1, 3);
  // f[1]->SetParLimits(3, 1, 3);

  h[0]->SetLineColor(ACStyle::kACBlue);
  h[1]->SetLineColor(ACStyle::kACRed);
  f[0]->SetLineColor(kBlue);
  f[1]->SetLineColor(kRed);
  // h[0]->SetMinimum(0);
  // h[1]->SetMinimum(0);

  TFitResultPtr res[2] = { h[0]->Fit(f[0], "SMEL"), h[1]->Fit(f[1], "SMEL") };

  char lab[2][64];
  sprintf(lab[0], "SiL (#sigma=%.1fns)", res[0]->Parameter(2));
  sprintf(lab[1], "SiR (#sigma=%.1fns)", res[1]->Parameter(2));
  TLegend* leg = new TLegend(0.1, 0.6, 0.4, 0.9);
  leg->AddEntry(h[0], lab[0]);
  leg->AddEntry(h[1], lab[1]);

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 700, 500);
  h[1]->Draw("SAME");
  h[0]->Draw("SAME");
  leg->Draw();
  c->SaveAs("img/prot_dt_res.png");
}
