#include "scripts/SiliconAnalyses/Utils/TMELoop.C"

#include <sstream>

void Si16a_TMELoop(int i_run) {
  std::stringstream run_str;
  run_str << "0" << i_run;
  TMELoopArgs args;
  args.infilename = "~/data/out/v10/out" + run_str.str() + ".root";
  args.tmetreename = "TMETree/TMETree";
  args.outfilename = "~/data/results/Si16a_useful/subtrees" + run_str.str() + ".root";
  args.n_entries = -1; // run for all TMEs

  // Channel configuration
  args.muon_channels.push_back(&SiT_1);
  args.muon_channels.push_back(&SiT_2);
  args.muon_channels.push_back(&SiT_3);
  args.muon_channels.push_back(&SiT_4);

  args.right_arm.name = "SiR";
  args.right_arm.layer1_channels.push_back(&SiR1_1);
  args.right_arm.layer1_channels.push_back(&SiR1_2);
  args.right_arm.layer1_channels.push_back(&SiR1_3);
  args.right_arm.layer1_channels.push_back(&SiR1_4);
  args.right_arm.layer2_channels.push_back(&SiR2);
  args.right_arm.layer3_channels.push_back(&SiR3);

  args.left_arm.name = "SiL";
  args.left_arm.layer1_channels.push_back(&empty);
  args.left_arm.layer2_channels.push_back(&empty);
  args.left_arm.layer3_channels.push_back(&empty);

  args.ge_lo_gain.name = "GeLoGain";
  args.ge_lo_gain.channel = &GeLoGain;
  args.ge_hi_gain.name = "GeHiGain";
  args.ge_hi_gain.channel = &GeHiGain;

  args.target.name = "Target";
  args.target.layer1_channels.push_back(&SiL1_2);
  args.target.layer1_channels.push_back(&SiL1_3);
  args.target.layer1_channels.push_back(&SiL1_4);
  args.target.layer1_channels.push_back(&SiL1_5);
  args.target.layer1_channels.push_back(&SiL1_6);
  args.target.layer1_channels.push_back(&SiL1_7);
  args.target.layer1_channels.push_back(&SiL1_8);
  args.target.layer1_channels.push_back(&SiL1_9);
  args.target.layer1_channels.push_back(&SiL1_10);
  args.target.layer1_channels.push_back(&SiL1_11);
  args.target.layer1_channels.push_back(&SiL1_12);
  args.target.layer1_channels.push_back(&SiL1_13);
  args.target.layer1_channels.push_back(&SiL1_14);
  args.target.layer1_channels.push_back(&SiL1_15);
  args.target.layer2_channels.push_back(&SiL3);
  args.target.layer3_channels.push_back(&empty);

  // Event vetos
  args.veto_any_double_counts = true;
  args.veto_max_muon_channel_pulses = 1;
  args.max_muon_channel_pulses = 1;

  
  TMELoop(args);
}
