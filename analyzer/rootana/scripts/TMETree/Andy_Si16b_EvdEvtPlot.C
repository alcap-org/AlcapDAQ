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

#include "TH3.h"
#include "TMath.h"

void Andy_Si16b_EvdEvtPlot() {

  std::string filename = "~/data/out/v10/Si16b.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");

  double min_x_energy = 0;
  double max_x_energy = 20000;
  double min_y_energy = 0;
  double max_y_energy = 10000;
  double energy_width = 50;
  int n_x_energy_bins = (max_x_energy - min_x_energy) / energy_width;
  int n_y_energy_bins = (max_y_energy - min_y_energy) / energy_width;

  double min_time = -1000;
  double max_time = 6000;
  double time_width = 100;
  int n_time_bins = (max_time - min_time) / time_width;


  TH3F* hEvdEvt_Si16b_Veto = new TH3F("hEvdEvt_Si16b_Veto", "", n_x_energy_bins,min_x_energy,max_x_energy, n_y_energy_bins,min_y_energy,max_y_energy, n_time_bins,min_time,max_time);
  hEvdEvt_Si16b_Veto->SetTitle("EvdEvt SiR (w/ SiR3 Veto)");
  hEvdEvt_Si16b_Veto->SetXTitle("E_{SiR1} + E_{SiR2} [keV]");
  hEvdEvt_Si16b_Veto->SetYTitle("E_{SiR1} [keV]");
  hEvdEvt_Si16b_Veto->SetZTitle("Thick TME Time [ns]");

  TH3F* hEvdEvt_Si16b_NoVeto = new TH3F("hEvdEvt_Si16b_NoVeto", "", n_x_energy_bins,min_x_energy,max_x_energy, n_y_energy_bins,min_y_energy,max_y_energy, n_time_bins,min_time,max_time);
  hEvdEvt_Si16b_NoVeto->SetTitle("EvdEvt SiR (w/o SiR3 Veto)");
  hEvdEvt_Si16b_NoVeto->SetXTitle("E_{SiR1} + E_{SiR2} [keV]");
  hEvdEvt_Si16b_NoVeto->SetYTitle("E_{SiR1} [keV]");
  hEvdEvt_Si16b_NoVeto->SetZTitle("Thick TME Time [ns]");

  double min_x_energy_zoom = 0;
  double max_x_energy_zoom = 20000;
  double min_y_energy_zoom = 0;
  double max_y_energy_zoom = 3000;
  double energy_zoom_width = 50;
  int n_x_energy_zoom_bins = (max_x_energy_zoom - min_x_energy_zoom) / energy_zoom_width;
  int n_y_energy_zoom_bins = (max_y_energy_zoom - min_y_energy_zoom) / energy_zoom_width;
  TH3F* hEvdEvt_Si16b_ZoomNoVeto = new TH3F("hEvdEvt_Si16b_ZoomNoVeto", "", n_x_energy_zoom_bins,min_x_energy_zoom,max_x_energy_zoom, n_y_energy_zoom_bins,min_y_energy_zoom,max_y_energy_zoom, n_time_bins,min_time,max_time);
  hEvdEvt_Si16b_ZoomNoVeto->SetTitle("EvdEvt SiR (w/o SiR3 Veto)");
  hEvdEvt_Si16b_ZoomNoVeto->SetXTitle("E_{SiR1} + E_{SiR2} [keV]");
  hEvdEvt_Si16b_ZoomNoVeto->SetYTitle("E_{SiR1} [keV]");
  hEvdEvt_Si16b_ZoomNoVeto->SetZTitle("Thick TME Time [ns]");

  TH3F* hEvdEvt_Si16b_ZoomVeto = new TH3F("hEvdEvt_Si16b_ZoomVeto", "", n_x_energy_zoom_bins,min_x_energy_zoom,max_x_energy_zoom, n_y_energy_zoom_bins,min_y_energy_zoom,max_y_energy_zoom, n_time_bins,min_time,max_time);
  hEvdEvt_Si16b_ZoomVeto->SetTitle("EvdEvt SiR (w/ SiR3 Veto)");
  hEvdEvt_Si16b_ZoomVeto->SetXTitle("E_{SiR1} + E_{SiR2} [keV]");
  hEvdEvt_Si16b_ZoomVeto->SetYTitle("E_{SiR1} [keV]");
  hEvdEvt_Si16b_ZoomVeto->SetZTitle("Thick TME Time [ns]");

  TH3F* hEvdEvt_Si16b_Proton_NoVeto = new TH3F("hEvdEvt_Si16b_Proton_NoVeto", "", n_x_energy_zoom_bins,min_x_energy_zoom,max_x_energy_zoom, n_y_energy_zoom_bins,min_y_energy_zoom,max_y_energy_zoom, n_time_bins,min_time,max_time);
  hEvdEvt_Si16b_Proton_NoVeto->SetTitle("EvdEvt Protons SiR (w/o SiR3 Veto)");
  hEvdEvt_Si16b_Proton_NoVeto->SetXTitle("E_{SiR1} + E_{SiR2} [keV]");
  hEvdEvt_Si16b_Proton_NoVeto->SetYTitle("E_{SiR1} [keV]");
  hEvdEvt_Si16b_Proton_NoVeto->SetZTitle("Thick TME Time [ns]");


  TH3F* hEvdEvt_Si16b_Proton_Veto = new TH3F("hEvdEvt_Si16b_Proton_Veto", "", n_x_energy_zoom_bins,min_x_energy_zoom,max_x_energy_zoom, n_y_energy_zoom_bins,min_y_energy_zoom,max_y_energy_zoom, n_time_bins,min_time,max_time);
  hEvdEvt_Si16b_Proton_Veto->SetTitle("EvdEvt Protons SiR (w/ SiR3 Veto)");
  hEvdEvt_Si16b_Proton_Veto->SetXTitle("E_{SiR1} + E_{SiR2} [keV]");
  hEvdEvt_Si16b_Proton_Veto->SetYTitle("E_{SiR1} [keV]");
  hEvdEvt_Si16b_Proton_Veto->SetZTitle("Thick TME Time [ns]");

  // Define the various cuts
  double layer_coincidence_time = 200;

  double x_1 = 0, y_1 = 2250, x_2 = 5500, y_2 = 0;
  double electron_spot_gradient = (y_2 - y_1) / (x_2 - x_1);
  double electron_spot_yoffset = y_1;

  double punch_through_cut = 300; // keV

  double deuteron_cut_peak = 4500;
  double deuteron_cut_slope = -0.0004;
  double deuteron_cut_yoffset = 500;

  double pb_time_cut = 200; //ns

  SetTMEBranchAddresses(tmetree); // setup the branch addresses

  std::cout << "Total TMEs = " << tmetree->GetEntries() << std::endl;
  int n_entries = 100000000;//tmetree->GetEntries();
  for (int i_tme = 0; i_tme < n_entries; ++i_tme) {

    if (i_tme % 100000 == 0) {
      std::cout << i_tme << " / " << n_entries << std::endl;
    }

    tmetree->GetEntry(i_tme);
    CollectChannels(); // collect all channels into various groups (e.g. SiR1, SiL1, All) for easy looping

    std::vector<SimplePulse>* i_thick_pulse_list = SiR2;
    int n_thick_pulses = i_thick_pulse_list->size();

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
	    
	    
	    hEvdEvt_Si16b_NoVeto->Fill(thick_energy+thin_energy, thin_energy, thick_time);
	    hEvdEvt_Si16b_ZoomNoVeto->Fill(thick_energy+thin_energy, thin_energy, thick_time);
	    if (passes_proton_cut && thin_energy>punch_through_cut) { // want the extra punch-through cut for the no SiR3 veto plot
	      hEvdEvt_Si16b_Proton_NoVeto->Fill(thick_energy+thin_energy, thin_energy, thick_time);
	    }
	    
	    if (SiR3->size() == 0) {
	      hEvdEvt_Si16b_Veto->Fill(thick_energy+thin_energy, thin_energy, thick_time);
	      hEvdEvt_Si16b_ZoomVeto->Fill(thick_energy+thin_energy, thin_energy, thick_time);
	      if (passes_proton_cut) {
		hEvdEvt_Si16b_Proton_Veto->Fill(thick_energy+thin_energy, thin_energy, thick_time);
	      }
	    }
	  }
	}
      }
    }
  }

  hEvdEvt_Si16b_Veto->Draw("COLZ");

  TFile* out_file = new TFile("Andy_Si16b_EvdEvtPlot.root", "RECREATE");
  hEvdEvt_Si16b_Veto->Write();
  hEvdEvt_Si16b_NoVeto->Write();
  hEvdEvt_Si16b_ZoomNoVeto->Write();
  hEvdEvt_Si16b_ZoomVeto->Write();
  hEvdEvt_Si16b_Proton_NoVeto->Write();
  hEvdEvt_Si16b_Proton_Veto->Write();

  out_file->Write();
  out_file->Close();
}
