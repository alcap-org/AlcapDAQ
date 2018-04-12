#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

void Andy_Si16b_IncomingMuons_NonCentralMuons() {
  
  std::string filename = "Andy_Si16b_IncomingMuons.root";
  //  std::string filename = "Andy_Si16_MC_SiTMuons.root";

  TFile* file = new TFile(filename.c_str(), "READ");

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
  gStyle->SetOptFit(1);
  
  TH2F* hCorrelations = (TH2F*) file->Get("hCorrelations");

  double min_energy_cut = 5000;
  double max_energy_cut = 6000;
  int min_energy_cut_bin = hCorrelations->GetYaxis()->FindBin(min_energy_cut);
  int max_energy_cut_bin = hCorrelations->GetYaxis()->FindBin(max_energy_cut);

  TH1D* hTime = hCorrelations->ProjectionX("hTime", min_energy_cut_bin, max_energy_cut_bin);
  hTime->Rebin(5);
  
  double min_time_fit = 100;
  double max_time_fit = 3000;
  TF1* hTimeFit = new TF1("hTimeFit", "[0]*TMath::Exp(-x/[1]) + [2]*TMath::Exp(-x/[3]) + [4]", min_time_fit, max_time_fit);
  hTimeFit->SetParameters(10000, 750, 10000, 100, 10);
  hTimeFit->SetParName(0, "Si Norm");
  hTimeFit->SetParName(1, "Si Lifetime");
  hTimeFit->SetParName(2, "Pb? Norm");
  hTimeFit->SetParName(3, "Pb? Lifetime");
  hTimeFit->SetParName(4, "Bkg");
  
  hTime->Fit(hTimeFit, "R");

  hTime->Draw("HIST E");
  hTime->GetFunction("hTimeFit")->Draw("LSAME");
}
