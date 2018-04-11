#include "TFile.h"
#include "TH2.h"
#include "TTree.h"

#include "scripts/TMETree/AlCapConstants.h"
#include "scripts/TMETree/TMETreeBranches.h"

#include <iostream>

struct PlotParams {
  PlotParams() {};
  PlotParams(double x_min, double x_max, double x_width) : min(x_min), max(x_max), width(x_width) {
    n_bins = (max - min) / width;
  }
  double min;
  double max;
  double width;
  int n_bins;
};

struct ArmInfo {
  std::string name;
  std::vector<std::vector<SimplePulse>** > layer1_channels;
  std::vector<std::vector<SimplePulse>** > layer2_channels;
  std::vector<std::vector<SimplePulse>** > layer3_channels;
};

struct ArmOutput {
  ArmOutput() {};
  ArmOutput(const ArmInfo* a) : arminfo(a) {};

  const ArmInfo* arminfo;
  TH2F* hEvdE_all;
  TH2F* hEvdE_all_veto;
};

struct GeInfo {
  std::string name;
  std::vector<SimplePulse>** channel;
};

struct GeOutput {
  GeOutput() {};
  GeOutput(const GeInfo* g) : geinfo(g) {};

  const GeInfo* geinfo;
  TH2F* hEvstTME;
};
  


struct TMELoopArgs {
  // Standard parameters
  std::string infilename;
  std::string tmetreename;
  std::string outfilename;
  int n_entries;

  // Channel configuration
  std::vector<std::vector<SimplePulse>** > muon_channels;
  ArmInfo left_arm;
  ArmInfo right_arm;
  GeInfo ge_lo_gain;
  GeInfo ge_hi_gain;
  std::vector<std::vector<SimplePulse>** > target_channels;

  bool left_arm_complete() const {
    if (!left_arm.layer1_channels.empty() && !left_arm.layer2_channels.empty()) { // only ever had two layers working in the left arm
      return true;
    }
    else {
      return false;
    }
  }
  bool right_arm_complete() const {
    if (!right_arm.layer1_channels.empty() && !right_arm.layer2_channels.empty() && !right_arm.layer3_channels.empty()) {
      return true;
    }
    else {
      return false;
    }
  }

  // Event vetoes
  bool veto_any_double_counts;
  bool veto_max_muon_channel_pulses;
  int max_muon_channel_pulses;

  // To produce the EvdE plot for SiR
  bool produceEvdEPlots;
  PlotParams params_EvdE[2];
  double evde_layer_coincidence_time;
  double evde_time_cut;

  // To produce E vs tTME plots for the ge channels
  bool produceGeEvstTMEPlots;
  PlotParams params_GeEvstTME[2];
};

struct TMELoopOutput {
  std::vector<ArmOutput> arms;
  std::vector<GeOutput> ges;

  TTree* infotree;

  void Write() {
    for (std::vector<ArmOutput>::const_iterator i_arm = arms.begin(); i_arm != arms.end(); ++i_arm) {
      if(i_arm->hEvdE_all) {
	i_arm->hEvdE_all->Write();
      }
      if(i_arm->hEvdE_all_veto) {
	i_arm->hEvdE_all_veto->Write();
      }
    }

    for (std::vector<GeOutput>::const_iterator i_ge = ges.begin(); i_ge != ges.end(); ++i_ge) {
      if(i_ge->hEvstTME) {
	i_ge->hEvstTME->Write();
      }
    }
    
    if (infotree) {
      infotree->Fill();
      infotree->Write();
    }
  }
} final_output;

int CheckArgs(const TMELoopArgs& args) {
  if (args.muon_channels.empty()) {
    std::cout << "No muon channels specified" << std::endl;
    return 1;
  }

  if (!args.veto_any_double_counts) {
    std::cout << "WARNING: not vetoing on any TMEs with any double counted events" << std::endl;
    std::cout << "TMELoop.C does not currently accurately handle double counted pulses" << std::endl;
    std::cout << "If you want to continue remove the return statement in this if branch and re-run" << std::endl;
    return 1;
  }

  return 0;
}

void Setup(const TMELoopArgs& args) {
  final_output.infotree = new TTree("infotree", "");

  ////////////////////////////////////
  // For the EvdE Plots
  bool produceEvdEPlots = args.produceEvdEPlots;
  final_output.infotree->Branch("produceEvdEPlots", &produceEvdEPlots);  
  if (args.produceEvdEPlots) {
    const PlotParams& xaxis = args.params_EvdE[0];
    const PlotParams& yaxis = args.params_EvdE[1];
    std::string allhist_basename = "hEvdE_all_";
    std::string protonhist_basename = "hEvdE_proton_";
    if (args.left_arm_complete()) {
      final_output.arms.push_back(ArmOutput(&args.left_arm));
      std::string histname = allhist_basename + args.left_arm.name;
      final_output.arms.back().hEvdE_all = new TH2F(histname.c_str(), "", xaxis.n_bins,xaxis.min,xaxis.max, yaxis.n_bins,yaxis.min,yaxis.max);

      histname += "_veto";
      final_output.arms.back().hEvdE_all_veto = new TH2F(histname.c_str(), "", xaxis.n_bins,xaxis.min,xaxis.max, yaxis.n_bins,yaxis.min,yaxis.max);
    }
    
    if (args.right_arm_complete()) {
      final_output.arms.push_back(ArmOutput(&args.right_arm));
      std::string histname = allhist_basename + args.right_arm.name;
      final_output.arms.back().hEvdE_all = new TH2F(histname.c_str(), "", xaxis.n_bins,xaxis.min,xaxis.max, yaxis.n_bins,yaxis.min,yaxis.max);

      histname += "_veto";
      final_output.arms.back().hEvdE_all_veto = new TH2F(histname.c_str(), "", xaxis.n_bins,xaxis.min,xaxis.max, yaxis.n_bins,yaxis.min,yaxis.max);
    }

    // Record info about these plots
    double evde_layer_coincidence_time = args.evde_layer_coincidence_time;
    double evde_time_cut = args.evde_time_cut;
    final_output.infotree->Branch("evde_layer_coincidence_time", &evde_layer_coincidence_time);
    final_output.infotree->Branch("evde_time_cut", &evde_time_cut);
  }

  ///////////////////////////////////////////
  // For the germanium EvstTME plots
  bool produceGeEvstTMEPlots = args.produceGeEvstTMEPlots;
  final_output.infotree->Branch("produceGeEvstTMEPlots", &produceGeEvstTMEPlots);  
  if (args.produceGeEvstTMEPlots) {
    const PlotParams& xaxis = args.params_GeEvstTME[0];
    const PlotParams& yaxis = args.params_GeEvstTME[1];
    std::string allhist_basename = "hEvstTME_";
    if (args.ge_lo_gain.channel) {
      final_output.ges.push_back(GeOutput(&args.ge_lo_gain));
      std::string histname = allhist_basename + args.ge_lo_gain.name;
      final_output.ges.back().hEvstTME = new TH2F(histname.c_str(), "", xaxis.n_bins,xaxis.min,xaxis.max, yaxis.n_bins,yaxis.min,yaxis.max);
    }
    if (args.ge_hi_gain.channel) {
      final_output.ges.push_back(GeOutput(&args.ge_hi_gain));
      std::string histname = allhist_basename + args.ge_hi_gain.name;
      final_output.ges.back().hEvstTME = new TH2F(histname.c_str(), "", xaxis.n_bins,xaxis.min,xaxis.max, yaxis.n_bins,yaxis.min,yaxis.max);
    }
  }
}

void TMELoop(const TMELoopArgs& args) {

  TFile* infile = new TFile(args.infilename.c_str(), "READ");
  TTree* tmetree = (TTree*) infile->Get(args.tmetreename.c_str());

  if (CheckArgs(args)) {
    std::cout << "Problem with input TME loop arguments" << std::endl;
    return;
  }

  Setup(args);

  SetTMEBranchAddresses(tmetree); // setup the branch addresses

  int n_total_tmes = 0;
  int n_analysed_tmes = 0;

  final_output.infotree->Branch("n_total_tmes", &n_total_tmes);
  final_output.infotree->Branch("n_analysed_tmes", &n_analysed_tmes);

  std::cout << "Total TMEs = " << tmetree->GetEntries() << std::endl;
  int n_entries = args.n_entries;
  if (n_entries < 0) {
    n_entries = tmetree->GetEntries();
  }
  for (int i_tme = 0; i_tme < n_entries; ++i_tme) {

    ++n_total_tmes;
    
    if (i_tme % 100000 == 0) {
      std::cout << i_tme << " / " << n_entries << std::endl;
    }

    tmetree->GetEntry(i_tme);
    CollectChannels(); // collect all channels into various groups (e.g. SiR1, SiL1, All) for easy looping

    // Veto on events
    if (args.veto_any_double_counts && anyDoubleCountedPulses) {
      continue; // to the next TME
    }
    if (args.veto_max_muon_channel_pulses) {
      int n_muon_channels = args.muon_channels.size();
      int n_muon_channel_pulses = 0;
      for (int i_muon_channel = 0; i_muon_channel < n_muon_channels; ++i_muon_channel) {
	n_muon_channel_pulses += (*args.muon_channels[i_muon_channel])->size();
      }
      if (n_muon_channel_pulses > args.max_muon_channel_pulses) {
	continue; // to the next TME
      }
    }

    ++n_analysed_tmes;

    if (args.produceEvdEPlots) {
      for (std::vector<ArmOutput>::iterator i_arm = final_output.arms.begin(); i_arm != final_output.arms.end(); ++i_arm) {

	// Loop through layer 2 channels
	int n_thick_channels = i_arm->arminfo->layer2_channels.size();
	for (int i_thick_channel = 0; i_thick_channel < n_thick_channels; ++i_thick_channel) {
	  std::vector<SimplePulse>* i_thick_pulse_list = *(i_arm->arminfo->layer2_channels.at(i_thick_channel));
	  int n_thick_pulses = i_thick_pulse_list->size();

	  // Loop through layer 1 channels
	  int n_thin_channels = i_arm->arminfo->layer1_channels.size();
	  for (int i_thin_channel = 0; i_thin_channel < n_thin_channels; ++i_thin_channel) {
	    std::vector<SimplePulse>* i_thin_pulse_list = *(i_arm->arminfo->layer1_channels.at(i_thin_channel));
	    int n_thin_pulses = i_thin_pulse_list->size();

	    // Loop through layer 2 pulses
	    for (int i_thick_pulse = 0; i_thick_pulse < n_thick_pulses; ++i_thick_pulse) {
	      double thick_time = i_thick_pulse_list->at(i_thick_pulse).tTME;
	      double thick_energy = i_thick_pulse_list->at(i_thick_pulse).E;

	      // Loop through layer 1 pulses
	      for (int i_thin_pulse = 0; i_thin_pulse < n_thin_pulses; ++i_thin_pulse) {
		double thin_time = i_thin_pulse_list->at(i_thin_pulse).tTME;
		double thin_energy = i_thin_pulse_list->at(i_thin_pulse).E;

		// Loop through layer 3 channels and just count the number of pulses for the veto
		int n_third_channels = i_arm->arminfo->layer2_channels.size();
		int total_n_third_pulses = 0;
		for (int i_third_channel = 0; i_third_channel < n_third_channels; ++i_third_channel) {
		  std::vector<SimplePulse>* i_third_pulse_list = *(i_arm->arminfo->layer3_channels.at(i_third_channel));
		  int n_third_pulses = i_third_pulse_list->size();
		  total_n_third_pulses += n_third_pulses;
		}

		// are the hits in the first and second layers in time?
		if (std::fabs(thin_time - thick_time) < args.evde_layer_coincidence_time) {

		  // cut on hits at early times that might be coming from elsewhere
		  if (thick_time < args.evde_time_cut || thin_time < args.evde_time_cut) {
		    continue; // to the next pulse
		  }

		  i_arm->hEvdE_all->Fill(thick_energy+thin_energy, thin_energy);

		  // veto on a hit in the third layer
		  if (total_n_third_pulses == 0) {
		    i_arm->hEvdE_all_veto->Fill(thick_energy+thin_energy, thin_energy);
		  }
		}
	      }
	    }
	  }
	}
      } // end Arm loop
    } // end EvdE plots if

    /////////////////////////////////////////////
    // Fill Ge EvstTME plots
    if (args.produceGeEvstTMEPlots) {
      for (std::vector<GeOutput>::iterator i_ge = final_output.ges.begin(); i_ge != final_output.ges.end(); ++i_ge) {

	std::vector<SimplePulse>* i_ge_pulse_list = *(i_ge->geinfo->channel);
	int n_ge_pulses = i_ge_pulse_list->size();
	for (int i_ge_pulse = 0; i_ge_pulse < n_ge_pulses; ++i_ge_pulse) {
	  double ge_energy = i_ge_pulse_list->at(i_ge_pulse).E;
	  double ge_time = i_ge_pulse_list->at(i_ge_pulse).tTME;
	  
	  i_ge->hEvstTME->Fill(ge_time, ge_energy);
	}
      }      
    } // end if Ge EvstTME plots
    
  } // end TME loop

  TFile* outfile = new TFile(args.outfilename.c_str(), "RECREATE");
  final_output.Write();
  outfile->Close();
}
