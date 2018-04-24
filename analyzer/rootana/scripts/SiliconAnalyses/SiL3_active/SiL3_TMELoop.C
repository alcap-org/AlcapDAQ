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

  args.right_arm.name = "SiR";
  args.right_arm.layer1_channels.push_back(&empty);
  args.right_arm.layer2_channels.push_back(&empty);
  args.right_arm.layer3_channels.push_back(&empty);

  args.left_arm.name = "SiL";
  args.left_arm.layer1_channels.push_back(&empty);
  args.left_arm.layer2_channels.push_back(&empty);
  args.left_arm.layer3_channels.push_back(&empty);

  args.target.name = "Target";
  args.target.layer1_channels.push_back(&empty);
  args.target.layer2_channels.push_back(&SiL3);
  args.target.layer3_channels.push_back(&empty);
  
  
  // Event vetos
  args.veto_any_double_counts = true;
  args.veto_max_muon_channel_pulses = 1;
  args.max_muon_channel_pulses = 1;
  
  TMELoop(args);
}
