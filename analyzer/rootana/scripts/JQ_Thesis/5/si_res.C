#include "scripts/Al50/util.h"

#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TH1.h"
#include "TH1D.h"
#include "TStyle.h"
#include "TTree.h"

#include <array>
#include <cstdio>
using std::array;
using std::printf;

void eres(array<array<TTree*,2>,5>& tr) {
  double e = 5400;
  double err[] = {
    SiUtils::ECal::ConstructECal("SiT-1")  .Eerr(5400),
    SiUtils::ECal::ConstructECal("SiT-2")  .Eerr(5400),
    SiUtils::ECal::ConstructECal("SiT-3")  .Eerr(5400),
    SiUtils::ECal::ConstructECal("SiT-4")  .Eerr(5400),
    SiUtils::ECal::ConstructECal("SiR1-1") .Eerr(5400),
    SiUtils::ECal::ConstructECal("SiR1-2") .Eerr(5400),
    SiUtils::ECal::ConstructECal("SiR1-3") .Eerr(5400),
    SiUtils::ECal::ConstructECal("SiR1-4") .Eerr(5400),
    SiUtils::ECal::ConstructECal("SiL1-2") .Eerr(5400),
    SiUtils::ECal::ConstructECal("SiL1-3") .Eerr(5400),
    SiUtils::ECal::ConstructECal("SiL1-4") .Eerr(5400),
    SiUtils::ECal::ConstructECal("SiL1-5") .Eerr(5400),
    SiUtils::ECal::ConstructECal("SiL1-6") .Eerr(5400),
    SiUtils::ECal::ConstructECal("SiL1-7") .Eerr(5400),
    SiUtils::ECal::ConstructECal("SiL1-8") .Eerr(5400),
    SiUtils::ECal::ConstructECal("SiL1-9") .Eerr(5400),
    SiUtils::ECal::ConstructECal("SiL1-10").Eerr(5400),
    SiUtils::ECal::ConstructECal("SiL1-11").Eerr(5400),
    SiUtils::ECal::ConstructECal("SiL1-12").Eerr(5400),
    SiUtils::ECal::ConstructECal("SiL1-13").Eerr(5400),
    SiUtils::ECal::ConstructECal("SiL1-14").Eerr(5400),
    SiUtils::ECal::ConstructECal("SiL1-15").Eerr(5400),
    SiUtils::ECal::ConstructECal("SiL3").Eerr(5400),
    SiUtils::ECal::ConstructECal("SiR2").Eerr(5400),
    SiUtils::ECal::ConstructECal("SiR3").Eerr(5400)
  };
  printf("SiT-1:  \t%g\n", err[0]);
  printf("SiT-2:  \t%g\n", err[1]);
  printf("SiT-3:  \t%g\n", err[2]);
  printf("SiT-4:  \t%g\n", err[3]);
  printf("SiR1-1: \t%g\n", err[4]);
  printf("SiR1-2: \t%g\n", err[5]);
  printf("SiR1-3: \t%g\n", err[6]);
  printf("SiR1-4: \t%g\n", err[7]);
  printf("SiL1-2: \t%g\n", err[8]);
  printf("SiL1-3: \t%g\n", err[9]);
  printf("SiL1-4: \t%g\n", err[10]);
  printf("SiL1-5: \t%g\n", err[11]);
  printf("SiL1-6: \t%g\n", err[12]);
  printf("SiL1-7: \t%g\n", err[13]);
  printf("SiL1-8: \t%g\n", err[14]);
  printf("SiL1-9: \t%g\n", err[15]);
  printf("SiL1-10:\t%g\n", err[16]);
  printf("SiL1-11:\t%g\n", err[17]);
  printf("SiL1-12:\t%g\n", err[18]);
  printf("SiL1-13:\t%g\n", err[19]);
  printf("SiL1-14:\t%g\n", err[20]);
  printf("SiL1-15:\t%g\n", err[21]);
  printf("SiR2:   \t%g\n", err[22]);
  printf("SiR3:   \t%g\n", err[23]);
  printf("SiL3:   \t%g\n", err[24]);
}

void tres(array<array<TTree*,2>,5>& tr) {

  TH1* hl = new TH1D("hlt", "SiL Time Resolution;t_{Si2}-t_{SiT} [ns]", 140, -200, 500);
  TH1* hr = new TH1D("hrt", "SiR Time Resolution;t_{Si2}-t_{SiT} [ns]", 140, -200, 500);
  for (int i = 0; i < 5; ++i) {
    tr[i][0]->Draw("t2>>+hlt", "", "goff");
    tr[i][1]->Draw("t2>>+hrt", "", "goff");
  }
  hl->SetLineColor(kBlack);
  hr->SetLineColor(kBlack);

  TF1* fcn[2]  = {
    new TF1("fcnlt", "gaus(0)", -200, 20), new TF1("fcnrt", "gaus(0)", -200, 20)
  };
  fcn[0]->SetLineColor(kBlack);
  fcn[0]->SetLineStyle(kDashed);
  fcn[0]->SetParNames("Amplitude", "Offset", "Resolution");
  fcn[1]->SetLineColor(kBlack);
  fcn[1]->SetLineStyle(kDashed);
  fcn[1]->SetParNames("Amplitude", "Offset", "Resolution");

  TFitResultPtr res[2] = { hl->Fit(fcn[0], "MERS"), hr->Fit(fcn[1], "MERS") };

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(111);
  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1);
  hl->Draw();
  c->cd(2);
  hr->Draw();
  c->SaveAs("img/thesis/5/si_tres.png");
}

void si_res() {
  TFile* f = new TFile("data/Al50/psel.3layer.pidpow2.root");
  array<array<TTree*,2>,5> tr = {{
    {{ (TTree*)f->Get("PID_LP"), (TTree*)f->Get("PID_RP") }},
    {{ (TTree*)f->Get("PID_LD"), (TTree*)f->Get("PID_RD") }},
    {{ (TTree*)f->Get("PID_LT"), (TTree*)f->Get("PID_RT") }},
    {{ (TTree*)f->Get("PID_LA"), (TTree*)f->Get("PID_RA") }},
    {{ (TTree*)f->Get("PID_LU"), (TTree*)f->Get("PID_RU") }}
  }};
  tres(tr);
  eres(tr);
}
