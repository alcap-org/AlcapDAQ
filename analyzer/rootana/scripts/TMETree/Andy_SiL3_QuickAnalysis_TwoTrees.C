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

#include "TH2.h"

void Andy_SiL3_QuickAnalysis(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");
  TTree* nontmetree = (TTree*) file->Get("TMETree/NonTMETree");

  double min_time = -30000;
  double max_time = 30000;
  double time_width = 200;
  int n_time_bins = (max_time - min_time) / time_width;

  double min_energy = 0;
  double max_energy = 20000;
  double energy_width = 100;
  int n_energy_bins = (max_energy - min_energy) / energy_width;

  TH2F* hTimeEnergy = new TH2F("hTimeEnergy", "", n_time_bins,min_time,max_time, n_energy_bins,min_energy,max_energy);
  hTimeEnergy->SetTitle("Andy, Run 9040, SiL3-S");
  hTimeEnergy->SetXTitle("Time [ns]");
  hTimeEnergy->SetYTitle("Energy [keV]");

  CollectChannels(); // collect all channels into various groups (e.g. SiR1, SiL1, All) for easy looping

  std::cout << "Total TMEs = " << tmetree->GetEntries() << std::endl;
  int n_blocks = 1;
  for (int i_block = 0; i_block < n_blocks; ++i_block) {
    std::cout << "Block #" << i_block << std::endl;
    int n_tme_entries = tmetree->GetEntries();
    for (int i_tme = 0; i_tme < n_tme_entries; ++i_tme) {
      //      if (i_tme % 100000 == 0) {
      //	std::cout << i_tme << " / " << n_entries << std::endl;
      //      }
      SetTMEBranchAddresses(tmetree); // setup the branch addresses
	
      tmetree->GetEntry(i_tme);

      if (blockId > i_block) {
	break;
      }

      std::vector<SimplePulse>* i_thick_pulse_list = SiL3;
      int n_thick_pulses = i_thick_pulse_list->size();
      if (n_thick_pulses < 1) {
	continue;
      }
      double this_tme_time = centralMuonTime;
      std::cout << "\t TME #" << TMEId << std::endl;
      std::cout << "\t\t Central Muon Time = " << this_tme_time << std::endl;
      std::cout << "\t\t SiL3 Pulse Energies [keV] = ";
      for (int i_thick_pulse = 0; i_thick_pulse < n_thick_pulses; ++i_thick_pulse) {
	std::cout << i_thick_pulse_list->at(i_thick_pulse).E << " ";
      }
      std::cout << std::endl;
      std::cout << "\t\t SiL3 Pulse TME Times [ns] = ";
      for (int i_thick_pulse = 0; i_thick_pulse < n_thick_pulses; ++i_thick_pulse) {
	std::cout << i_thick_pulse_list->at(i_thick_pulse).tTME << " ";
      }
      std::cout << std::endl;

      std::cout << "\t NonTME" << std::endl;
      std::cout << "\t\t SiL3 Energies [keV] = ";

      int n_nontme_entries = nontmetree->GetEntries();
      for (int i_nontme = 0; i_nontme < n_nontme_entries; ++i_nontme) {
	SetTMEBranchAddresses(nontmetree); // setup the branch addresses

	nontmetree->GetEntry(i_nontme);
	if (blockId != i_block) {
	  break;
	}
	std::vector<SimplePulse>* i_leftover_pulse_list = SiL3;
	int n_leftover_pulses = i_leftover_pulse_list->size();
	for (int i_leftover_pulse = 0; i_leftover_pulse < n_leftover_pulses; ++i_leftover_pulse) {
	  double this_leftover_time = i_leftover_pulse_list->at(i_leftover_pulse).tblock;
	  double this_leftover_energy = i_leftover_pulse_list->at(i_leftover_pulse).E;
	  if (this_leftover_time < this_tme_time) {
	    continue;
	  }
	  std::cout << this_leftover_energy << " ";
	}
	std::cout << std::endl;
      }
      std::cout << "\t\t SiL3 Pulse Block Times [ns] = ";
      for (int i_nontme = 0; i_nontme < n_nontme_entries; ++i_nontme) {
	SetTMEBranchAddresses(nontmetree); // setup the branch addresses

	nontmetree->GetEntry(i_nontme);
	if (blockId != i_block) {
	  break;
	}
	std::vector<SimplePulse>* i_leftover_pulse_list = SiL3;
	int n_leftover_pulses = i_leftover_pulse_list->size();
	for (int i_leftover_pulse = 0; i_leftover_pulse < n_leftover_pulses; ++i_leftover_pulse) {
	  double this_leftover_time = i_leftover_pulse_list->at(i_leftover_pulse).tblock;
	  if (this_leftover_time < this_tme_time) {
	    continue;
	  }
	  std::cout << this_leftover_time << " (" << this_leftover_time - this_tme_time << ") ";
	}
	std::cout << std::endl;
      }
    }
  }


	/*    std::vector<SimplePulse>* i_muSc_pulse_list = muSc;
    int n_muSc_pulses = i_muSc_pulse_list->size();

    if (n_thick_pulses <= 1 || n_muSc_pulses>1) {
      continue;
    }

    // Check for any double-counted pulses
    if(anyDoubleCountedPulses) {
      continue;
    }
    /*
    bool double_counted = false;
    for (int i_thick_pulse = 0; i_thick_pulse < n_thick_pulses; ++i_thick_pulse) {
      int bit_mask = i_thick_pulse_list->at(i_thick_pulse).bit_mask;
      if (bit_mask == kDoubleCounted) {
	double_counted = true;
	break;
      }
    }
    if (double_counted) {
      continue;
    }
    

    // Look for the muon
    double min_muon_energy = 3000;
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

    // Plot the energy and time difference of everything else
    for (int i_thick_pulse = 0; i_thick_pulse < n_thick_pulses; ++i_thick_pulse) {
      if (i_thick_pulse != muon_element) {
	double thick_time = i_thick_pulse_list->at(i_thick_pulse).tTME;
	double thick_energy = i_thick_pulse_list->at(i_thick_pulse).E;
	//      	std::cout << "Proton? E = " << thick_energy << ", t = " << thick_time << ", dt = " << thick_time - muon_time << std::endl;
	if (thick_energy > max_muon_energy) {
	  hTimeEnergy->Fill(thick_time - muon_time, thick_energy);
	}
      }
    }
  }

  hTimeEnergy->Draw("COLZ");
	*/
}
