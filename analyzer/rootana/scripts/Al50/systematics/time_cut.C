#include "./scripts/Al50/util.h"

#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TH1.h"
#include "TH1D.h"
#include "TStyle.h"
#include "TTree.h"

#include <cstdio>
#include <iostream>

void time_cut() {
  TFile* ifile = new TFile("data/Al50/psel.3layer.pidpow2.root");
  TTree* tr[2] = { (TTree*)ifile->Get("PID_LP"), (TTree*)ifile->Get("PID_RP") };
  TH1* ht[2] = {
    new TH1D("htl", "htl", 100, -500., 500.),
    new TH1D("htr", "htr", 100, -500., 500.)
  };
  tr[0]->Draw("t2>>htl");
  tr[1]->Draw("t2>>htr");

  TF1* fitfnc[2] = {
    new TF1("fitfncl", "gaus(0)", -200, 0),
    new TF1("fitfncr", "gaus(0)", -200, 0)
  };
  fitfnc[0]->SetLineColor(kBlue);
  fitfnc[1]->SetLineColor(kRed);
  // fitfnc[0]->SetParameters(400, 0, 50, 0);
  // fitfnc[1]->SetParameters(400, 0, 50, 0);
  TFitResultPtr res[2] = {
    ht[0]->Fit(fitfnc[0], "SMELR"),
    ht[1]->Fit(fitfnc[1], "SMELR")
  };

  std::cout << "Left chi2/ndf:  " << res[0]->Chi2() << "/" << res[0]->Ndf() << std::endl
            << "Right chi2/ndf: " << res[1]->Chi2() << "/" << res[1]->Ndf() << std::endl;

  ht[0]->SetTitle("SiL Timing;t_{2} [ns];Counts/10ns");
  ht[1]->SetTitle("SiR Timing;t_{2} [ns];Counts/10ns");
  ht[0]->SetLineColor(ACStyle::kACBlue);
  ht[1]->SetLineColor(ACStyle::kACRed);

  gStyle->SetOptStat(0);
  gStyle->SetOptFit (111);

  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1); ht[1]->Draw();
  c->cd(2); ht[0]->Draw();

  std::cout << "SiL Eff: " << fitfnc[0]->Integral(-200, 200)/fitfnc[0]->Integral(-10e3, 10e3) << std::endl;
  std::cout << "SiL Eff: " << fitfnc[1]->Integral(-200, 200)/fitfnc[1]->Integral(-10e3, 10e3) << std::endl;
  std::cout << fitfnc[0]->IntegralError(-200, 200)/*/fitfnc[0]->Integral(-200, 200)*/ << " " << fitfnc[1]->IntegralError(-200, 200)/*/fitfnc[1]->Integral(-200, 200)*/ << std::endl;
}
