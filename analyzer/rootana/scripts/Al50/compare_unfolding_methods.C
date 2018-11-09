#include "scripts/Al50/util.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TROOT.h"

void compare_unfolding_methods() {

  TFile* f = new TFile("data/Al50/en.2layer.t400.dt200.rebin5.elo2e3keV.ehi20e3keV.root");
  TH1* hs[3][2];
  hs[0][0] = (TH1*)f->Get("hl_e_reco_bay");
  hs[0][1] = (TH1*)f->Get("hr_e_reco_bay");
  hs[1][0] = (TH1*)f->Get("hl_e_reco_svd");
  hs[1][1] = (TH1*)f->Get("hr_e_reco_svd");
  hs[2][0] = (TH1*)f->Get("hl_e_reco_bbb");
  hs[2][1] = (TH1*)f->Get("hr_e_reco_bbb");
  hs[0][0]->SetLineColor(1); hs[0][1]->SetLineColor(1);
  hs[1][0]->SetLineColor(2); hs[1][1]->SetLineColor(2);
  hs[2][0]->SetLineColor(3); hs[2][1]->SetLineColor(3);
  hs[0][0]->SetTitle("Left;E [keV];/p/500 keV");
  hs[0][1]->SetTitle("Right;E [keV];/p/500 keV");

  TLegend* leg = new TLegend(0.6, 0.7, 0.9, 0.9);
  leg->AddEntry(hs[0][0], "Bayes");
  leg->AddEntry(hs[1][0], "Singular Value Decomposition");
  leg->AddEntry(hs[2][0], "Bin-by-bin");

  double norm = Normalization::Al50();
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 2; ++j)
      hs[i][j]->Scale(1/norm);

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1);
  hs[0][0]->Draw();
  hs[1][0]->Draw("SAME");
  hs[2][0]->Draw("SAME");
  leg->Draw();
  c->cd(2);
  hs[0][1]->Draw();
  hs[1][1]->Draw("SAME");
  hs[2][1]->Draw("SAME");
  leg->Draw();
  c->SaveAs("img/compare_unfolding_methods.png");

  double n[3][2][2];
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 2; ++j) {
      n[i][j][0] = hs[i][j]->Integral(hs[i][j]->FindFixBin(3.5e3),
                                      hs[i][j]->FindFixBin(10e3));
      n[i][j][1] = hs[i][j]->Integral(hs[i][j]->FindFixBin(4e3),
                                      hs[i][j]->FindFixBin(8e3));
    }
  }


  printf("Unfold method\t3.5-10 MeV\t\t\t4-8 MeV\n");
  printf("\t\tLeft\t\tRight\t\tLeft\t\tRight\n");
  printf("Bayes\t\t%g\t%g\t%g\t%g\n",      n[0][0][0], n[0][1][0], n[0][0][1], n[0][1][1]);
  printf("SVD\t\t%g\t%g\t%g\t%g\n",        n[1][0][0], n[1][1][0], n[1][0][1], n[1][1][1]);
  printf("Bin-by-bin\t%g\t%g\t%g\t%g\n", n[2][0][0], n[2][1][0], n[2][0][1], n[2][1][1]);
}
