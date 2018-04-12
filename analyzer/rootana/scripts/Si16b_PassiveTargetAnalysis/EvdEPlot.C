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

#include "TH2.h"
#include "TMath.h"

struct FullPulseId {
  FullPulseId(int r, int b, int t) : run_id(r), block_id(b), tpi_id(t) {}
  int run_id;
  int block_id;
  int tpi_id;
};

void EvdEPlot() {

  /////////////////////////////
  // User parameters
  std::string filename = "~/data/out/v10/Si16b.root";
  std::string outfilename = "~/data/results/Si16b_passive/EvdEPlots.root";
  double layer_coincidence_time = 200; // coincidence time between layers
  double x_1 = 0, y_1 = 2250, x_2 = 5500, y_2 = 0; // parameters for the diagonal electron spot cut
  double electron_spot_gradient = (y_2 - y_1) / (x_2 - x_1);
  double electron_spot_yoffset = y_1;
  double punch_through_cut = 300; // parameters for the horizontal punch through proton cut
  double deuteron_cut_peak = 4500; // parameters for the exponential remove deuteron cut
  double deuteron_cut_slope = -0.0004;
  double deuteron_cut_yoffset = 500;
  //  double pb_time_cut = 200; // early time cut to remove lead contamination

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");

  double min_x_energy = 0;
  double max_x_energy = 20000;
  double min_y_energy = 0;
  double max_y_energy = 10000;
  double energy_width = 10;
  int n_x_energy_bins = (max_x_energy - min_x_energy) / energy_width;
  int n_y_energy_bins = (max_y_energy - min_y_energy) / energy_width;

  TH2F* hEvdE_Si16b_Veto = new TH2F("hEvdE_Si16b_Veto", "", n_x_energy_bins,min_x_energy,max_x_energy, n_y_energy_bins,min_y_energy,max_y_energy);
  hEvdE_Si16b_Veto->SetTitle("EvdE SiR (w/ SiR3 Veto)");
  hEvdE_Si16b_Veto->SetXTitle("E_{SiR1} + E_{SiR2} [keV]");
  hEvdE_Si16b_Veto->SetYTitle("E_{SiR1} [keV]");

  TH2F* hEvdE_Si16b_NoVeto = new TH2F("hEvdE_Si16b_NoVeto", "", n_x_energy_bins,min_x_energy,max_x_energy, n_y_energy_bins,min_y_energy,max_y_energy);
  hEvdE_Si16b_NoVeto->SetTitle("EvdE SiR (w/o SiR3 Veto)");
  hEvdE_Si16b_NoVeto->SetXTitle("E_{SiR1} + E_{SiR2} [keV]");
  hEvdE_Si16b_NoVeto->SetYTitle("E_{SiR1} [keV]");

  double min_x_energy_zoom = 0;
  double max_x_energy_zoom = 20000;
  double min_y_energy_zoom = 0;
  double max_y_energy_zoom = 3000;
  double energy_zoom_width = 10;
  int n_x_energy_zoom_bins = (max_x_energy_zoom - min_x_energy_zoom) / energy_zoom_width;
  int n_y_energy_zoom_bins = (max_y_energy_zoom - min_y_energy_zoom) / energy_zoom_width;
  TH2F* hEvdE_Si16b_ZoomNoVeto = new TH2F("hEvdE_Si16b_ZoomNoVeto", "", n_x_energy_zoom_bins,min_x_energy_zoom,max_x_energy_zoom, n_y_energy_zoom_bins,min_y_energy_zoom,max_y_energy_zoom);
  hEvdE_Si16b_ZoomNoVeto->SetTitle("EvdE SiR (w/o SiR3 Veto)");
  hEvdE_Si16b_ZoomNoVeto->SetXTitle("E_{SiR1} + E_{SiR2} [keV]");
  hEvdE_Si16b_ZoomNoVeto->SetYTitle("E_{SiR1} [keV]");

  TH2F* hEvdE_Si16b_ZoomVeto = new TH2F("hEvdE_Si16b_ZoomVeto", "", n_x_energy_zoom_bins,min_x_energy_zoom,max_x_energy_zoom, n_y_energy_zoom_bins,min_y_energy_zoom,max_y_energy_zoom);
  hEvdE_Si16b_ZoomVeto->SetTitle("EvdE SiR (w/ SiR3 Veto)");
  hEvdE_Si16b_ZoomVeto->SetXTitle("E_{SiR1} + E_{SiR2} [keV]");
  hEvdE_Si16b_ZoomVeto->SetYTitle("E_{SiR1} [keV]");

  TH2F* hEvdE_Si16b_Proton_NoVeto = new TH2F("hEvdE_Si16b_Proton_NoVeto", "", n_x_energy_zoom_bins,min_x_energy_zoom,max_x_energy_zoom, n_y_energy_zoom_bins,min_y_energy_zoom,max_y_energy_zoom);
  hEvdE_Si16b_Proton_NoVeto->SetTitle("EvdE Protons SiR (w/o SiR3 Veto)");
  hEvdE_Si16b_Proton_NoVeto->SetXTitle("E_{SiR1} + E_{SiR2} [keV]");
  hEvdE_Si16b_Proton_NoVeto->SetYTitle("E_{SiR1} [keV]");

  TH2F* hEvdE_Si16b_Proton_Veto = new TH2F("hEvdE_Si16b_Proton_Veto", "", n_x_energy_zoom_bins,min_x_energy_zoom,max_x_energy_zoom, n_y_energy_zoom_bins,min_y_energy_zoom,max_y_energy_zoom);
  hEvdE_Si16b_Proton_Veto->SetTitle("EvdE Protons SiR (w/ SiR3 Veto)");
  hEvdE_Si16b_Proton_Veto->SetXTitle("E_{SiR1} + E_{SiR2} [keV]");
  hEvdE_Si16b_Proton_Veto->SetYTitle("E_{SiR1} [keV]");

  double min_time = -30000;
  double max_time = 30000;
  double time_width = 10;
  int n_time_bins = (max_time - min_time) / time_width;
  TH1F* hTime_Proton_NoVeto = new TH1F("hTime_Proton_NoVeto", "", n_time_bins,min_time,max_time);
  hTime_Proton_NoVeto->SetTitle("Proton Hit Times (w/o SiR3 Veto)");
  hTime_Proton_NoVeto->SetXTitle("TME Time [ns]");

  TH1F* hTime_Proton_Veto = new TH1F("hTime_Proton_Veto", "", n_time_bins,min_time,max_time);
  hTime_Proton_Veto->SetTitle("Proton Hit Times (w/ SiR3 Veto)");
  hTime_Proton_Veto->SetXTitle("TME Time [ns]");

  double min_ge_energy = 0;
  double max_ge_energy = 1500;
  double ge_energy_width = 0.5;
  int n_ge_energy_bins = (max_ge_energy - min_ge_energy) / ge_energy_width;
  TH2F* hGeLoGain_EnergyTime = new TH2F("hGeLoGain_EnergyTime", "", n_ge_energy_bins,min_ge_energy,max_ge_energy, n_time_bins,min_time,max_time);
  hGeLoGain_EnergyTime->SetTitle("GeLoGain Energy vs Time");
  hGeLoGain_EnergyTime->SetXTitle("Time [ns]");
  hGeLoGain_EnergyTime->SetYTitle("Energy [keV]");

  TH2F* hGeHiGain_EnergyTime = new TH2F("hGeHiGain_EnergyTime", "", n_ge_energy_bins,min_ge_energy,max_ge_energy, n_time_bins,min_time,max_time);
  hGeHiGain_EnergyTime->SetTitle("GeHiGain Energy vs Time");
  hGeHiGain_EnergyTime->SetYTitle("Time [ns]");
  hGeHiGain_EnergyTime->SetXTitle("Energy [keV]");

  std::vector<FullPulseId> GeLoGain_pulses_seen;
  std::vector<FullPulseId> GeHiGain_pulses_seen;
  
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

    //    if (n_thick_pulses < 1) {
    //      continue;
    //    }

    for (int i_thin_channel = 0; i_thin_channel < n_SiR1_channels; ++i_thin_channel) {
      std::vector<SimplePulse>* i_thin_pulse_list = all_SiR1_channels[i_thin_channel];
      int n_thin_pulses = i_thin_pulse_list->size();

      //      if (n_thin_pulses < 1) {
      //	continue;
      //      }


      for (int i_thick_pulse = 0; i_thick_pulse < n_thick_pulses; ++i_thick_pulse) {
	double thick_time = i_thick_pulse_list->at(i_thick_pulse).tTME;
	double thick_energy = i_thick_pulse_list->at(i_thick_pulse).E;

	//	if (i_thick_pulse_list->at(i_thick_pulse).bit_mask == 2) {
	//	  continue;
	//	}
	
	for (int i_thin_pulse = 0; i_thin_pulse < n_thin_pulses; ++i_thin_pulse) {
	  double thin_time = i_thin_pulse_list->at(i_thin_pulse).tTME;
	  double thin_energy = i_thin_pulse_list->at(i_thin_pulse).E;

	  //	  if (thick_time < pb_time_cut || thin_time < pb_time_cut) {
	  //	    continue;
	  //	  }
	  //	  if (i_thin_pulse_list->at(i_thin_pulse).bit_mask == 2) {
	  //	    continue;
	  //	  }

	  if (std::fabs(thin_time - thick_time) < layer_coincidence_time) {
	    //	    std::cout << "Thin Hit: " << thin_energy << " keV, " << thin_time << " ns" << std::endl;
	    //	    std::cout << "Thick Hit: " << thick_energy << " keV, " << thick_time << " ns" << std::endl;
	    bool passes_proton_cut = false;
	    double electron_spot_cut_line = electron_spot_gradient*(thin_energy+thick_energy) + electron_spot_yoffset;
	    double deuteron_cut_line = deuteron_cut_peak*TMath::Exp(deuteron_cut_slope*(thin_energy+thick_energy)) + deuteron_cut_yoffset;
	    if (thin_energy>electron_spot_cut_line &&
		thin_energy<deuteron_cut_line) {
	      passes_proton_cut = true;
	    }
	    
	    
	    hEvdE_Si16b_NoVeto->Fill(thick_energy+thin_energy, thin_energy);
	    hEvdE_Si16b_ZoomNoVeto->Fill(thick_energy+thin_energy, thin_energy);
	    if (passes_proton_cut && thin_energy>punch_through_cut) { // want the extra punch-through cut for the no SiR3 veto plot
	      hEvdE_Si16b_Proton_NoVeto->Fill(thick_energy+thin_energy, thin_energy);
	      hTime_Proton_NoVeto->Fill(thick_time);
	    }
	    
	    if (SiR3->size() == 0) {
	      hEvdE_Si16b_Veto->Fill(thick_energy+thin_energy, thin_energy);
	      hEvdE_Si16b_ZoomVeto->Fill(thick_energy+thin_energy, thin_energy);
	      if (passes_proton_cut) {
		hEvdE_Si16b_Proton_Veto->Fill(thick_energy+thin_energy, thin_energy);
		hTime_Proton_Veto->Fill(thick_time);
	      }
	    }
	  }
	}
      }
    }


    // Now fill the germanium plots
    int n_ge_pulses = GeLoGain->size();
    for (int i_ge_pulse = 0; i_ge_pulse < n_ge_pulses; ++i_ge_pulse) {
      double ge_energy = GeLoGain->at(i_ge_pulse).E;
      double ge_time = GeLoGain->at(i_ge_pulse).tTME;
      int ge_tpi_id = GeLoGain->at(i_ge_pulse).tpi_id;
      int bit_mask = GeLoGain->at(i_ge_pulse).bit_mask;

      bool double_counted = false;
      if (bit_mask == 2) { // this pulse also appears in another TME
	//	std::cout << "AE: Run #" << runId << ", Block #" << blockId << ", TPI #" << ge_tpi_id << " is a double count" << std::endl;
	for (std::vector<FullPulseId>::const_reverse_iterator i_ge_pulse_seen = GeLoGain_pulses_seen.rbegin(); i_ge_pulse_seen != GeLoGain_pulses_seen.rend(); ++i_ge_pulse_seen) {
	  //	  std::cout << "Checking against (" << i_ge_pulse_seen->run_id << ", " << i_ge_pulse_seen->block_id << ", " << i_ge_pulse_seen->tpi_id << ")" << std::endl;
	  if (ge_tpi_id == i_ge_pulse_seen->tpi_id && runId == i_ge_pulse_seen->run_id && blockId == i_ge_pulse_seen->block_id) {
	    //	    std::cout << "Caught!" << std::endl;
	    double_counted = true;
	    break;
	  }	      
	}
	if (!double_counted) { // if we've got this far...
	  //	  std::cout << "Not caught so adding to vector" << std::endl;
	  GeLoGain_pulses_seen.push_back(FullPulseId(runId, blockId, ge_tpi_id)); // ...add it to the vector
	}
      }

      if (!double_counted) {
	hGeLoGain_EnergyTime->Fill(ge_energy, ge_time);
      }
    }
    n_ge_pulses = GeHiGain->size();
    for (int i_ge_pulse = 0; i_ge_pulse < n_ge_pulses; ++i_ge_pulse) {
      double ge_energy = GeHiGain->at(i_ge_pulse).E;
      double ge_time = GeHiGain->at(i_ge_pulse).tTME;
      int ge_tpi_id = GeHiGain->at(i_ge_pulse).tpi_id;
      int bit_mask = GeHiGain->at(i_ge_pulse).bit_mask;

      bool double_counted = false;
      if (bit_mask == 2) { // this pulse also appears in another TME
	//	std::cout << "AE: Run #" << runId << ", Block #" << blockId << ", TPI #" << ge_tpi_id << " is a double count" << std::endl;
	for (std::vector<FullPulseId>::const_reverse_iterator i_ge_pulse_seen = GeHiGain_pulses_seen.rbegin(); i_ge_pulse_seen != GeHiGain_pulses_seen.rend(); ++i_ge_pulse_seen) {
	  //	  std::cout << "Checking against (" << i_ge_pulse_seen->run_id << ", " << i_ge_pulse_seen->block_id << ", " << i_ge_pulse_seen->tpi_id << ")" << std::endl;
	  if (ge_tpi_id == i_ge_pulse_seen->tpi_id && runId == i_ge_pulse_seen->run_id && blockId == i_ge_pulse_seen->block_id) {
	    //	    std::cout << "Caught!" << std::endl;
	    double_counted = true;
	    break;
	  }	      
	}
	if (!double_counted) { // if we've got this far...
	  //	  std::cout << "Not caught so adding to vector" << std::endl;
	  GeHiGain_pulses_seen.push_back(FullPulseId(runId, blockId, ge_tpi_id)); // ...add it to the vector
	}
      }

      if (!double_counted) {
	hGeHiGain_EnergyTime->Fill(ge_energy, ge_time);
      }
    }
  }

  hEvdE_Si16b_Veto->Draw("COLZ");

  TFile* out_file = new TFile(outfilename.c_str(), "RECREATE");
  hEvdE_Si16b_Veto->Write();
  hEvdE_Si16b_NoVeto->Write();
  hEvdE_Si16b_ZoomNoVeto->Write();
  hEvdE_Si16b_ZoomVeto->Write();
  hEvdE_Si16b_Proton_NoVeto->Write();
  hEvdE_Si16b_Proton_Veto->Write();
  hTime_Proton_NoVeto->Write();
  hTime_Proton_Veto->Write();
  hGeLoGain_EnergyTime->Write();
  hGeHiGain_EnergyTime->Write();
    
  out_file->Write();
  out_file->Close();
}
