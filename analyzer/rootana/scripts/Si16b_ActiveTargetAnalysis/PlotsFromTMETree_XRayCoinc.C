// EvdE plot for Si16b dataset
// To use (from analyzer/rootana):
// > root -l -b
// > .L scripts/TMETree/FirstMacro.C+
// > FirstMacro("out_tme-tree.root")

#include "TFile.h"
#include "TTree.h"

#include "scripts/TMETree/AlCapConstants.h"
#include "scripts/TMETree/TMETreeBranches.h"

#include <iostream>
#include <sstream>

#include "TH2.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLatex.h"

void PlotsFromTMETree_XRayCoinc() {

  std::string filename = "~/data/out/local/out10404_tme-tree.root";
  //  std::string outfilename = "scripts/Si16b_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run10404.root";

  // The cuts I'm doing
  double min_xray_energy = 397; // clooking for the 2p-1s x-ray
  double max_xray_energy = 403;
  double time_cut = 200;
  //  int n_stopped_muons = 0;
  //  double overall_energy_cut = 100; // only accept energy deposits greater than this (to remove noise)
  
  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");

  double min_time = -20000;
  double max_time = 20000;
  double time_width = 100;
  int n_time_bins = (max_time - min_time) / time_width;

  double min_energy = 0;
  double max_energy = 10000;
  double energy_width = 100;
  int n_energy_bins = (max_energy - min_energy) / energy_width;

  TH2F* hTimeEnergy_coinc = new TH2F("hTimeEnergy_coinc", "", n_time_bins,min_time,max_time, n_energy_bins,min_energy,max_energy);
  hTimeEnergy_coinc->SetTitle("");
  hTimeEnergy_coinc->SetXTitle("Time [ns]");
  hTimeEnergy_coinc->SetYTitle("Energy [keV]");
  TH2F* hTimeEnergy_noncoinc = new TH2F("hTimeEnergy_noncoinc", "", n_time_bins,min_time,max_time, n_energy_bins,min_energy,max_energy);
  hTimeEnergy_noncoinc->SetTitle("");
  hTimeEnergy_noncoinc->SetXTitle("Time [ns]");
  hTimeEnergy_noncoinc->SetYTitle("Energy [keV]");

  SetTMEBranchAddresses(tmetree); // setup the branch addresses
  CollectChannels(); // collect all channels into various groups (e.g. SiR1, SiL1, All) for easy looping

  std::cout << "Total TMEs = " << tmetree->GetEntries() << std::endl;
  int n_tme_entries = tmetree->GetEntries();
  for (int i_tme = 0; i_tme < n_tme_entries; ++i_tme) {
    if (i_tme % 100000 == 0) {
      std::cout << i_tme << " / " << n_tme_entries << std::endl;
    }
    tmetree->GetEntry(i_tme);

    int n_GeLoGain_pulses = GeLoGain->size();
    bool xray = false;
    for (int i_GeLoGain_pulse = 0; i_GeLoGain_pulse < n_GeLoGain_pulses; ++i_GeLoGain_pulse) {
      double GeLoGain_energy = GeLoGain->at(i_GeLoGain_pulse).E;
      if (GeLoGain_energy < min_xray_energy || GeLoGain_energy > max_xray_energy) {
	xray = false;
	//	continue; // not the x-ray we're looking for, skip to the next pulse
      }
      else {
	xray = true;
      }
      double GeLoGain_time = GeLoGain->at(i_GeLoGain_pulse).tTME;
      
      for (int i_SiL1_channel = 0; i_SiL1_channel < n_SiL1_channels; ++i_SiL1_channel) {
	std::vector<SimplePulse>* i_target_pulse_list = all_SiL1_channels[i_SiL1_channel];
	int n_target_pulses = i_target_pulse_list->size();
      
	for (int i_target_pulse = 0; i_target_pulse < n_target_pulses; ++i_target_pulse) {      
	  double target_time = i_target_pulse_list->at(i_target_pulse).tTME;
	  double target_energy = i_target_pulse_list->at(i_target_pulse).E;

	  if (xray) {
	    hTimeEnergy_coinc->Fill(target_time, target_energy);
	  }
	  else {
	    hTimeEnergy_noncoinc->Fill(target_time, target_energy);
	  }
	}
      }
    }
  }

  hTimeEnergy_coinc->Draw("COLZ");

  int min_time_cut_bin = hTimeEnergy_coinc->GetXaxis()->FindBin(-time_cut);
  int max_time_cut_bin = hTimeEnergy_coinc->GetXaxis()->FindBin(time_cut);

  TH1D* hStoppedMuonEnergy = hTimeEnergy_coinc->ProjectionY("hStoppedMuonEnergy", min_time_cut_bin, max_time_cut_bin);
  hStoppedMuonEnergy->Draw();
  double peak_location = hStoppedMuonEnergy->GetBinCenter(hStoppedMuonEnergy->GetMaximumBin());
  TF1* fit = new TF1("fit", "[0]*TMath::Gaus(x, [1], [2])", peak_location-200, peak_location+200);
  fit->SetParameters(hStoppedMuonEnergy->GetMaximum(), peak_location, 10);
  hStoppedMuonEnergy->Fit(fit, "R");

  TH1D* hNotStoppedMuonEnergy = hTimeEnergy_noncoinc->ProjectionY("hNotStoppedMuonEnergy", min_time_cut_bin, max_time_cut_bin);
  
  /*  TFile* output = new TFile(outfilename.c_str(), "RECREATE");
  hTimeEnergy_coinc->Write();
  
  TTree* tree = new TTree("cuts", "cuts");
  tree->Branch("n_total_tmes", &n_total_tmes);
  tree->Branch("n_analysed_tmes", &n_analysed_tmes);
  tree->Branch("max_central_muon_pulses", &max_central_muon_pulses);
  tree->Branch("n_stopped_muons", &n_stopped_muons);
  tree->Branch("min_muon_energy", &min_muon_energy);
  tree->Branch("max_muon_energy", &max_muon_energy);
  tree->Branch("muon_time_cut", &muon_time_cut);
  tree->Branch("overall_energy_cut", &overall_energy_cut);
  tree->Fill();
  tree->Write();

  output->Write();
  output->Close();
  */
}
