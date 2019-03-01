#include "TFile.h"
#include "TH2.h"
#include "TTree.h"

#include "TF1.h"

#include "scripts/TMETree/AlCapConstants.h"
#include "scripts/TMETree/TMETreeBranches.h"

#include <iostream>

struct TargetBlockInfo {
  std::string name;
  std::vector<SimplePulse>** channel;
  double calibGain;
  double calibOffset;
};

struct TargetBlockTreeInfo {
  int run_id;
  int block_id;
  int tme_id;
  
  double energy;
  double time;
  int tpi_id;
  int tap_id;
  int channel;
  double calib_gain;
  double calib_offset;
} targetBlockTreeInfo;

struct TargetBlockOutput {
  TargetBlockOutput() {};
  TargetBlockOutput(const TargetBlockInfo* a) : targetBlockInfo(a) {};

  const TargetBlockInfo* targetBlockInfo;
  TTree* targetBlockTree;
};

struct SpecialTMELoopArgs {
  // Standard parameters
  std::string infilename;
  std::string tmetreename;
  std::string outfilename;
  int n_entries;

  // Channel configuration
  std::vector<std::vector<SimplePulse>** > muon_channels;
  TargetBlockInfo target; // for active target runs

  // Event vetoes
  bool veto_any_double_counts;
  bool veto_max_muon_channel_pulses;
  int max_muon_channel_pulses;
  double min_muon_energy;
  double max_muon_energy;
  double min_muon_time;
  double max_muon_time;
};

struct SpecialTMELoopOutput {
  TargetBlockOutput targetBlock;

  TTree* infotree;

  void Write() {
    if(targetBlock.targetBlockTree) {
      targetBlock.targetBlockTree->Write();
    }
    
    if (infotree) {
      infotree->Fill();
      infotree->Write();
    }
  }
} final_output;

int CheckArgs(const SpecialTMELoopArgs& args) {
  if (args.muon_channels.empty()) {
    std::cout << "WARNING: No muon channels specified" << std::endl;
    return 1;
  }

  if (!args.veto_any_double_counts) {
    std::cout << "WARNING: not vetoing on any TMEs with any double counted events" << std::endl
	      << "SpecialTMELoop.C does not currently accurately handle double counted pulses" << std::endl
	      << "If you want to continue remove the return statement in this if branch and re-run" << std::endl;
    return 1;
  }

  return 0;
}

void Setup(const SpecialTMELoopArgs& args) {
  final_output.infotree = new TTree("infotree", "");

  ///////////////////////////////////////////
  // Put everything into the final output
  final_output.targetBlock = TargetBlockOutput(&args.target);

  ///////////////////////////////////////////////
  // Create the subtrees
  std::string tree_basename = "targetBlockTree";
  std::string treename = tree_basename;
  final_output.targetBlock.targetBlockTree = new TTree(treename.c_str(), treename.c_str());
  final_output.targetBlock.targetBlockTree->Branch("run_id", &targetBlockTreeInfo.run_id);
  final_output.targetBlock.targetBlockTree->Branch("block_id", &targetBlockTreeInfo.block_id);
  final_output.targetBlock.targetBlockTree->Branch("tme_id", &targetBlockTreeInfo.tme_id);
    
  final_output.targetBlock.targetBlockTree->Branch("energy", &targetBlockTreeInfo.energy);
  final_output.targetBlock.targetBlockTree->Branch("time", &targetBlockTreeInfo.time);
  final_output.targetBlock.targetBlockTree->Branch("tpi_id", &targetBlockTreeInfo.tpi_id);
  final_output.targetBlock.targetBlockTree->Branch("tap_id", &targetBlockTreeInfo.tap_id);
  final_output.targetBlock.targetBlockTree->Branch("channel", &targetBlockTreeInfo.channel);
  final_output.targetBlock.targetBlockTree->Branch("calib_gain", &targetBlockTreeInfo.calib_gain);
  final_output.targetBlock.targetBlockTree->Branch("calib_offset", &targetBlockTreeInfo.calib_offset);
}

void SpecialTMELoop(const SpecialTMELoopArgs& args) {

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
    std::vector<SimplePulse>* target_pulses = *(final_output.targetBlock.targetBlockInfo->channel);
    int n_target_pulses = target_pulses->size();
    if (n_target_pulses < 2) {
      continue; // to the next TME
    }

    ++n_analysed_tmes;

    /////////////////////////////////
    // Fill TargetBlock tree

    // If we have hits in all channels
    bool muon_hit = false;
    for (int i_target_pulse = 0; i_target_pulse < n_target_pulses; ++i_target_pulse) {
      const SimplePulse& target_pulse = target_pulses->at(i_target_pulse);
      double pulse_energy = target_pulse.Amp * final_output.targetBlock.targetBlockInfo->calibGain + final_output.targetBlock.targetBlockInfo->calibOffset;
      double pulse_time = target_pulse.tTME;

      if (pulse_energy > args.min_muon_energy && pulse_energy < args.max_muon_energy
	  && pulse_time > args.min_muon_time && pulse_time < args.max_muon_time) {
	muon_hit = true;
	break;
      }
    }

    // Now fill the tree
    if (muon_hit) {
      for (int i_target_pulse = 0; i_target_pulse < n_target_pulses; ++i_target_pulse) {
	const SimplePulse& target_pulse = target_pulses->at(i_target_pulse);
	double pulse_energy = target_pulse.Amp * final_output.targetBlock.targetBlockInfo->calibGain + final_output.targetBlock.targetBlockInfo->calibOffset;
	double pulse_time = target_pulse.tTME;
      
	if (pulse_energy > args.min_muon_energy && pulse_energy < args.max_muon_energy
	    && pulse_time > args.min_muon_time && pulse_time < args.max_muon_time) {
	  continue; // only want to write out the non-muon hits
	}

	targetBlockTreeInfo.time = pulse_time;
	//	      targetBlockTreeInfo.energy = i_pulse_list->at(i_pulse).E;
	targetBlockTreeInfo.energy = pulse_energy;
	targetBlockTreeInfo.tpi_id = target_pulse.tpi_id;
	targetBlockTreeInfo.tap_id = target_pulse.tap_id;
	targetBlockTreeInfo.calib_gain = final_output.targetBlock.targetBlockInfo->calibGain;
	targetBlockTreeInfo.calib_offset = final_output.targetBlock.targetBlockInfo->calibOffset;
	final_output.targetBlock.targetBlockTree->Fill();
      }
    }
  } // end TME loop
  
  final_output.Write();
  outfile->Close();
}
