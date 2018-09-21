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

void Andy_SiL3_QuickAnalysis_wTemplates() {

  //  std::string filename = "~/data/out/v5/SiL3_tmetree.root";
  std::string filename = "~/data/out/local/out09040_template-fits.root";
  //    std::string filename = "~/data/out/local/out09040_template-fits_w-tap-tree.root";

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");

  double min_time = -30000;
  double max_time = 30000;
  double time_width = 200;
  int n_time_bins = (max_time - min_time) / time_width;

  double min_energy = 0;
  double max_energy = 30000;
  double energy_width = 100;
  int n_energy_bins = (max_energy - min_energy) / energy_width;

  TH2F* hTimeEnergy = new TH2F("hTimeEnergy", "", n_time_bins,min_time,max_time, n_energy_bins,min_energy,max_energy);
  hTimeEnergy->SetTitle("Andy, SiL3 Dataset, SiL3-S");
  hTimeEnergy->SetXTitle("Time [ns]");
  hTimeEnergy->SetYTitle("Energy [keV]");

  SetTMEBranchAddresses(tmetree); // setup the branch addresses
  CollectChannels(); // collect all channels into various groups (e.g. SiR1, SiL1, All) for easy looping

  TH1F* hNormalisation = new TH1F("hNormalisation", "", 3,0,3);
  hNormalisation->GetXaxis()->SetBinLabel(1, "All TMEs");
  hNormalisation->GetXaxis()->SetBinLabel(2, "TMEs w/ 1 muSc hit");
  hNormalisation->GetXaxis()->SetBinLabel(3, "SiL3 hits w 3000 < E < 6000 keV");
  
  
  std::cout << "Total TMEs = " << tmetree->GetEntries() << std::endl;
  int n_tme_entries = tmetree->GetEntries();
  for (int i_tme = 0; i_tme < n_tme_entries; ++i_tme) {
    if (i_tme % 100000 == 0) {
      std::cout << i_tme << " / " << n_tme_entries << std::endl;
    }
    tmetree->GetEntry(i_tme);

    hNormalisation->Fill("All TMEs", 1);

    std::vector<SimplePulse>* i_thick_pulse_list = SiL3;
    int n_thick_pulses = i_thick_pulse_list->size();

    std::vector<SimplePulse>* i_muSc_pulse_list = muSc;
    int n_muSc_pulses = i_muSc_pulse_list->size();
    if (n_muSc_pulses > 1) {
      continue;
    }

    hNormalisation->Fill("TMEs w/ 1 muSc hit", 1);
    
    // Look for the muon
    double min_muon_energy = 3000;
    double max_muon_energy = 6000;
    int muon_element = -1;
    double muon_time = 0;
    for (int i_thick_pulse = 0; i_thick_pulse < n_thick_pulses; ++i_thick_pulse) {      
      double thick_time = i_thick_pulse_list->at(i_thick_pulse).tTME;
      double thick_energy = i_thick_pulse_list->at(i_thick_pulse).E;
      if (thick_energy > min_muon_energy && thick_energy < max_muon_energy && std::fabs(thick_time)<200) {
	muon_element = i_thick_pulse;
	muon_time = thick_time;

	hNormalisation->Fill("SiL3 hits w 3000 < E < 6000 keV", 1);
	//	std::cout << "Muon! E = " << thick_energy << ", t = " << thick_time << std::endl;
      }
    }
    /*
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
	if ( thick_time>1000 && thick_time<4000) {
	  hTimeEnergy->Fill(thick_time, thick_energy);
	  //	  hTimeEnergy->Fill(thick_time-muon_time, thick_energy);
	}
	//      }
    }
    //    }
  }

  hTimeEnergy->Draw("COLZ");
  hNormalisation->Draw("HIST TEXT");
  
  TFile* output = new TFile("Andy_SiL3_QuickAnalysis_wTemplates.root", "RECREATE");
  hTimeEnergy->Write();
  hNormalisation->Draw();
  output->Write();
  output->Close();
}
