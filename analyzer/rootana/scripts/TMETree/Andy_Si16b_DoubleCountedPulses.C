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

void Andy_Si16b_DoubleCountedPulses() {

  std::string filename = "~/data/out/v10/Si16b.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");

  double min_time = -20000;
  double max_time = 20000;
  double time_width = 200;
  int n_time_bins = (max_time - min_time) / time_width;


  TH2F* hDoubleCounted = new TH2F("hDoubleCounted", "", n_time_bins,min_time,max_time, n_time_bins,min_time,max_time);
  hDoubleCounted->SetTitle("DoubleCounted");
  hDoubleCounted->SetXTitle("TME 1 Time [ns]");
  hDoubleCounted->SetYTitle("TME 2 Time [ns]]");

  std::vector<int> found_tpis;
  std::vector<float> found_tpi_times;

  SetTMEBranchAddresses(tmetree); // setup the branch addresses

  std::cout << "Total TMEs = " << tmetree->GetEntries() << std::endl;
  int n_entries = tmetree->GetEntries();
  for (int i_tme = 0; i_tme < n_entries; ++i_tme) {

    if (i_tme % 100000 == 0) {
      std::cout << i_tme << " / " << n_entries << std::endl;
    }

    tmetree->GetEntry(i_tme);
    CollectChannels(); // collect all channels into various groups (e.g. SiR1, SiL1, All) for easy looping

    for (int i_thin_channel = 0; i_thin_channel < 1;/*n_SiR1_channels;*/ ++i_thin_channel) {
      std::vector<SimplePulse>* i_thin_pulse_list = all_SiR1_channels[i_thin_channel];
      int n_thin_pulses = i_thin_pulse_list->size();

      if (n_thin_pulses < 1) {
	continue;
      }


      for (int i_thin_pulse = 0; i_thin_pulse < n_thin_pulses; ++i_thin_pulse) {
	double thin_time = i_thin_pulse_list->at(i_thin_pulse).tTME;
	double thin_energy = i_thin_pulse_list->at(i_thin_pulse).E;
	int thin_bit_mask = i_thin_pulse_list->at(i_thin_pulse).bit_mask;
	int thin_tpi_id = i_thin_pulse_list->at(i_thin_pulse).tpi_id;

	if (thin_bit_mask == 2) {
	  // Check for this TPI ID
	  bool found = false;
	  int i_found_tpi;
	  for (i_found_tpi = 0; i_found_tpi < found_tpis.size(); ++i_found_tpi) {
	    if (thin_tpi_id == found_tpis.at(i_found_tpi)) {
	      found = true;
	      break;
	    }
	  }

	  if (!found) {
	    found_tpis.push_back(thin_tpi_id);
	    found_tpi_times.push_back(thin_time);
	  }
	  else {
	    hDoubleCounted->Fill(found_tpi_times.at(i_found_tpi), thin_time);
	    found_tpis.at(i_found_tpi) = -1; // nullify it
	  }
	}
      }
    }

    // Clean up vectors
    int final_size = found_tpis.size();
    for (std::vector<int>::const_reverse_iterator i_tpi = found_tpis.rbegin(); i_tpi != found_tpis.rend(); ++i_tpi) {
      if (*i_tpi == -1) {
	--final_size;
      }
    }
    found_tpis.resize(final_size);
    found_tpi_times.resize(final_size);
  }

  hDoubleCounted->Draw("COLZ");

  /*  TFile* out_file = new TFile("Andy_Si16b_EvdEvtPlot.root", "RECREATE");
  hEvdEvt_Si16b_Veto->Write();
  hEvdEvt_Si16b_NoVeto->Write();
  hEvdEvt_Si16b_ZoomNoVeto->Write();
  hEvdEvt_Si16b_ZoomVeto->Write();
  hEvdEvt_Si16b_Proton_NoVeto->Write();
  hEvdEvt_Si16b_Proton_Veto->Write();

  out_file->Write();
  out_file->Close();
  */
}
