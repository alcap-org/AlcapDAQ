#include "TCanvas.h"
#include "TF1.h"
#include "TH1.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TRandomGen.h"
#include "TStyle.h"

namespace {
  TRandomMixMax RNDM(65534);
  double TLIM[2] = { -500., 6e3 };
  double DT      = 10.;
  int    NBIN    = (TLIM[1]-TLIM[0])/DT;
  double TSIG    = 45.;
  int    NBG     = 2.5*338949;
  int    NPB     = 17.855367/40.*NBG;
  int    NAL     = 0.76489349*NBG;
  double LTPB    = 80.;
  double LTAL    = 864.;
}

void tres_simulate_res_2exp() {
  TH1* htrue = new TH1D("htrue", "htrue", NBIN, TLIM[0], TLIM[1]);
  TH1* hmeas = new TH1D("hmeas", "hmeas", NBIN, TLIM[0], TLIM[1]);
  for (int i = 0; i < NBG; ++i) {
    double t  = RNDM.Uniform(TLIM[0], TLIM[1]);
    double dt = RNDM.Gaus(-50., TSIG);
    htrue->Fill(t);
    hmeas->Fill(t+dt);
  }
  for (int i = 0; i < NPB; ++i) {
    double t  = RNDM.Exp(LTPB);
    double dt = RNDM.Gaus(-50, TSIG);
    htrue->Fill(t);
    hmeas->Fill(t+dt);
  }
  for (int i = 0; i < NAL; ++i) {
    double t  = RNDM.Exp(LTAL);
    double dt = RNDM.Gaus(-50., TSIG);
    htrue->Fill(t);
    hmeas->Fill(t+dt);
  }
  // TF1* fitfnc = new TF1("fitfnc", "[0]+[1]*exp(-x/[2])+[3]*exp(-x/[4])", 0., 5e3);
  TF1* fitfnc = new TF1("fitfnc", "[0]+[1]*exp(-x/[2])+[3]*exp(-x/[4])", 500., 5e3);
  fitfnc->SetParNames("Const.", "Al Amp.", "#tau_{Al}", "Pb/SS Amp.", "#tau_{Pb/SS}");
  fitfnc->SetParameters(1.4e3, 6.2e3, 864., 28e3, 95.);
  fitfnc->FixParameter(3, 0.); fitfnc->FixParameter(4, 1.);
  htrue ->SetLineColor(kGreen);
  hmeas ->SetLineColor(kRed);
  fitfnc->SetLineColor(kBlack);
  // htrue->GetXaxis()->SetRangeUser(0., 5e3);
  htrue->GetXaxis()->SetRangeUser(500., 5e3);
  hmeas->Fit(fitfnc, "R");

  htrue->SetTitle("Effects of Timing Resolution on Lifetime;Time [ns];[/100ns]");

  TLegend* leg = new TLegend(0.300, 0.635, 0.600, 0.935);
  leg->AddEntry(htrue,  "True");
  leg->AddEntry(hmeas,  "Res. Folded");
  leg->AddEntry(fitfnc, "Res. Fold. Fit");

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(10111);
  TCanvas* c = new TCanvas("c", "c", 700, 500);
  htrue->Draw("SAME");
  hmeas->Draw("SAME");
  leg  ->Draw();
  c->SetLogy();
  // c->SaveAs("img/res_simulate_2exp.png");
  c->SaveAs("img/res_simulate_1exp.png");
}
