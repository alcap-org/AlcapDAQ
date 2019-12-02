#include "TFile.h"
#include "TH2.h"
#include "TTree.h"

#include "TF1.h"

#include "../../TMETree/AlCapConstants.h"
#include "../../TMETree/TMETreeBranches.h"

#include <iostream>

struct SiBlockInfo {
  std::string name;
  std::vector<std::vector<SimplePulse>** > layer1_channels;
  std::vector<double> layer1_calibGains;
  std::vector<double> layer1_calibOffsets;
  std::vector<std::vector<SimplePulse>** > layer2_channels;
  std::vector<double> layer2_calibGains;
  std::vector<double> layer2_calibOffsets;
  std::vector<std::vector<SimplePulse>** > layer3_channels;
  std::vector<double> layer3_calibGains;
  std::vector<double> layer3_calibOffsets;

};

struct SiBlockTreeInfo {
  int run_id;
  int block_id;
  int tme_id;
  double central_muon_time;
  
  double thin_energy;
  double thick_energy;
  double third_energy;
  double thin_time;
  double thick_time;
  double third_time;
  int thin_tpi_id;
  int thick_tpi_id;
  int third_tpi_id;
  int thin_tap_id;
  int thick_tap_id;
  int third_tap_id;
  int thin_channel;
  int thick_channel;
  int third_channel;
  double thin_calib_gain;
  double thin_calib_offset;
  double thick_calib_gain;
  double thick_calib_offset;
  double third_calib_gain;
  double third_calib_offset;
} siBlockTreeInfo;

struct SiBlockOutput {
  SiBlockOutput() {};
  SiBlockOutput(const SiBlockInfo* a) : siBlockInfo(a) {};

  const SiBlockInfo* siBlockInfo;
  TTree* siBlockTree;
};

struct GeInfo {
  std::string name;
  std::vector<SimplePulse>** channel;
  double calibGain;
  double calibOffset;
};

struct GeTreeInfo {
  int run_id;
  int block_id;
  int tme_id;
  int tpi_id;
  int tap_id;

  double central_muon_time;
  
  double energy;
  double time;
  double calib_gain;
  double calib_offset;
} geTreeInfo;

struct GeOutput {
  GeOutput() {};
  GeOutput(const GeInfo* g) : geInfo(g) {};

  const GeInfo* geInfo;
  TTree* geTree;
};
  

struct GeTargetTreeInfo {
  int run_id;
  int block_id;
  int tme_id;
  double central_muon_time;

  int ge_tpi_id;
  int ge_tap_id;
  double ge_energy;
  double ge_time;
  double ge_calib_gain;
  double ge_calib_offset;

  double thin_energy;
  double thick_energy;
  double third_energy;
  double thin_time;
  double thick_time;
  double third_time;
  int thin_tpi_id;
  int thick_tpi_id;
  int third_tpi_id;
  int thin_tap_id;
  int thick_tap_id;
  int third_tap_id;
  int thin_channel;
  int thick_channel;
  int third_channel;
  double thin_calib_gain;
  double thin_calib_offset;
  double thick_calib_gain;
  double thick_calib_offset;
  double third_calib_gain;
  double third_calib_offset;
} geTargetTreeInfo;

struct GeTargetOutput {
  GeTargetOutput() {};
  GeTargetOutput(const GeInfo* g, const SiBlockInfo* t) : geInfo(g), targetInfo(t) {};

  const GeInfo* geInfo;
  const SiBlockInfo* targetInfo;
  TTree* geTargetTree;
};

struct TMELoopArgs {
  // Standard parameters
  std::string infilename;
  std::string tmetreename;
  std::string outfilename;
  int n_entries;

  // Channel configuration
  std::vector<std::vector<SimplePulse>** > muon_channels;
  SiBlockInfo left_arm;
  SiBlockInfo right_arm;
  GeInfo ge_lo_gain;
  GeInfo ge_hi_gain;
  SiBlockInfo target; // for active target runs
  SiBlockInfo sit;

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

  bool veto_pp_window;
  double pp_window;

  bool req_tgt_muon;
  int tgt_layer;
  int min_tgt_pulses;
  //  double muScCutLo;
  //  double muScCutHi;

  int reqd_run_id;
};

struct TMELoopOutput {
  std::vector<SiBlockOutput> siBlocks;
  std::vector<GeOutput> ges;
  std::vector<GeTargetOutput> geTargets;

  TTree* infotree;

  TH1F* hCentralMuonTDiff;

  void Write() {
    for (std::vector<SiBlockOutput>::const_iterator i_siBlock = siBlocks.begin(); i_siBlock != siBlocks.end(); ++i_siBlock) {
      if(i_siBlock->siBlockTree) {
	i_siBlock->siBlockTree->Write();
      }
    }

    for (std::vector<GeOutput>::const_iterator i_ge = ges.begin(); i_ge != ges.end(); ++i_ge) {
      if(i_ge->geTree) {
	i_ge->geTree->Write();
      }
    }

    for (std::vector<GeTargetOutput>::const_iterator i_geTarget = geTargets.begin(); i_geTarget != geTargets.end(); ++i_geTarget) {
      if(i_geTarget->geTargetTree) {
	i_geTarget->geTargetTree->Write();
      }
    }
    
    if (infotree) {
      infotree->Fill();
      infotree->Write();
    }
    if (hCentralMuonTDiff) {
      hCentralMuonTDiff->Write();
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
    //    return 1;
  }

  return 0;
}

void Setup(const TMELoopArgs& args) {
  final_output.infotree = new TTree("infotree", "");
  final_output.hCentralMuonTDiff = new TH1F("hCentralMuonTDiff", "", 200,0,1000000);
  final_output.hCentralMuonTDiff->SetXTitle("| t_{#mu + 1} - t_{#mu} | [ns]");

  ///////////////////////////////////////////
  // Put everything into the final output
  final_output.siBlocks.push_back(SiBlockOutput(&args.left_arm));
  final_output.siBlocks.push_back(SiBlockOutput(&args.right_arm));
  final_output.siBlocks.push_back(SiBlockOutput(&args.target));
  final_output.siBlocks.push_back(SiBlockOutput(&args.sit));

  final_output.ges.push_back(GeOutput(&args.ge_lo_gain));
  final_output.ges.push_back(GeOutput(&args.ge_hi_gain));

  final_output.geTargets.push_back(GeTargetOutput(&args.ge_lo_gain, &args.target));
  final_output.geTargets.push_back(GeTargetOutput(&args.ge_hi_gain, &args.target));

  ///////////////////////////////////////////////
  // Create the subtrees
  std::string tree_basename = "siBlockTree_";
  for (std::vector<SiBlockOutput>::iterator i_siBlock = final_output.siBlocks.begin(); i_siBlock != final_output.siBlocks.end(); ++i_siBlock) {
    
    std::string treename = tree_basename + i_siBlock->siBlockInfo->name;
    i_siBlock->siBlockTree = new TTree(treename.c_str(), treename.c_str());
    i_siBlock->siBlockTree->Branch("run_id", &siBlockTreeInfo.run_id);
    i_siBlock->siBlockTree->Branch("block_id", &siBlockTreeInfo.block_id);
    i_siBlock->siBlockTree->Branch("tme_id", &siBlockTreeInfo.tme_id);
    i_siBlock->siBlockTree->Branch("central_muon_time", &siBlockTreeInfo.central_muon_time);
    
    i_siBlock->siBlockTree->Branch("thin_energy", &siBlockTreeInfo.thin_energy);
    i_siBlock->siBlockTree->Branch("thin_time", &siBlockTreeInfo.thin_time);
    i_siBlock->siBlockTree->Branch("thin_tpi_id", &siBlockTreeInfo.thin_tpi_id);
    i_siBlock->siBlockTree->Branch("thin_tap_id", &siBlockTreeInfo.thin_tap_id);
    i_siBlock->siBlockTree->Branch("thin_channel", &siBlockTreeInfo.thin_channel);
    i_siBlock->siBlockTree->Branch("thin_calib_gain", &siBlockTreeInfo.thin_calib_gain);
    i_siBlock->siBlockTree->Branch("thin_calib_offset", &siBlockTreeInfo.thin_calib_offset);

    i_siBlock->siBlockTree->Branch("thick_energy", &siBlockTreeInfo.thick_energy);
    i_siBlock->siBlockTree->Branch("thick_time", &siBlockTreeInfo.thick_time);
    i_siBlock->siBlockTree->Branch("thick_tpi_id", &siBlockTreeInfo.thick_tpi_id);
    i_siBlock->siBlockTree->Branch("thick_tap_id", &siBlockTreeInfo.thick_tap_id);
    i_siBlock->siBlockTree->Branch("thick_channel", &siBlockTreeInfo.thick_channel);
    i_siBlock->siBlockTree->Branch("thick_calib_gain", &siBlockTreeInfo.thick_calib_gain);
    i_siBlock->siBlockTree->Branch("thick_calib_offset", &siBlockTreeInfo.thick_calib_offset);

    i_siBlock->siBlockTree->Branch("third_energy", &siBlockTreeInfo.third_energy);
    i_siBlock->siBlockTree->Branch("third_time", &siBlockTreeInfo.third_time);
    i_siBlock->siBlockTree->Branch("third_tpi_id", &siBlockTreeInfo.third_tpi_id);
    i_siBlock->siBlockTree->Branch("third_tap_id", &siBlockTreeInfo.third_tap_id);
    i_siBlock->siBlockTree->Branch("third_channel", &siBlockTreeInfo.third_channel);
    i_siBlock->siBlockTree->Branch("third_calib_gain", &siBlockTreeInfo.third_calib_gain);
    i_siBlock->siBlockTree->Branch("third_calib_offset", &siBlockTreeInfo.third_calib_offset);
  }

  tree_basename = "geTree_";
  for (std::vector<GeOutput>::iterator i_ge = final_output.ges.begin(); i_ge != final_output.ges.end(); ++i_ge) {
    std::string treename = tree_basename + i_ge->geInfo->name;
    i_ge->geTree = new TTree(treename.c_str(), treename.c_str());
    i_ge->geTree->Branch("run_id", &geTreeInfo.run_id);
    i_ge->geTree->Branch("block_id", &geTreeInfo.block_id);
    i_ge->geTree->Branch("tme_id", &geTreeInfo.tme_id);
    i_ge->geTree->Branch("tpi_id", &geTreeInfo.tpi_id);
    i_ge->geTree->Branch("tap_id", &geTreeInfo.tap_id);
    i_ge->geTree->Branch("central_muon_time", &geTreeInfo.central_muon_time);
    i_ge->geTree->Branch("energy", &geTreeInfo.energy);
    i_ge->geTree->Branch("time", &geTreeInfo.time);
    i_ge->geTree->Branch("calib_gain", &geTreeInfo.calib_gain);
    i_ge->geTree->Branch("calib_offset", &geTreeInfo.calib_offset);
  }

  // Create the cross-correlation between the ge and the active target tree
  tree_basename = "geTargetTree_";
  for (std::vector<GeTargetOutput>::iterator i_geTarget = final_output.geTargets.begin(); i_geTarget != final_output.geTargets.end(); ++i_geTarget) {
    std::string treename = tree_basename + i_geTarget->geInfo->name;
    i_geTarget->geTargetTree = new TTree(treename.c_str(), treename.c_str());
    i_geTarget->geTargetTree->Branch("run_id", &geTargetTreeInfo.run_id);
    i_geTarget->geTargetTree->Branch("block_id", &geTargetTreeInfo.block_id);
    i_geTarget->geTargetTree->Branch("tme_id", &geTargetTreeInfo.tme_id);
    i_geTarget->geTargetTree->Branch("central_muon_time", &geTargetTreeInfo.central_muon_time);
    
    i_geTarget->geTargetTree->Branch("ge_tpi_id", &geTargetTreeInfo.ge_tpi_id);
    i_geTarget->geTargetTree->Branch("ge_tap_id", &geTargetTreeInfo.ge_tap_id);
    i_geTarget->geTargetTree->Branch("ge_energy", &geTargetTreeInfo.ge_energy);
    i_geTarget->geTargetTree->Branch("ge_time", &geTargetTreeInfo.ge_time);
    i_geTarget->geTargetTree->Branch("ge_calib_gain", &geTargetTreeInfo.ge_calib_gain);
    i_geTarget->geTargetTree->Branch("ge_calib_offset", &geTargetTreeInfo.ge_calib_offset);

    i_geTarget->geTargetTree->Branch("thin_energy", &geTargetTreeInfo.thin_energy);
    i_geTarget->geTargetTree->Branch("thin_time", &geTargetTreeInfo.thin_time);
    i_geTarget->geTargetTree->Branch("thin_tpi_id", &geTargetTreeInfo.thin_tpi_id);
    i_geTarget->geTargetTree->Branch("thin_tap_id", &geTargetTreeInfo.thin_tap_id);
    i_geTarget->geTargetTree->Branch("thin_channel", &geTargetTreeInfo.thin_channel);
    i_geTarget->geTargetTree->Branch("thin_calib_gain", &geTargetTreeInfo.thin_calib_gain);
    i_geTarget->geTargetTree->Branch("thin_calib_offset", &geTargetTreeInfo.thin_calib_offset);

    i_geTarget->geTargetTree->Branch("thick_energy", &geTargetTreeInfo.thick_energy);
    i_geTarget->geTargetTree->Branch("thick_time", &geTargetTreeInfo.thick_time);
    i_geTarget->geTargetTree->Branch("thick_tpi_id", &geTargetTreeInfo.thick_tpi_id);
    i_geTarget->geTargetTree->Branch("thick_tap_id", &geTargetTreeInfo.thick_tap_id);
    i_geTarget->geTargetTree->Branch("thick_channel", &geTargetTreeInfo.thick_channel);
    i_geTarget->geTargetTree->Branch("thick_calib_gain", &geTargetTreeInfo.thick_calib_gain);
    i_geTarget->geTargetTree->Branch("thick_calib_offset", &geTargetTreeInfo.thick_calib_offset);

    i_geTarget->geTargetTree->Branch("third_energy", &geTargetTreeInfo.third_energy);
    i_geTarget->geTargetTree->Branch("third_time", &geTargetTreeInfo.third_time);
    i_geTarget->geTargetTree->Branch("third_tpi_id", &geTargetTreeInfo.third_tpi_id);
    i_geTarget->geTargetTree->Branch("third_tap_id", &geTargetTreeInfo.third_tap_id);
    i_geTarget->geTargetTree->Branch("third_channel", &geTargetTreeInfo.third_channel);
    i_geTarget->geTargetTree->Branch("third_calib_gain", &geTargetTreeInfo.third_calib_gain);
    i_geTarget->geTargetTree->Branch("third_calib_offset", &geTargetTreeInfo.third_calib_offset);
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

  double prevCentralMuonTime = -1;
  for (int i_tme = 0; i_tme < n_entries; ++i_tme) {

    ++n_total_tmes;
    
    if (i_tme % 100000 == 0) {
      std::cout << i_tme << " / " << n_entries << std::endl;
    }

    tmetree->GetEntry(i_tme);
    CollectChannels(); // collect all channels into various groups (e.g. SiR1, SiL1, All) for easy looping

    /////////////////////////////////////
    // Event Vetoes
    if (args.reqd_run_id>0) {
      if (runId != args.reqd_run_id) {
	continue; // to the next TME
      }
    }
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
    //    double muSc_amp = (*args.muon_channels[0])->at(0).Amp;
    //    if (muSc_amp < args.muScCutLo || muSc_amp>args.muScCutHi) {
    //      continue; // to the next TME
    //    }
    if (args.req_tgt_muon) {
      const std::vector<std::vector<SimplePulse>** >* tgt_channels_ptr = NULL;
      if (args.tgt_layer == 1) {
	tgt_channels_ptr = &args.target.layer1_channels;
      }
      else if (args.tgt_layer == 2) {
	tgt_channels_ptr = &args.target.layer2_channels;
      }
      else { std::cout << "ERROR: Invalid args.tgt_layer (" << args.tgt_layer << ")" << std::endl; return;
      }
      std::vector<std::vector<SimplePulse>** > tgt_channels = *tgt_channels_ptr;
      // count all the target pulses
      int n_total_tgt_pulses = 0;
      for (int i_tgt_channel = 0; i_tgt_channel < tgt_channels.size(); ++i_tgt_channel) {
	n_total_tgt_pulses += (*tgt_channels.at(i_tgt_channel))->size();
      }
      if( n_total_tgt_pulses < args.min_tgt_pulses) {
	continue; // to the next TME
      }
      /*      else {
	double calib_gain = args.target.layer2_calibGains.at(0);
	double calib_offset = args.target.layer2_calibOffsets.at(0);
	double time = (*args.target.layer2_channels.at(0))->at(0).tTME;
	double energy = (*args.target.layer2_channels.at(0))->at(0).Amp * calib_gain + calib_offset;

	if (time < -300 || time > 300 || energy < 3000 || energy > 6000) { // muon definition
	  continue; // to the next TME
	}
	}*/
    }
    if (args.veto_pp_window) {
      if (timeToNextTME < args.pp_window || timeToPrevTME < args.pp_window) {
	continue; // to the next TME
      }
    }
    
    ++n_analysed_tmes;

    if (prevCentralMuonTime < 0) {
      prevCentralMuonTime = centralMuonTime;
    }
    else {
      final_output.hCentralMuonTDiff->Fill(std::fabs(centralMuonTime - prevCentralMuonTime));
      prevCentralMuonTime = centralMuonTime;
    }

    /////////////////////////////////
    // Fill SiBlock trees

    for (std::vector<SiBlockOutput>::iterator i_siBlock = final_output.siBlocks.begin(); i_siBlock != final_output.siBlocks.end(); ++i_siBlock) {

      siBlockTreeInfo.run_id = runId;
      siBlockTreeInfo.block_id = blockId;
      siBlockTreeInfo.tme_id = TMEId;
      siBlockTreeInfo.central_muon_time = centralMuonTime;
      
      // Reset these branches
      siBlockTreeInfo.thin_energy = 0;
      siBlockTreeInfo.thin_time = 0;
      siBlockTreeInfo.thin_tpi_id = -1;
      siBlockTreeInfo.thin_tap_id = -1;
      siBlockTreeInfo.thin_channel = -1;
      siBlockTreeInfo.thin_calib_gain = 0;
      siBlockTreeInfo.thin_calib_offset = 0;

      siBlockTreeInfo.thick_energy = 0;
      siBlockTreeInfo.thick_time = 0;
      siBlockTreeInfo.thick_tpi_id = -1;
      siBlockTreeInfo.thick_tap_id = -1;
      siBlockTreeInfo.thick_channel = -1;
      siBlockTreeInfo.thick_calib_gain = 0;
      siBlockTreeInfo.thick_calib_offset = 0;
      
      siBlockTreeInfo.third_energy = 0;
      siBlockTreeInfo.third_time = 0;
      siBlockTreeInfo.third_tpi_id = -1;
      siBlockTreeInfo.third_tap_id = -1;
      siBlockTreeInfo.third_channel = -1;
      siBlockTreeInfo.third_calib_gain = 0;
      siBlockTreeInfo.third_calib_offset = 0;

      // First count all the pulses in each layer
      // Loop through layer 1 channels
      int n_thin_channels = i_siBlock->siBlockInfo->layer1_channels.size();
      int n_total_thin_pulses = 0;
      for (int i_thin_channel = 0; i_thin_channel < n_thin_channels; ++i_thin_channel) {
	std::vector<SimplePulse>* i_thin_pulse_list = *(i_siBlock->siBlockInfo->layer1_channels.at(i_thin_channel));
	n_total_thin_pulses += i_thin_pulse_list->size();
      }
      
      // Loop through layer 2 channels
      int n_thick_channels = i_siBlock->siBlockInfo->layer2_channels.size();
      int n_total_thick_pulses = 0;
      for (int i_thick_channel = 0; i_thick_channel < n_thick_channels; ++i_thick_channel) {
	std::vector<SimplePulse>* i_thick_pulse_list = *(i_siBlock->siBlockInfo->layer2_channels.at(i_thick_channel));
	n_total_thick_pulses += i_thick_pulse_list->size();
      }
      
      // Loop through layer 3 channels
      int n_third_channels = i_siBlock->siBlockInfo->layer3_channels.size();
      int n_total_third_pulses = 0;
      for (int i_third_channel = 0; i_third_channel < n_third_channels; ++i_third_channel) {
	std::vector<SimplePulse>* i_third_pulse_list = *(i_siBlock->siBlockInfo->layer3_channels.at(i_third_channel));
	n_total_third_pulses += i_third_pulse_list->size();
      }
      if (n_total_thin_pulses == 0 && n_total_thick_pulses == 0 && n_total_third_pulses == 0) {
	continue; // to next SiBlock
      }
      //      std::cout << i_siBlock->siBlockInfo->name << " SiBlock" << std::endl;
      //      std::cout << "(N Thin, N Thick, N Third) = (" << n_total_thin_pulses << ", " << n_total_thick_pulses << ", " << n_total_third_pulses << ")" << std::endl;

      // Now that we know how many total pulses there are, we can now fill the tree safely
      for (int i_thin_channel = 0; i_thin_channel < n_thin_channels; ++i_thin_channel) {
	std::vector<SimplePulse>* i_thin_pulse_list = *(i_siBlock->siBlockInfo->layer1_channels.at(i_thin_channel));
	double i_thin_calib_gain = i_siBlock->siBlockInfo->layer1_calibGains.at(i_thin_channel);
	double i_thin_calib_offset = i_siBlock->siBlockInfo->layer1_calibOffsets.at(i_thin_channel);
	int n_thin_pulses = i_thin_pulse_list->size();
	// Loop through layer 2 channels
	for (int i_thick_channel = 0; i_thick_channel < n_thick_channels; ++i_thick_channel) {
	  std::vector<SimplePulse>* i_thick_pulse_list = *(i_siBlock->siBlockInfo->layer2_channels.at(i_thick_channel));
	  double i_thick_calib_gain = i_siBlock->siBlockInfo->layer2_calibGains.at(i_thick_channel);
	  double i_thick_calib_offset = i_siBlock->siBlockInfo->layer2_calibOffsets.at(i_thick_channel);
	  int n_thick_pulses = i_thick_pulse_list->size();

	  int n_third_channels = i_siBlock->siBlockInfo->layer3_channels.size();
	  for (int i_third_channel = 0; i_third_channel < n_third_channels; ++i_third_channel) {
	    std::vector<SimplePulse>* i_third_pulse_list = *(i_siBlock->siBlockInfo->layer3_channels.at(i_third_channel));
	    double i_third_calib_gain = i_siBlock->siBlockInfo->layer3_calibGains.at(i_third_channel);
	    double i_third_calib_offset = i_siBlock->siBlockInfo->layer3_calibOffsets.at(i_third_channel);
	    int n_third_pulses = i_third_pulse_list->size();

	    // If we have hits in all channels
	    for (int i_thin_pulse = 0; i_thin_pulse < n_thin_pulses; ++i_thin_pulse) {
	      siBlockTreeInfo.thin_time = i_thin_pulse_list->at(i_thin_pulse).tTME;
	      //	      siBlockTreeInfo.thin_energy = i_thin_pulse_list->at(i_thin_pulse).E;
	      siBlockTreeInfo.thin_energy = i_thin_pulse_list->at(i_thin_pulse).Amp * i_thin_calib_gain + i_thin_calib_offset;
	      siBlockTreeInfo.thin_tpi_id = i_thin_pulse_list->at(i_thin_pulse).tpi_id;
	      siBlockTreeInfo.thin_tap_id = i_thin_pulse_list->at(i_thin_pulse).tap_id;
	      siBlockTreeInfo.thin_channel = i_thin_channel;
	      siBlockTreeInfo.thin_calib_gain = i_thin_calib_gain;
	      siBlockTreeInfo.thin_calib_offset = i_thin_calib_offset;
	      for (int i_thick_pulse = 0; i_thick_pulse < n_thick_pulses; ++i_thick_pulse) {
		siBlockTreeInfo.thick_time = i_thick_pulse_list->at(i_thick_pulse).tTME;
		//		siBlockTreeInfo.thick_energy = i_thick_pulse_list->at(i_thick_pulse).E;
		siBlockTreeInfo.thick_energy = i_thick_pulse_list->at(i_thick_pulse).Amp * i_thick_calib_gain + i_thick_calib_offset;
		siBlockTreeInfo.thick_tpi_id = i_thick_pulse_list->at(i_thick_pulse).tpi_id;
		siBlockTreeInfo.thick_tap_id = i_thick_pulse_list->at(i_thick_pulse).tap_id;
		siBlockTreeInfo.thick_channel = i_thick_channel;
		siBlockTreeInfo.thick_calib_gain = i_thick_calib_gain;
		siBlockTreeInfo.thick_calib_offset = i_thick_calib_offset;
		for (int i_third_pulse = 0; i_third_pulse < n_third_pulses; ++i_third_pulse) {
		  siBlockTreeInfo.third_time = i_third_pulse_list->at(i_third_pulse).tTME;
		  //		  siBlockTreeInfo.third_energy = i_third_pulse_list->at(i_third_pulse).E;
		  siBlockTreeInfo.third_energy = i_third_pulse_list->at(i_third_pulse).Amp * i_third_calib_gain + i_third_calib_offset;
		  siBlockTreeInfo.third_tpi_id = i_third_pulse_list->at(i_third_pulse).tpi_id;
		  siBlockTreeInfo.third_tap_id = i_third_pulse_list->at(i_third_pulse).tap_id;
		  siBlockTreeInfo.third_channel = i_third_channel;
		  siBlockTreeInfo.third_calib_gain = i_third_calib_gain;
		  siBlockTreeInfo.third_calib_offset = i_third_calib_offset;
		  //		  std::cout << "==> Filling 123" << std::endl;
		  i_siBlock->siBlockTree->Fill(); // filling when hits in layers 1,2,3
		} // end loop through layer 3 pulses
		if (n_total_third_pulses == 0 && i_third_channel == 0) { // only fill once
		  //		  std::cout << "==> Filling 12" << std::endl;
		  i_siBlock->siBlockTree->Fill(); // filling when hits in layers 1,2
		}
	      } // end loop through layer 2 pulses
	      if (n_total_thick_pulses == 0 && i_thick_channel == 0) {
		for (int i_third_pulse = 0; i_third_pulse < n_third_pulses; ++i_third_pulse) {
		  siBlockTreeInfo.third_time = i_third_pulse_list->at(i_third_pulse).tTME;
		  //		  siBlockTreeInfo.third_energy = i_third_pulse_list->at(i_third_pulse).E;
		  siBlockTreeInfo.third_energy = i_third_pulse_list->at(i_third_pulse).Amp * i_third_calib_gain + i_third_calib_offset;
		  siBlockTreeInfo.third_tpi_id = i_third_pulse_list->at(i_third_pulse).tpi_id;
		  siBlockTreeInfo.third_tap_id = i_third_pulse_list->at(i_third_pulse).tap_id;
		  siBlockTreeInfo.third_channel = i_third_channel;
		  siBlockTreeInfo.third_calib_gain = i_third_calib_gain;
		  siBlockTreeInfo.third_calib_offset = i_third_calib_offset;
		  //		  std::cout << "==> Filling 13" << std::endl;
		  i_siBlock->siBlockTree->Fill(); // filling when hits in layers 1
		} // end loop through layer 3 pulses
	      }
	      if (n_total_thick_pulses == 0 && i_thick_channel == 0 &&
		  n_total_third_pulses == 0 && i_third_channel == 0) {
		//		  std::cout << "==> Filling 1" << std::endl;
		  i_siBlock->siBlockTree->Fill(); // filling when hits in layers 1
	      }		
	    } // end loop through layer 1 pulses
	      
	    if (n_total_thin_pulses == 0 && i_thin_channel == 0) {
	      for (int i_thick_pulse = 0; i_thick_pulse < n_thick_pulses; ++i_thick_pulse) {
		siBlockTreeInfo.thick_time = i_thick_pulse_list->at(i_thick_pulse).tTME;
		//		siBlockTreeInfo.thick_energy = i_thick_pulse_list->at(i_thick_pulse).E;
		siBlockTreeInfo.thick_energy = i_thick_pulse_list->at(i_thick_pulse).Amp * i_thick_calib_gain + i_thick_calib_offset;
		siBlockTreeInfo.thick_tpi_id = i_thick_pulse_list->at(i_thick_pulse).tpi_id;
		siBlockTreeInfo.thick_tap_id = i_thick_pulse_list->at(i_thick_pulse).tap_id;
		siBlockTreeInfo.thick_channel = i_thick_channel;
		siBlockTreeInfo.thick_calib_gain = i_thick_calib_gain;
		siBlockTreeInfo.thick_calib_offset = i_thick_calib_offset;
		for (int i_third_pulse = 0; i_third_pulse < n_third_pulses; ++i_third_pulse) {
		  siBlockTreeInfo.third_time = i_third_pulse_list->at(i_third_pulse).tTME;
		  //		  siBlockTreeInfo.third_energy = i_third_pulse_list->at(i_third_pulse).E;
		  siBlockTreeInfo.third_energy = i_third_pulse_list->at(i_third_pulse).Amp * i_third_calib_gain + i_third_calib_offset;
		  siBlockTreeInfo.third_tpi_id = i_third_pulse_list->at(i_third_pulse).tpi_id;
		  siBlockTreeInfo.third_tap_id = i_third_pulse_list->at(i_third_pulse).tap_id;
		  siBlockTreeInfo.third_channel = i_third_channel;
		  siBlockTreeInfo.third_calib_gain = i_third_calib_gain;
		  siBlockTreeInfo.third_calib_offset = i_third_calib_offset;
		  //		  std::cout << "==> Filling 23" << std::endl;
		  i_siBlock->siBlockTree->Fill(); // filling when hits in layers 2,3
		} // end loop through layer 3 pulses
		if (n_total_third_pulses == 0 && i_third_channel == 0) {
		  //		  std::cout << "==> Filling 2" << std::endl;
		  i_siBlock->siBlockTree->Fill(); // filling when hits in layers 2
		}
	      } // end loop through layer 2 pulses
	      if (n_total_thick_pulses == 0 && i_thick_channel == 0) {
		for (int i_third_pulse = 0; i_third_pulse < n_third_pulses; ++i_third_pulse) {
		  siBlockTreeInfo.third_time = i_third_pulse_list->at(i_third_pulse).tTME;
		  //		  siBlockTreeInfo.third_energy = i_third_pulse_list->at(i_third_pulse).E;
		  siBlockTreeInfo.third_energy = i_third_pulse_list->at(i_third_pulse).Amp * i_third_calib_gain + i_third_calib_offset;
		  siBlockTreeInfo.third_tpi_id = i_third_pulse_list->at(i_third_pulse).tpi_id;
		  siBlockTreeInfo.third_tap_id = i_third_pulse_list->at(i_third_pulse).tap_id;
		  siBlockTreeInfo.third_channel = i_third_channel;
		  siBlockTreeInfo.third_calib_gain = i_third_calib_gain;
		  siBlockTreeInfo.third_calib_offset = i_third_calib_offset;
		  //		  std::cout << "==> Filling 3" << std::endl;
		  i_siBlock->siBlockTree->Fill(); // filling when hits in layers 3
		} // end loop through layer 3 pulses
	      }
	    }
	  } // end loop through third channel
	} // end loop through second channel
      } // end loop through thin channel
    }
    
    /////////////////////////////////////////////
    // Fill GeOutput tree
    for (std::vector<GeOutput>::iterator i_ge = final_output.ges.begin(); i_ge != final_output.ges.end(); ++i_ge) {
      geTreeInfo.run_id = runId;
      geTreeInfo.block_id = blockId;
      geTreeInfo.tme_id = TMEId;
      geTreeInfo.central_muon_time = centralMuonTime;
      
      // Reset these branches
      geTreeInfo.energy = 0;
      geTreeInfo.time = 0;
      geTreeInfo.tpi_id = -1;
      geTreeInfo.tap_id = -1;
      geTreeInfo.calib_gain = 0;
      geTreeInfo.calib_offset = 0;

      std::vector<SimplePulse>* i_ge_pulse_list = *(i_ge->geInfo->channel);
      double i_calib_gain = i_ge->geInfo->calibGain;
      double i_calib_offset = i_ge->geInfo->calibOffset;
      int n_ge_pulses = i_ge_pulse_list->size();
      for (int i_ge_pulse = 0; i_ge_pulse < n_ge_pulses; ++i_ge_pulse) {
	geTreeInfo.tpi_id = i_ge_pulse_list->at(i_ge_pulse).tpi_id;
	geTreeInfo.tap_id = i_ge_pulse_list->at(i_ge_pulse).tap_id;
	//	geTreeInfo.energy = i_ge_pulse_list->at(i_ge_pulse).E;
	geTreeInfo.energy = i_ge_pulse_list->at(i_ge_pulse).Amp * i_ge->geInfo->calibGain + i_ge->geInfo->calibOffset;
	geTreeInfo.time = i_ge_pulse_list->at(i_ge_pulse).tTME;
	geTreeInfo.calib_gain = i_calib_gain;
	geTreeInfo.calib_offset = i_calib_offset;
	
	i_ge->geTree->Fill();
      }      
    }

    ///////////////////////////////////////
    // Fill geTarget tree
    for (std::vector<GeTargetOutput>::iterator i_geTarget = final_output.geTargets.begin(); i_geTarget != final_output.geTargets.end(); ++i_geTarget) {
      geTargetTreeInfo.run_id = runId;
      geTargetTreeInfo.block_id = blockId;
      geTargetTreeInfo.tme_id = TMEId;
      geTargetTreeInfo.central_muon_time = centralMuonTime;

      geTargetTreeInfo.ge_tpi_id = -1;
      geTargetTreeInfo.ge_tap_id = -1;
      geTargetTreeInfo.ge_energy = 0;
      geTargetTreeInfo.ge_time = 0;
      geTargetTreeInfo.ge_calib_gain = 0;
      geTargetTreeInfo.ge_calib_offset = 0;

      std::vector<SimplePulse>* i_ge_pulse_list = *(i_geTarget->geInfo->channel);
      double i_ge_calib_gain = i_geTarget->geInfo->calibGain;
      double i_ge_calib_offset = i_geTarget->geInfo->calibOffset;
      int n_ge_pulses = i_ge_pulse_list->size();
      for (int i_ge_pulse = 0; i_ge_pulse < n_ge_pulses; ++i_ge_pulse) {
	//	double ge_energy = i_ge_pulse_list->at(i_ge_pulse).E;
	double ge_energy = i_ge_pulse_list->at(i_ge_pulse).Amp * i_ge_calib_gain + i_ge_calib_offset;
	double ge_time = i_ge_pulse_list->at(i_ge_pulse).tTME;
	double ge_tpi_id = i_ge_pulse_list->at(i_ge_pulse).tpi_id;
	geTargetTreeInfo.ge_tpi_id = ge_tpi_id;
	geTargetTreeInfo.ge_energy = ge_energy;
	geTargetTreeInfo.ge_time = ge_time;
	geTargetTreeInfo.ge_calib_gain = i_ge_calib_gain;
	geTargetTreeInfo.ge_calib_offset = i_ge_calib_offset;

	geTargetTreeInfo.thin_energy = 0;
	geTargetTreeInfo.thin_time = 0;
	geTargetTreeInfo.thin_tpi_id = -1;
	geTargetTreeInfo.thin_tap_id = -1;
	geTargetTreeInfo.thin_channel = -1;
	geTargetTreeInfo.thin_calib_gain = 0;
	geTargetTreeInfo.thin_calib_offset = 0;

	geTargetTreeInfo.thick_energy = 0;
	geTargetTreeInfo.thick_time = 0;
	geTargetTreeInfo.thick_tpi_id = -1;
	geTargetTreeInfo.thick_tap_id = -1;
	geTargetTreeInfo.thick_channel = -1;
	geTargetTreeInfo.thick_calib_gain = 0;
	geTargetTreeInfo.thick_calib_offset = 0;
	
	geTargetTreeInfo.third_energy = 0;
	geTargetTreeInfo.third_time = 0;
	geTargetTreeInfo.third_tpi_id = -1;
	geTargetTreeInfo.third_tap_id = -1;
	geTargetTreeInfo.third_channel = -1;
	geTargetTreeInfo.third_calib_gain = 0;
	geTargetTreeInfo.third_calib_offset = 0;


	// First count all the pulses in each layer
	// Loop through layer 1 channels
	int n_thin_channels = i_geTarget->targetInfo->layer1_channels.size();
	int n_total_thin_pulses = 0;
	for (int i_thin_channel = 0; i_thin_channel < n_thin_channels; ++i_thin_channel) {
	  std::vector<SimplePulse>* i_thin_pulse_list = *(i_geTarget->targetInfo->layer1_channels.at(i_thin_channel));
	  n_total_thin_pulses += i_thin_pulse_list->size();
	}
      
	// Loop through layer 2 channels
	int n_thick_channels = i_geTarget->targetInfo->layer2_channels.size();
	int n_total_thick_pulses = 0;
	for (int i_thick_channel = 0; i_thick_channel < n_thick_channels; ++i_thick_channel) {
	  std::vector<SimplePulse>* i_thick_pulse_list = *(i_geTarget->targetInfo->layer2_channels.at(i_thick_channel));
	  n_total_thick_pulses += i_thick_pulse_list->size();
	}
      
	// Loop through layer 3 channels
	int n_third_channels = i_geTarget->targetInfo->layer3_channels.size();
	int n_total_third_pulses = 0;
	for (int i_third_channel = 0; i_third_channel < n_third_channels; ++i_third_channel) {
	  std::vector<SimplePulse>* i_third_pulse_list = *(i_geTarget->targetInfo->layer3_channels.at(i_third_channel));
	  n_total_third_pulses += i_third_pulse_list->size();
	}
	if (n_total_thin_pulses == 0 && n_total_thick_pulses == 0 && n_total_third_pulses == 0) {
	  continue; // to next SiBlock
	}
	//      std::cout << i_geTarget->targetInfo->name << " SiBlock" << std::endl;
	//      std::cout << "(N Thin, N Thick, N Third) = (" << n_total_thin_pulses << ", " << n_total_thick_pulses << ", " << n_total_third_pulses << ")" << std::endl;

	// Now that we know how many total pulses there are, we can now fill the tree safely
	for (int i_thin_channel = 0; i_thin_channel < n_thin_channels; ++i_thin_channel) {
	  std::vector<SimplePulse>* i_thin_pulse_list = *(i_geTarget->targetInfo->layer1_channels.at(i_thin_channel));
	  double i_thin_calib_gain = i_geTarget->targetInfo->layer1_calibGains.at(i_thin_channel);
	  double i_thin_calib_offset = i_geTarget->targetInfo->layer1_calibOffsets.at(i_thin_channel);
	  int n_thin_pulses = i_thin_pulse_list->size();
	  // Loop through layer 2 channels
	  for (int i_thick_channel = 0; i_thick_channel < n_thick_channels; ++i_thick_channel) {
	    std::vector<SimplePulse>* i_thick_pulse_list = *(i_geTarget->targetInfo->layer2_channels.at(i_thick_channel));
	    double i_thick_calib_gain = i_geTarget->targetInfo->layer2_calibGains.at(i_thick_channel);
	    double i_thick_calib_offset = i_geTarget->targetInfo->layer2_calibOffsets.at(i_thick_channel);
	    int n_thick_pulses = i_thick_pulse_list->size();

	    int n_third_channels = i_geTarget->targetInfo->layer3_channels.size();
	    for (int i_third_channel = 0; i_third_channel < n_third_channels; ++i_third_channel) {
	      std::vector<SimplePulse>* i_third_pulse_list = *(i_geTarget->targetInfo->layer3_channels.at(i_third_channel));
	      double i_third_calib_gain = i_geTarget->targetInfo->layer3_calibGains.at(i_third_channel);
	      double i_third_calib_offset = i_geTarget->targetInfo->layer3_calibOffsets.at(i_third_channel);
	      int n_third_pulses = i_third_pulse_list->size();

	      // If we have hits in all channels
	      for (int i_thin_pulse = 0; i_thin_pulse < n_thin_pulses; ++i_thin_pulse) {
		geTargetTreeInfo.thin_time = i_thin_pulse_list->at(i_thin_pulse).tTME;
		//		geTargetTreeInfo.thin_energy = i_thin_pulse_list->at(i_thin_pulse).E;
		geTargetTreeInfo.thin_energy = i_thin_pulse_list->at(i_thin_pulse).Amp * i_thin_calib_gain + i_thin_calib_offset;
		geTargetTreeInfo.thin_tpi_id = i_thin_pulse_list->at(i_thin_pulse).tpi_id;
		geTargetTreeInfo.thin_tap_id = i_thin_pulse_list->at(i_thin_pulse).tap_id;
		geTargetTreeInfo.thin_channel = i_thin_channel;
		geTargetTreeInfo.thin_calib_gain = i_thin_calib_gain;
		geTargetTreeInfo.thin_calib_offset = i_thin_calib_offset;
		for (int i_thick_pulse = 0; i_thick_pulse < n_thick_pulses; ++i_thick_pulse) {
		  geTargetTreeInfo.thick_time = i_thick_pulse_list->at(i_thick_pulse).tTME;
		  //		  geTargetTreeInfo.thick_energy = i_thick_pulse_list->at(i_thick_pulse).E;
		  geTargetTreeInfo.thick_energy = i_thick_pulse_list->at(i_thick_pulse).Amp * i_thick_calib_gain + i_thick_calib_offset;
		  geTargetTreeInfo.thick_tpi_id = i_thick_pulse_list->at(i_thick_pulse).tpi_id;
		  geTargetTreeInfo.thick_tap_id = i_thick_pulse_list->at(i_thick_pulse).tap_id;
		  geTargetTreeInfo.thick_channel = i_thick_channel;
		  geTargetTreeInfo.thick_calib_gain = i_thick_calib_gain;
		  geTargetTreeInfo.thick_calib_offset = i_thick_calib_offset;
		  for (int i_third_pulse = 0; i_third_pulse < n_third_pulses; ++i_third_pulse) {
		    geTargetTreeInfo.third_time = i_third_pulse_list->at(i_third_pulse).tTME;
		    //		    geTargetTreeInfo.third_energy = i_third_pulse_list->at(i_third_pulse).E;
		    geTargetTreeInfo.third_energy = i_third_pulse_list->at(i_third_pulse).Amp * i_third_calib_gain + i_third_calib_offset;
		    geTargetTreeInfo.third_tpi_id = i_third_pulse_list->at(i_third_pulse).tpi_id;
		    geTargetTreeInfo.third_tap_id = i_third_pulse_list->at(i_third_pulse).tap_id;
		    geTargetTreeInfo.third_channel = i_third_channel;
		    geTargetTreeInfo.third_calib_gain = i_third_calib_gain;
		    geTargetTreeInfo.third_calib_offset = i_third_calib_offset;
		    //		  std::cout << "==> Filling 123" << std::endl;
		    i_geTarget->geTargetTree->Fill(); // filling when hits in layers 1,2,3
		  } // end loop through layer 3 pulses
		  if (n_total_third_pulses == 0 && i_third_channel == 0) { // only fill once
		    //		  std::cout << "==> Filling 12" << std::endl;
		    i_geTarget->geTargetTree->Fill(); // filling when hits in layers 1,2
		  }
		} // end loop through layer 2 pulses
		if (n_total_thick_pulses == 0 && i_thick_channel == 0) {
		  for (int i_third_pulse = 0; i_third_pulse < n_third_pulses; ++i_third_pulse) {
		    geTargetTreeInfo.third_time = i_third_pulse_list->at(i_third_pulse).tTME;
		    //		    geTargetTreeInfo.third_energy = i_third_pulse_list->at(i_third_pulse).E;
		    geTargetTreeInfo.third_energy = i_third_pulse_list->at(i_third_pulse).Amp * i_third_calib_gain + i_third_calib_offset;
		    geTargetTreeInfo.third_tpi_id = i_third_pulse_list->at(i_third_pulse).tpi_id;
		    geTargetTreeInfo.third_tap_id = i_third_pulse_list->at(i_third_pulse).tap_id;
		    geTargetTreeInfo.third_channel = i_third_channel;
		    geTargetTreeInfo.third_calib_gain = i_third_calib_gain;
		    geTargetTreeInfo.third_calib_offset = i_third_calib_offset;
		    //		  std::cout << "==> Filling 13" << std::endl;
		    i_geTarget->geTargetTree->Fill(); // filling when hits in layers 1
		  } // end loop through layer 3 pulses
		}
		if (n_total_thick_pulses == 0 && i_thick_channel == 0 &&
		    n_total_third_pulses == 0 && i_third_channel == 0) {
		  //		  std::cout << "==> Filling 1" << std::endl;
		  i_geTarget->geTargetTree->Fill(); // filling when hits in layers 1
		}		
	      } // end loop through layer 1 pulses
	      
	      if (n_total_thin_pulses == 0 && i_thin_channel == 0) {
		for (int i_thick_pulse = 0; i_thick_pulse < n_thick_pulses; ++i_thick_pulse) {
		  geTargetTreeInfo.thick_time = i_thick_pulse_list->at(i_thick_pulse).tTME;
		  //		  geTargetTreeInfo.thick_energy = i_thick_pulse_list->at(i_thick_pulse).E;
		  geTargetTreeInfo.thick_energy = i_thick_pulse_list->at(i_thick_pulse).Amp * i_thick_calib_gain + i_thick_calib_offset;
		  geTargetTreeInfo.thick_tpi_id = i_thick_pulse_list->at(i_thick_pulse).tpi_id;
		  geTargetTreeInfo.thick_tap_id = i_thick_pulse_list->at(i_thick_pulse).tap_id;
		  geTargetTreeInfo.thick_channel = i_thick_channel;
		  geTargetTreeInfo.thick_calib_gain = i_thick_calib_gain;
		  geTargetTreeInfo.thick_calib_offset = i_thick_calib_offset;
		  for (int i_third_pulse = 0; i_third_pulse < n_third_pulses; ++i_third_pulse) {
		    geTargetTreeInfo.third_time = i_third_pulse_list->at(i_third_pulse).tTME;
		    //		    geTargetTreeInfo.third_energy = i_third_pulse_list->at(i_third_pulse).E;
		    geTargetTreeInfo.third_energy = i_third_pulse_list->at(i_third_pulse).Amp * i_third_calib_gain + i_third_calib_offset;
		    geTargetTreeInfo.third_tpi_id = i_third_pulse_list->at(i_third_pulse).tpi_id;
		    geTargetTreeInfo.third_tap_id = i_third_pulse_list->at(i_third_pulse).tap_id;
		    geTargetTreeInfo.third_channel = i_third_channel;
		    geTargetTreeInfo.third_calib_gain = i_third_calib_gain;
		    geTargetTreeInfo.third_calib_offset = i_third_calib_offset;
		    //		  std::cout << "==> Filling 23" << std::endl;
		    i_geTarget->geTargetTree->Fill(); // filling when hits in layers 2,3
		  } // end loop through layer 3 pulses
		  if (n_total_third_pulses == 0 && i_third_channel == 0) {
		    //		  std::cout << "==> Filling 2" << std::endl;
		    i_geTarget->geTargetTree->Fill(); // filling when hits in layers 2
		  }
		} // end loop through layer 2 pulses
		if (n_total_thick_pulses == 0 && i_thick_channel == 0) {
		  for (int i_third_pulse = 0; i_third_pulse < n_third_pulses; ++i_third_pulse) {
		    geTargetTreeInfo.third_time = i_third_pulse_list->at(i_third_pulse).tTME;
		    //		    geTargetTreeInfo.third_energy = i_third_pulse_list->at(i_third_pulse).E;
		    geTargetTreeInfo.third_energy = i_third_pulse_list->at(i_third_pulse).Amp * i_third_calib_gain + i_third_calib_offset;
		    geTargetTreeInfo.third_tpi_id = i_third_pulse_list->at(i_third_pulse).tpi_id;
		    geTargetTreeInfo.third_tap_id = i_third_pulse_list->at(i_third_pulse).tap_id;
		    geTargetTreeInfo.third_channel = i_third_channel;
		    geTargetTreeInfo.third_calib_gain = i_third_calib_gain;
		    geTargetTreeInfo.third_calib_offset = i_third_calib_offset;
		    //		  std::cout << "==> Filling 3" << std::endl;
		    i_geTarget->geTargetTree->Fill(); // filling when hits in layers 3
		  } // end loop through layer 3 pulses
		}
	      }
	    } // end loop through third channel
	  } // end loop through second channel
	} // end loop through thin channel
      }
    }
  } // end TME loop
  
  final_output.Write();
  outfile->Close();
}
