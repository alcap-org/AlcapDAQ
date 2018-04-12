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
#include "TMath.h"

void Andy_Si16b_EvdEPlot_ThreeLayers(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");

  double min_x_energy = 0;
  double max_x_energy = 30000;
  double energy_width_x = 10;
  double min_y_energy = 0;
  double max_y_energy = 100;
  double energy_width_y = 0.5;
  int n_x_energy_bins = (max_x_energy - min_x_energy) / energy_width_x;
  int n_y_energy_bins = (max_y_energy - min_y_energy) / energy_width_y;

  double SiR1_thickness = 58; // um
  double SiR2_thickness = 1500; // um

  TH2F* hEvdE_Si16b_ThreeLayer = new TH2F("hEvdE_Si16b_ThreeLayer", "", n_x_energy_bins,min_x_energy,max_x_energy, n_y_energy_bins,min_y_energy,max_y_energy);
  hEvdE_Si16b_ThreeLayer->SetTitle("EvdE SiR Three Layers");
  hEvdE_Si16b_ThreeLayer->SetXTitle("E_{SiR1} + E_{SiR2} (+ E_{SiR3}) [keV]");
  hEvdE_Si16b_ThreeLayer->SetYTitle("E_{SiR1}/th_{SiR1} (+ E_{SiR2}/th_{SiR2}) [keV/#mu m]");


  // Define the various cuts
  double layer_coincidence_time = 200;

  SetTMEBranchAddresses(tmetree); // setup the branch addresses

  std::cout << "Total TMEs = " << tmetree->GetEntries() << std::endl;
  int n_entries = tmetree->GetEntries();
  for (int i_tme = 0; i_tme < n_entries; ++i_tme) {

    if (i_tme % 100000 == 0) {
      std::cout << i_tme << " / " << n_entries << std::endl;
    }

    tmetree->GetEntry(i_tme);
    CollectChannels(); // collect all channels into various groups (e.g. SiR1, SiL1, All) for easy looping

    std::vector<SimplePulse>* i_thick_pulse_list = SiR2;
    int n_thick_pulses = i_thick_pulse_list->size();

    std::vector<SimplePulse>* i_third_pulse_list = SiR3;
    int n_third_pulses = i_third_pulse_list->size();
    if (n_thick_pulses < 1) {
      continue;
    }

    for (int i_thin_channel = 0; i_thin_channel < n_SiR1_channels; ++i_thin_channel) {
      std::vector<SimplePulse>* i_thin_pulse_list = all_SiR1_channels[i_thin_channel];
      int n_thin_pulses = i_thin_pulse_list->size();

      if (n_thin_pulses < 1) {
	continue;
      }


      for (int i_thick_pulse = 0; i_thick_pulse < n_thick_pulses; ++i_thick_pulse) {
	double thick_time = i_thick_pulse_list->at(i_thick_pulse).tTME;
	double thick_energy = i_thick_pulse_list->at(i_thick_pulse).E;

	for (int i_thin_pulse = 0; i_thin_pulse < n_thin_pulses; ++i_thin_pulse) {
	  double thin_time = i_thin_pulse_list->at(i_thin_pulse).tTME;
	  double thin_energy = i_thin_pulse_list->at(i_thin_pulse).E;

	  if (std::fabs(thin_time - thick_time) < layer_coincidence_time) {
	    //	    std::cout << "Thin Hit: " << thin_energy << " keV, " << thin_time << " ns" << std::endl;
	    //	    std::cout << "Thick Hit: " << thick_energy << " keV, " << thick_time << " ns" << std::endl;

	    if (n_third_pulses != 0) {
	      for (int i_third_pulse = 0; i_third_pulse < n_third_pulses; ++i_third_pulse) {
		double third_time = i_third_pulse_list->at(i_third_pulse).tTME;
		double third_energy = i_third_pulse_list->at(i_third_pulse).E;
		if (std::fabs(third_time - thick_time) < layer_coincidence_time) {
		  hEvdE_Si16b_ThreeLayer->Fill(third_energy+thick_energy+thin_energy, (thin_energy+thick_energy)/(SiR1_thickness+SiR2_thickness));
		  continue;
		}
	      }
	    }
	    hEvdE_Si16b_ThreeLayer->Fill(thick_energy+thin_energy, thin_energy/SiR1_thickness);
	  }
	}
      }
    }
  }

  hEvdE_Si16b_ThreeLayer->Draw("COLZ");

}
