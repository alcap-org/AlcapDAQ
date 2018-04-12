// EvdE plot for Si16b dataset
// To use (from analyzer/rootana):
// > root -l -b
// > .L scripts/TMETree/FirstMacro.C+
// > FirstMacro("out_tme-tree.root")

#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <sstream>

#include "TH2.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TLine.h"

void CountStoppedMuons_DirectTMEs() {

  //////////////////////////////
  // User parameters
  std::string filename = "~/data/results/SiL3_active/EventSelection_NewCuts.root";
  double min_energy_range = 0;
  double max_energy_range = 10000;

  
  TFile* file = new TFile(filename.c_str(), "READ");

  TH2F* hTimeEnergy = (TH2F*) file->Get("hTimeEnergy");
  TTree* cuttree = (TTree*) file->Get("cuts");

  int n_stopped_muons = 0;
  double min_muon_energy = 0;
  double max_muon_energy = 0;
  double muon_time_cut = 0;
  cuttree->SetBranchAddress("n_stopped_muons", &n_stopped_muons);
  cuttree->SetBranchAddress("min_muon_energy", &min_muon_energy);
  cuttree->SetBranchAddress("max_muon_energy", &max_muon_energy);
  cuttree->SetBranchAddress("muon_time_cut", &muon_time_cut);
  cuttree->GetEntry(0);


  TCanvas* c_muon = new TCanvas("c_muon", "c_muon");
  c_muon->Divide(2, 1);
  c_muon->cd(1);
  c_muon->GetPad(1)->SetLogz();
  
  TH2F* hTimeEnergy_Zoom = (TH2F*) hTimeEnergy->Clone("hTimeEnergy_Zoom");
  hTimeEnergy_Zoom->SetStats(false);
  hTimeEnergy_Zoom->GetXaxis()->SetRangeUser(-5000,5000);
  hTimeEnergy_Zoom->GetYaxis()->SetRangeUser(min_energy_range, max_energy_range);
  hTimeEnergy_Zoom->Draw("COLZ");

  TLine* min_muon_time_line = new TLine(-muon_time_cut, min_energy_range, -muon_time_cut, max_energy_range);
  min_muon_time_line->SetLineColor(kRed);
  min_muon_time_line->SetLineWidth(2);
  min_muon_time_line->Draw("LSAME");
  TLine* max_muon_time_line = new TLine(muon_time_cut, min_energy_range, muon_time_cut, max_energy_range);
  max_muon_time_line->SetLineColor(kRed);
  max_muon_time_line->SetLineWidth(2);
  max_muon_time_line->Draw("LSAME");

  c_muon->cd(2);
  int min_muon_time_cut_bin = hTimeEnergy->GetXaxis()->FindBin(-muon_time_cut);
  int max_muon_time_cut_bin = hTimeEnergy->GetXaxis()->FindBin(muon_time_cut);
  TH1D* hMuonEnergy = hTimeEnergy->ProjectionY("hMuonEnergy", min_muon_time_cut_bin, max_muon_time_cut_bin);
  hMuonEnergy->SetStats(false);
  hMuonEnergy->GetXaxis()->SetRangeUser(min_energy_range, max_energy_range);
  hMuonEnergy->Draw();
  TLine* min_muon_energy_line = new TLine(min_muon_energy, 0, min_muon_energy, hMuonEnergy->GetMaximum());
  min_muon_energy_line->SetLineColor(kRed);
  min_muon_energy_line->SetLineWidth(2);
  min_muon_energy_line->Draw("LSAME");
  TLine* max_muon_energy_line = new TLine(max_muon_energy, 0, max_muon_energy, hMuonEnergy->GetMaximum());
  max_muon_energy_line->SetLineColor(kRed);
  max_muon_energy_line->SetLineWidth(2);
  max_muon_energy_line->Draw("LSAME");

  std::stringstream text;
  text.str("");
  text << "N_{stop #mu} = (" << std::fixed << std::setprecision(2) << n_stopped_muons/1e6 << /*" #pm " << std::setprecision(1) << n_stopped_muons_error/1e6 <<*/ ") #times 10^{6}";
  TLatex* mustop_text = new TLatex(8000, 2e6, text.str().c_str());
  mustop_text->SetTextAlign(22);
  mustop_text->SetTextSize(0.05);
  mustop_text->Draw("");

  std::cout << n_stopped_muons << std::endl;

}
