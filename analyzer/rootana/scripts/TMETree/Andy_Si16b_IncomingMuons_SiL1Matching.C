#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TLegend.h"

#include <iostream>
#include <sstream>

void Andy_Si16b_IncomingMuons_SiL1Matching() {

  double min_edep = 400;
  double max_edep = 1700;
  int rebin_factor = 10;
  
  //  TFile* file_MC = new TFile("Andy_Si16_MC_SiL1Muons.root", "READ");
  //  TFile* file_MC = new TFile("Andy_Si16_MC_SiL1-Ephi--40deg_SiL1Muons.root", "READ");
  //  TFile* file_MC = new TFile("Andy_Si16_MC_SiL1-Ephi--35deg_SiL1Muons.root", "READ");
  //  TFile* file_MC = new TFile("Andy_Si16_MC_SiL1-Ephi--30deg_SiL1Muons.root", "READ");
  //  TFile* file_MC = new TFile("Andy_Si16_MC_SiL1-Ephi--41deg_SiL1Muons.root", "READ");
  //  TFile* file_MC = new TFile("Andy_Si16_MC_SiL1-Ephi--39deg_SiL1Muons.root", "READ");
  //  TFile* file_MC = new TFile("Andy_Si16_MC_SiL1-Ephi--40deg_ERes-40keV_SiL1Muons.root", "READ");
  //  TFile* file_MC = new TFile("Andy_Si16_MC_SiL1-Ephi--40deg_ERes-80keV_SiL1Muons.root", "READ");
  //  TFile* file_MC = new TFile("Andy_Si16_MC_Ekin-1-53MeV_SiL1-Ephi--40deg_SiL1Muons.root", "READ");
  //  TFile* file_MC = new TFile("Andy_Si16_MC_Ekin-1-50MeV_SiL1-Ephi--40deg_SiL1Muons.root", "READ");
  TFile* file_MC = new TFile("Andy_Si16_MC_Ekin-1-50MeV_SiL1-Ephi--40deg_ERes-80keV_SiL1Muons.root", "READ");
  TH1F* hSiL1_edep_MC = (TH1F*) file_MC->Get("hSiL1_All");
  hSiL1_edep_MC->Rebin(rebin_factor);
  hSiL1_edep_MC->GetXaxis()->SetRangeUser(min_edep, max_edep);
  hSiL1_edep_MC->SetLineColor(kRed);
  hSiL1_edep_MC->SetStats(false);
  int min_edep_bin_MC = hSiL1_edep_MC->FindBin(min_edep);
  int max_edep_bin_MC = hSiL1_edep_MC->FindBin(max_edep);
  double integral_MC = hSiL1_edep_MC->Integral(min_edep_bin_MC, max_edep_bin_MC);
  std::cout << "AE: Integral (MC) = " << integral_MC << std::endl;
  hSiL1_edep_MC->Sumw2();
  hSiL1_edep_MC->Scale(1.0/integral_MC);
  
  TFile* file_data = new TFile("Andy_Si16b_IncomingMuons_SiL1.root", "READ");
  TH1F* hSiL1_edep_data = (TH1F*) file_data->Get("hSiL1Energies");
  hSiL1_edep_data->Rebin(rebin_factor);
  hSiL1_edep_data->GetXaxis()->SetRangeUser(min_edep, max_edep);
  hSiL1_edep_data->SetStats(false);
  int min_edep_bin_data = hSiL1_edep_data->FindBin(min_edep);
  int max_edep_bin_data = hSiL1_edep_data->FindBin(max_edep);
  double integral_data = hSiL1_edep_data->Integral(min_edep_bin_data, max_edep_bin_data);
  std::cout << "AE: Integral (data) = " << integral_data << std::endl;
  hSiL1_edep_data->Sumw2();
  hSiL1_edep_data->Scale(1.0/integral_data);

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->Divide(1, 2);

  c1->cd(1);
  hSiL1_edep_data->Draw("HIST E");
  hSiL1_edep_MC->Draw("HIST E SAMES");

  TH1F* hSiL1_edep_residual = (TH1F*) hSiL1_edep_data->Clone("hSiL1_edep_residual");
  hSiL1_edep_residual->Add(hSiL1_edep_MC, -1);
  hSiL1_edep_residual->SetLineColor(kBlack);
  hSiL1_edep_residual->SetStats(false);
  hSiL1_edep_residual->SetTitle("Residual (Data - MC)");

  c1->cd(2);
  hSiL1_edep_residual->Draw("HIST E");
  int min_edep_bin_residual = hSiL1_edep_residual->FindBin(min_edep);
  int max_edep_bin_residual = hSiL1_edep_residual->FindBin(max_edep);
  double integral_residual = 0;
  for (int i_bin = min_edep_bin_residual; i_bin <= max_edep_bin_residual; ++i_bin) {
    integral_residual += std::fabs(hSiL1_edep_residual->GetBinContent(i_bin));
  }

  std::stringstream latex_text;
  latex_text.str("");
  latex_text << "Total Residual = " << integral_residual;
  TLatex* latex = new TLatex(700, 0, latex_text.str().c_str());
  latex->Draw();

  c1->cd(1);
  TLegend *leg = new TLegend(0.6,0.8,0.8,0.6);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.05);
  leg->SetFillColor(0);
  leg->AddEntry(hSiL1_edep_MC, "Monte Carlo", "l");
  leg->AddEntry(hSiL1_edep_data, "Data", "l");
  leg->AddEntry(hSiL1_edep_residual, "Data - Monte Carlo", "l");
  leg->Draw();

}
