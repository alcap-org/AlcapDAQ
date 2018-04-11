#include "scripts/SiliconAnalyses/Utils/TMELoop.C"

void SiL3_TMELoop() {
  TMELoopArgs args;
  args.infilename = "~/data/out/v13/SiL3.root";
  args.tmetreename = "TMETree/TMETree";
  args.outfilename = "~/data/results/SiL3_active/subtrees.root";
  args.n_entries = -1;

  // Channel configuration
  args.muon_channels.push_back(&muSc);

  args.ge_lo_gain.name = "GeLoGain";
  args.ge_lo_gain.channel = &GeLoGain;
  args.ge_hi_gain.name = "GeHiGain";
  args.ge_hi_gain.channel = &GeHiGain;

  args.target.channels.push_back(&SiL3);
  
  
  // Event vetos
  args.veto_any_double_counts = true;
  args.veto_max_muon_channel_pulses = 1;
  args.max_muon_channel_pulses = 1;

  // Don't want the EvdE plot
  args.produceEvdETree = false;

  // Want to produce the EvstTME plots for the ge channels
  args.produceGeEvstTMETree = true;

  // This is an active target analysis
  args.active_target_analysis = true;
  
  TMELoop(args);
}
