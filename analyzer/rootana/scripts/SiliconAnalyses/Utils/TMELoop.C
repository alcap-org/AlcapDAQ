#include "TFile.h"
#include "TH2.h"
#include "TTree.h"

#include "TF1.h"

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

struct ArmTreeInfo {
  int run_id;
  int block_id;
  int tme_id;
  
  double thin_energy;
  double thick_energy;
  double third_energy;
  double thin_time;
  double thick_time;
  double third_time;
  int thin_tpi_id;
  int thick_tpi_id;
  int third_tpi_id;
  
} armtreeinfo;

struct ArmOutput {
  ArmOutput() {};
  ArmOutput(const ArmInfo* a) : arminfo(a) {};

  const ArmInfo* arminfo;
  TTree* armtree;
};

struct GeInfo {
  std::string name;
  std::vector<SimplePulse>** channel;
};

struct GeTreeInfo {
  double energy;
  double time;
} getreeinfo;

struct GeOutput {
  GeOutput() {};
  GeOutput(const GeInfo* g) : geinfo(g) {};

  const GeInfo* geinfo;
  TTree* getree;
};
  
struct TargetInfo {
  std::vector<std::vector<SimplePulse>** > channels;
};

struct TargetTreeInfo {
  double energy;
  double time;
} targettreeinfo;

struct TargetOutput {
  TargetOutput() {};
  TargetOutput(const TargetInfo* t) : info(t) {};

  const TargetInfo* info;
  TTree* targettree;
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
  TargetInfo target;

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

  // To produce the EvdE tree
  bool produceEvdETree;

  // To produce E vs tTME plots for the ge channels
  bool produceGeEvstTMETree;

  // To produce E vs tTME plots for the target in an active analysis
  bool active_target_analysis;
};

struct TMELoopOutput {
  std::vector<ArmOutput> arms;
  std::vector<GeOutput> ges;
  TargetOutput target;

  TTree* infotree;

  void Write() {
    for (std::vector<ArmOutput>::const_iterator i_arm = arms.begin(); i_arm != arms.end(); ++i_arm) {
      if(i_arm->armtree) {
	i_arm->armtree->Write();
      }
    }

    for (std::vector<GeOutput>::const_iterator i_ge = ges.begin(); i_ge != ges.end(); ++i_ge) {
      if(i_ge->getree) {
	i_ge->getree->Write();
      }
    }

    if (target.targettree) {
      target.targettree->Write();
    }
    
    if (infotree) {
      infotree->Fill();
      infotree->Write();
    }
  }
} final_output;

int CheckArgs(const TMELoopArgs& args) {
  if (args.muon_channels.empty()) {
    std::cout << "WARNING: No muon channels specified" << std::endl;
    return 1;
  }

  if (!args.veto_any_double_counts) {
    std::cout << "WARNING: not vetoing on any TMEs with any double counted events" << std::endl
	      << "TMELoop.C does not currently accurately handle double counted pulses" << std::endl
	      << "If you want to continue remove the return statement in this if branch and re-run" << std::endl;
    return 1;
  }

  if (args.active_target_analysis && args.target.channels.empty()) {
    std::cout << "WARNING: TMELoop configured as an active target analysis but there are" << std::endl
	      << "no channels in the TargetInfo" << std::endl;
    return 1;
  }

  return 0;
}

void Setup(const TMELoopArgs& args) {
  final_output.infotree = new TTree("infotree", "");

  ////////////////////////////////////
  // Create the EvdE Plots
  bool produceEvdETree = args.produceEvdETree;
  final_output.infotree->Branch("produceEvdETree", &produceEvdETree);  
  if (args.produceEvdETree) {
    if (args.left_arm_complete()) {
      final_output.arms.push_back(ArmOutput(&args.left_arm));
    }
    
    if (args.right_arm_complete()) {
      final_output.arms.push_back(ArmOutput(&args.right_arm));
    }

    std::string tree_basename = "armtree_";
    for (std::vector<ArmOutput>::iterator i_arm = final_output.arms.begin(); i_arm != final_output.arms.end(); ++i_arm) {
      
      std::string treename = tree_basename + i_arm->arminfo->name;
      i_arm->armtree = new TTree(treename.c_str(), treename.c_str());
      i_arm->armtree->Branch("run_id", &armtreeinfo.run_id);
      i_arm->armtree->Branch("block_id", &armtreeinfo.block_id);
      i_arm->armtree->Branch("tme_id", &armtreeinfo.tme_id);
      
      i_arm->armtree->Branch("thin_energy", &armtreeinfo.thin_energy);
      i_arm->armtree->Branch("thin_time", &armtreeinfo.thin_time);
      i_arm->armtree->Branch("thin_tpi_id", &armtreeinfo.thin_tpi_id);
      i_arm->armtree->Branch("thick_energy", &armtreeinfo.thick_energy);
      i_arm->armtree->Branch("thick_time", &armtreeinfo.thick_time);
      i_arm->armtree->Branch("thick_tpi_id", &armtreeinfo.thick_tpi_id);
      i_arm->armtree->Branch("third_energy", &armtreeinfo.third_energy);
      i_arm->armtree->Branch("third_time", &armtreeinfo.third_time);
      i_arm->armtree->Branch("third_tpi_id", &armtreeinfo.third_tpi_id);
    }
  }

  ///////////////////////////////////////////
  // Create the germanium EvstTME plots
  bool produceGeEvstTMETree = args.produceGeEvstTMETree;
  final_output.infotree->Branch("produceGeEvstTMETree", &produceGeEvstTMETree);  
  if (args.produceGeEvstTMETree) {

    if (args.ge_lo_gain.channel) {
      final_output.ges.push_back(GeOutput(&args.ge_lo_gain));
    }
    if (args.ge_hi_gain.channel) {
      final_output.ges.push_back(GeOutput(&args.ge_hi_gain));
    }
    
    std::string tree_basename = "getree_";
    for (std::vector<GeOutput>::iterator i_ge = final_output.ges.begin(); i_ge != final_output.ges.end(); ++i_ge) {
      std::string treename = tree_basename + i_ge->geinfo->name;
      i_ge->getree = new TTree(treename.c_str(), treename.c_str());
      i_ge->getree->Branch("energy", &getreeinfo.energy);
      i_ge->getree->Branch("time", &getreeinfo.time);
    }
  }

  ///////////////////////////////////////
  // Create the active target EvstTME plots
  bool active_target_analysis = args.active_target_analysis;
  final_output.infotree->Branch("active_target_analysis", &active_target_analysis);
  if (args.active_target_analysis) {
    final_output.target.info = &args.target;
    std::string treename = "targettree";
    final_output.target.targettree = new TTree(treename.c_str(), treename.c_str());
    final_output.target.targettree->Branch("energy", &targettreeinfo.energy);
    final_output.target.targettree->Branch("time", &targettreeinfo.time);
  }
}

void TMELoop(const TMELoopArgs& args) {

  TFile* infile = new TFile(args.infilename.c_str(), "READ");
  TTree* tmetree = (TTree*) infile->Get(args.tmetreename.c_str());
  TFile* outfile = new TFile(args.outfilename.c_str(), "RECREATE");
  
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

    /////////////////////////////////////
    // Event Vetoes
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

    /////////////////////////////////
    // Fill EvdE plots
    if (args.produceEvdETree) {
      armtreeinfo.run_id = runId;
      armtreeinfo.block_id = blockId;
      armtreeinfo.tme_id = TMEId;

      // Reset these branches
      armtreeinfo.thick_energy = 0;
      armtreeinfo.thick_time = 0;
      armtreeinfo.thick_tpi_id = -1;
      armtreeinfo.thin_energy = 0;
      armtreeinfo.thin_time = 0;
      armtreeinfo.thin_tpi_id = -1;
      armtreeinfo.third_energy = 0;
      armtreeinfo.third_time = 0;
      armtreeinfo.third_tpi_id = -1;

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
	      double thick_tpi_id = i_thick_pulse_list->at(i_thick_pulse).tpi_id;
	      armtreeinfo.thick_energy = thick_energy;
	      armtreeinfo.thick_time = thick_time;
	      armtreeinfo.thick_tpi_id = thick_tpi_id;

	      // Loop through layer 1 pulses
	      for (int i_thin_pulse = 0; i_thin_pulse < n_thin_pulses; ++i_thin_pulse) {
		double thin_time = i_thin_pulse_list->at(i_thin_pulse).tTME;
		double thin_energy = i_thin_pulse_list->at(i_thin_pulse).E;
		double thin_tpi_id = i_thin_pulse_list->at(i_thin_pulse).tpi_id;
		armtreeinfo.thin_time = thin_time;
		armtreeinfo.thin_energy = thin_energy;
		armtreeinfo.thin_tpi_id = thin_tpi_id;
		
		// Loop through layer 3 channels and just count the number of pulses for the veto
		int n_third_channels = i_arm->arminfo->layer2_channels.size();
		int total_n_third_pulses = 0;
		for (int i_third_channel = 0; i_third_channel < n_third_channels; ++i_third_channel) {
		  std::vector<SimplePulse>* i_third_pulse_list = *(i_arm->arminfo->layer3_channels.at(i_third_channel));
		  int n_third_pulses = i_third_pulse_list->size();
		  total_n_third_pulses += n_third_pulses;

		  for (int i_third_pulse = 0; i_third_pulse < n_third_pulses; ++i_third_pulse) {
		    double third_time = i_third_pulse_list->at(i_third_pulse).tTME;
		    double third_energy = i_third_pulse_list->at(i_third_pulse).E;
		    double third_tpi_id = i_third_pulse_list->at(i_third_pulse).tpi_id;
		    armtreeinfo.third_energy = third_energy;
		    armtreeinfo.third_time = third_time;
		    armtreeinfo.third_tpi_id = third_tpi_id;
		    
		    i_arm->armtree->Fill();
		  }
		}

		if (total_n_third_pulses == 0) {
		  i_arm->armtree->Fill(); // fill in any events where there were no third layer hits
		}
	      }
	    }
	  }
	}
      } // end Arm loop
    } // end EvdE plots if

    /////////////////////////////////////////////
    // Fill Ge EvstTME plots
    if (args.produceGeEvstTMETree) {
      for (std::vector<GeOutput>::iterator i_ge = final_output.ges.begin(); i_ge != final_output.ges.end(); ++i_ge) {

	std::vector<SimplePulse>* i_ge_pulse_list = *(i_ge->geinfo->channel);
	int n_ge_pulses = i_ge_pulse_list->size();
	for (int i_ge_pulse = 0; i_ge_pulse < n_ge_pulses; ++i_ge_pulse) {
	  double ge_energy = i_ge_pulse_list->at(i_ge_pulse).E;
	  double ge_time = i_ge_pulse_list->at(i_ge_pulse).tTME;
	  getreeinfo.energy = ge_energy;
	  getreeinfo.time = ge_time;
	  
	  i_ge->getree->Fill();
	}
      }      
    } // end if Ge EvstTME plots

    ////////////////////////////////////////////
    // Fill Target EvstTME plots
    if (args.active_target_analysis) {
      int n_target_channels = final_output.target.info->channels.size();

      for (int i_target_channel = 0; i_target_channel < n_target_channels; ++i_target_channel) {
	std::vector<SimplePulse>* i_target_pulse_list = *(final_output.target.info->channels.at(i_target_channel));
	int n_target_pulses = i_target_pulse_list->size();

	for (int i_target_pulse = 0; i_target_pulse < n_target_pulses; ++i_target_pulse) {
	  double target_energy = i_target_pulse_list->at(i_target_pulse).E;
	  double target_time = i_target_pulse_list->at(i_target_pulse).tTME;
	  targettreeinfo.energy = target_energy;
	  targettreeinfo.time = target_time;

	  final_output.target.targettree->Fill();
	}
      }
    } // end fill target EvstTME plots
    
  } // end TME loop

  final_output.Write();
  outfile->Close();
}
