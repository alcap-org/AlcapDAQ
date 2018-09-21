// EvdE plot for Si16b dataset
// To use (from analyzer/rootana):
// > root -l -b
// > .L scripts/TMETree/FirstMacro.C+
// > FirstMacro("out_tme-tree.root")

#include "TFile.h"
#include "TTree.h"

#include "AlCapConstants.h"
#include "TMETreeBranches.h"

#include <iostream>
#include <sstream>

#include "TH2.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLatex.h"

void Andy_SiL3_QuickAnalysis() {

  std::string filename = "~/data/out/v5/SiL3_tmetree.root";

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");

  double min_time = -30000;
  double max_time = 30000;
  double time_width = 250;
  int n_time_bins = (max_time - min_time) / time_width;

  double min_energy = 0;
  double max_energy = 20000;
  double energy_width = 100;
  int n_energy_bins = (max_energy - min_energy) / energy_width;

  TH2F* hTimeEnergy = new TH2F("hTimeEnergy", "", n_time_bins,min_time,max_time, n_energy_bins,min_energy,max_energy);
  hTimeEnergy->SetTitle("Andy, SiL3 Dataset, SiL3-S");
  hTimeEnergy->SetXTitle("Time [ns]");
  hTimeEnergy->SetYTitle("Energy [keV]");

  SetTMEBranchAddresses(tmetree); // setup the branch addresses
  CollectChannels(); // collect all channels into various groups (e.g. SiR1, SiL1, All) for easy looping

  std::cout << "Total TMEs = " << tmetree->GetEntries() << std::endl;
  int n_tme_entries = tmetree->GetEntries();
  for (int i_tme = 0; i_tme < n_tme_entries; ++i_tme) {
    if (i_tme % 100000 == 0) {
      std::cout << i_tme << " / " << n_tme_entries << std::endl;
    }
    tmetree->GetEntry(i_tme);

    std::vector<SimplePulse>* i_thick_pulse_list = SiL3;
    int n_thick_pulses = i_thick_pulse_list->size();

    /*    std::vector<SimplePulse>* i_muSc_pulse_list = muSc;
    int n_muSc_pulses = i_muSc_pulse_list->size();
    if (n_muSc_pulses > 1) {
      continue;
    }
    */
    
    // Look for the muon
    /*    double min_muon_energy = 3000;
    double max_muon_energy = 6000;
    int muon_element = -1;
    double muon_time = 0;
    for (int i_thick_pulse = 0; i_thick_pulse < n_thick_pulses; ++i_thick_pulse) {      
      double thick_time = i_thick_pulse_list->at(i_thick_pulse).tTME;
      double thick_energy = i_thick_pulse_list->at(i_thick_pulse).E;
      if (thick_energy > min_muon_energy && thick_energy < max_muon_energy) {
	muon_element = i_thick_pulse;
	muon_time = thick_time;
	//	std::cout << "Muon! E = " << thick_energy << ", t = " << thick_time << std::endl;
      }
    }
    if (muon_element == -1) {
      continue; // didn't find a muon
    }
    */
    // Plot the energy and time difference of everything else
    for (int i_thick_pulse = 0; i_thick_pulse < n_thick_pulses; ++i_thick_pulse) {
      //      if (i_thick_pulse != muon_element) {
	double thick_time = i_thick_pulse_list->at(i_thick_pulse).tTME;
	double thick_energy = i_thick_pulse_list->at(i_thick_pulse).E;
	double thick_bit_mask = i_thick_pulse_list->at(i_thick_pulse).bit_mask;
	//      	std::cout << "Proton? E = " << thick_energy << ", t = " << thick_time << ", dt = " << thick_time - muon_time << std::endl;
	if (thick_energy > 5000 && thick_bit_mask==1) {
	  //	  hTimeEnergy->Fill(thick_time - muon_time, thick_energy);
	  hTimeEnergy->Fill(thick_time, thick_energy);
	}
	//      }
    }
  }

  hTimeEnergy->Draw("COLZ");
  //  TFile* output = new TFile("Andy_SiL3_QuickAnalysis.root", "RECREATE");
  //  hTimeEnergy->Write();
  //  output->Write();
  //  output->Close();
}
