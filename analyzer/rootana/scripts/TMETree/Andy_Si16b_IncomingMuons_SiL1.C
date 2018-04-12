// A first example macro
// -- it loops through the first 100 TMEs in the TMETree and will print all its information
// To use (from analyzer/rootana):
// > root -l -b
// > .L scripts/TMETree/FirstMacro.C+
// > FirstMacro("out_tme-tree.root")

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"

#include "AlCapConstants.h"
#include "TMETreeBranches.h"

#include <iostream>

void Andy_Si16b_IncomingMuons_SiL1() {

  std::string filename = "~/data/out/v10/Si16b.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");

  SetTMEBranchAddresses(tmetree); // setup the branch addresses

  double min_energy = 0;
  double max_energy = 10000;
  double energy_width = 1;
  int n_energy_bins = (max_energy - min_energy) / energy_width;
  TH1F* hSiL1Energies = new TH1F("hSiL1Energies", "Energy of the SiL1 Hits (bit_mask==1)", n_energy_bins,min_energy,max_energy);
  hSiL1Energies->SetXTitle("Central Muon Energy [keV]");
  hSiL1Energies->SetYTitle("Raw Count");
  hSiL1Energies->SetLineWidth(2);
  hSiL1Energies->SetLineColor(kBlue);

  double min_time = -20000;
  double max_time = 20000;
  double time_width = 10;
  int n_time_bins = (max_time - min_time) / time_width;
  TH2F* hSiL1Energies_wSiR1Hit = new TH2F("hSiL1Energies_wSiR1Hit", "Energy of the SiL1 Hits (bit_mask==1 and a hit in SiR1)", n_time_bins,min_time,max_time, n_energy_bins,min_energy,max_energy);
  hSiL1Energies_wSiR1Hit->SetYTitle("Central Muon Energy [keV]");
  hSiL1Energies_wSiR1Hit->SetXTitle("TME Time [ns]");
  hSiL1Energies_wSiR1Hit->SetLineWidth(2);
  hSiL1Energies_wSiR1Hit->SetLineColor(kBlue);

  int n_entries = tmetree->GetEntries();
  for (int i_tme = 0; i_tme < n_entries; ++i_tme) {
    if (i_tme % 100000 == 0) {
      std::cout << i_tme << " / " << n_entries << std::endl;
    }
    tmetree->GetEntry(i_tme);
    CollectChannels(); // collect all channels into various groups (e.g. SiL1, SiR1, All) for easy looping

    /*    if (anyDoubleCountedPulses) {
      continue;
    }
    */
    for(int i_SiL1_channel = 0; i_SiL1_channel < n_SiL1_channels; ++i_SiL1_channel) {
      std::vector<SimplePulse>* i_pulse_list = all_SiL1_channels[i_SiL1_channel];
      int n_pulses = i_pulse_list->size();
      if (n_pulses > 0) {
	for (int i_pulse = 0; i_pulse < n_pulses; ++i_pulse) {
	  double energy = i_pulse_list->at(i_pulse).E;
	  int bit_mask = i_pulse_list->at(i_pulse).bit_mask;
	  double time = i_pulse_list->at(i_pulse).tTME;

	  if (bit_mask == 1) {
	    hSiL1Energies->Fill(energy);

	    // Loop through the SiR1 channels and see if there are any hits there
	    for (int i_SiR1_channel = 0; i_SiR1_channel < n_SiR1_channels; ++i_SiR1_channel) {
	      std::vector<SimplePulse>* i_SiR1_pulse_list = all_SiR1_channels[i_SiR1_channel];
	      if (i_SiR1_pulse_list->size() > 0) {
		hSiL1Energies_wSiR1Hit->Fill(time, energy);
	      }
	    }
	  }
	}
      }
    }
  }

  //  hSiL1Energies->Draw();
  //  hSiL1Energies_wSiR1Hit->SetLineColor(kRed);
  hSiL1Energies_wSiR1Hit->Draw("COLZ");
  //  std::cout << "Total Number of Incoming Muons = " << hSiL1Energies->Integral() << std::endl;
  
  /*  double spectrum_max = hSiL1Energies_Zoom->GetMaximum();
  double spectrum_max_pos = hSiL1Energies_Zoom->GetBinCenter(hSiL1Energies_Zoom->GetMaximumBin());
  TF1* gaussian = new TF1("gaussian", "[0]*TMath::Gaus(x,[1],[2])");
  gaussian->SetParameters(spectrum_max, spectrum_max_pos, 100);
  hSiL1Energies_Zoom->Fit(gaussian, "Q", "", spectrum_max_pos-100, spectrum_max_pos+100);
  std::cout << "Punch Through Peak is at " << gaussian->GetParameter(1) << " keV" << std::endl;
  std::cout << "Punch Through Peak widhth is " << gaussian->GetParameter(2) << " keV" << std::endl;
  */

  TFile* output = new TFile("Andy_Si16b_IncomingMuons_SiL1.root", "RECREATE");
  hSiL1Energies->Write();
  hSiL1Energies_wSiR1Hit->Write();
  output->Write();
  output->Close();
}
