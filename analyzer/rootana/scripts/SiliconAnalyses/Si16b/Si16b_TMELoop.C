#include "../Utils/TMELoop.C"

void Si16b_TMELoop() {
  TMELoopArgs args;
  args.infilename = "~/data/out/v10/Si16b.root";
  args.tmetreename = "TMETree/TMETree";
  args.outfilename = "~/data/results/Si16b/subtrees_newPP_geq1TgtPulse_1.root";
  args.n_entries = -1; // run for all TMEs
  //  args.n_entries = 1000000;

  ///////////////////////////////////////////
  // Calibration Constants
  // from elog:138, with Am241 energy subtraction
  double SiR1_1_Gain = 1.77056; double SiR1_1_Offset = -23.15;
  double SiR1_2_Gain = 1.75883; double SiR1_2_Offset = -9.26;
  double SiR1_3_Gain = 1.79166; double SiR1_3_Offset = -36.47;
  double SiR1_4_Gain = 1.76022; double SiR1_4_Offset = -50.47;
  double SiR2_Gain = 4.2416;    double SiR2_Offset = 107.35;
  double SiR3_Gain = 5.75035;   double SiR3_Offset = 58.52;
  double SiL3_Gain = 4.36673;   double SiL3_Offset = 35.17;
  double SiT_1_Gain = 2.02316;  double SiT_1_Offset = -8.02;
  double SiT_2_Gain = 1.91805;  double SiT_2_Offset = 12.48;
  double SiT_3_Gain = 1.94223;  double SiT_3_Offset = -11.79;
  double SiT_4_Gain = 1.95524;  double SiT_4_Offset = 0.85;
  
  // from Mark's calibration with Am241 energy subtraction
  double SiL1_Gain = 2.056969;  double SiL1_Offset = 1.370678;

  // from Ge calibration
  double GeLoGain_Gain = 0.399114; double GeLoGain_Offset = 0.229208;
  double GeHiGain_Gain = 0.185084; double GeHiGain_Offset = 0.114321;

  
  //////////////////////////
  // Channel configurations
  args.muon_channels.push_back(&SiT_1);
  args.muon_channels.push_back(&SiT_2);
  args.muon_channels.push_back(&SiT_3);
  args.muon_channels.push_back(&SiT_4);

  // Right Arm - Layer 1
  args.right_arm.name = "SiR";
  args.right_arm.layer1_channels.push_back(&SiR1_1);
  args.right_arm.layer1_calibGains.push_back(SiR1_1_Gain);
  args.right_arm.layer1_calibOffsets.push_back(SiR1_1_Offset);

  args.right_arm.layer1_channels.push_back(&SiR1_2);
  args.right_arm.layer1_calibGains.push_back(SiR1_2_Gain);
  args.right_arm.layer1_calibOffsets.push_back(SiR1_2_Offset);

  args.right_arm.layer1_channels.push_back(&SiR1_3);
  args.right_arm.layer1_calibGains.push_back(SiR1_3_Gain);
  args.right_arm.layer1_calibOffsets.push_back(SiR1_3_Offset);

  args.right_arm.layer1_channels.push_back(&SiR1_4);
  args.right_arm.layer1_calibGains.push_back(SiR1_4_Gain);
  args.right_arm.layer1_calibOffsets.push_back(SiR1_4_Offset);

  // Right Arm - Layer 2
  args.right_arm.layer2_channels.push_back(&SiR2);
  args.right_arm.layer2_calibGains.push_back(SiR2_Gain);
  args.right_arm.layer2_calibOffsets.push_back(SiR2_Offset);

  // Right Arm - Layer 3
  args.right_arm.layer3_channels.push_back(&SiR3);
  args.right_arm.layer3_calibGains.push_back(SiR3_Gain);
  args.right_arm.layer3_calibOffsets.push_back(SiR3_Offset);

  // Left Arm - Layer 1
  args.left_arm.name = "SiL";
  /*  args.left_arm.layer1_channels.push_back(&SiL1_2);
  args.left_arm.layer1_calibGains.push_back(SiL1_Gain);
  args.left_arm.layer1_calibOffsets.push_back(SiL1_Offset);

  args.left_arm.layer1_channels.push_back(&SiL1_3);
  args.left_arm.layer1_calibGains.push_back(SiL1_Gain);
  args.left_arm.layer1_calibOffsets.push_back(SiL1_Offset);

  args.left_arm.layer1_channels.push_back(&SiL1_4);
  args.left_arm.layer1_calibGains.push_back(SiL1_Gain);
  args.left_arm.layer1_calibOffsets.push_back(SiL1_Offset);

  args.left_arm.layer1_channels.push_back(&SiL1_5);
  args.left_arm.layer1_calibGains.push_back(SiL1_Gain);
  args.left_arm.layer1_calibOffsets.push_back(SiL1_Offset);

  args.left_arm.layer1_channels.push_back(&SiL1_6);
  args.left_arm.layer1_calibGains.push_back(SiL1_Gain);
  args.left_arm.layer1_calibOffsets.push_back(SiL1_Offset);

  args.left_arm.layer1_channels.push_back(&SiL1_7);
  args.left_arm.layer1_calibGains.push_back(SiL1_Gain);
  args.left_arm.layer1_calibOffsets.push_back(SiL1_Offset);

  args.left_arm.layer1_channels.push_back(&SiL1_8);
  args.left_arm.layer1_calibGains.push_back(SiL1_Gain);
  args.left_arm.layer1_calibOffsets.push_back(SiL1_Offset);

  args.left_arm.layer1_channels.push_back(&SiL1_9);
  args.left_arm.layer1_calibGains.push_back(SiL1_Gain);
  args.left_arm.layer1_calibOffsets.push_back(SiL1_Offset);

  args.left_arm.layer1_channels.push_back(&SiL1_10);
  args.left_arm.layer1_calibGains.push_back(SiL1_Gain);
  args.left_arm.layer1_calibOffsets.push_back(SiL1_Offset);

  args.left_arm.layer1_channels.push_back(&SiL1_11);
  args.left_arm.layer1_calibGains.push_back(SiL1_Gain);
  args.left_arm.layer1_calibOffsets.push_back(SiL1_Offset);

  args.left_arm.layer1_channels.push_back(&SiL1_12);
  args.left_arm.layer1_calibGains.push_back(SiL1_Gain);
  args.left_arm.layer1_calibOffsets.push_back(SiL1_Offset);

  args.left_arm.layer1_channels.push_back(&SiL1_13);
  args.left_arm.layer1_calibGains.push_back(SiL1_Gain);
  args.left_arm.layer1_calibOffsets.push_back(SiL1_Offset);

  args.left_arm.layer1_channels.push_back(&SiL1_14);
  args.left_arm.layer1_calibGains.push_back(SiL1_Gain);
  args.left_arm.layer1_calibOffsets.push_back(SiL1_Offset);

  args.left_arm.layer1_channels.push_back(&SiL1_15);
  args.left_arm.layer1_calibGains.push_back(SiL1_Gain);
  args.left_arm.layer1_calibOffsets.push_back(SiL1_Offset);
  */
  args.left_arm.layer1_channels.push_back(&empty);
  args.left_arm.layer1_calibGains.push_back(0);
  args.left_arm.layer1_calibOffsets.push_back(0);
  // Left Arm - Layer 2
  args.left_arm.layer2_channels.push_back(&SiL3);
  args.left_arm.layer2_calibGains.push_back(SiL3_Gain);
  args.left_arm.layer2_calibOffsets.push_back(SiL3_Offset);
  // Left Arm - Layer 3
  args.left_arm.layer3_channels.push_back(&empty);
  args.left_arm.layer3_calibGains.push_back(0);
  args.left_arm.layer3_calibOffsets.push_back(0);

  // Germanium
  args.ge_lo_gain.name = "GeLoGain";
  args.ge_lo_gain.channel = &GeLoGain;
  args.ge_lo_gain.calibGain = GeLoGain_Gain;
  args.ge_lo_gain.calibOffset = GeLoGain_Offset;
  args.ge_hi_gain.name = "GeHiGain";
  args.ge_hi_gain.channel = &GeHiGain;
  args.ge_hi_gain.calibGain = GeHiGain_Gain;
  args.ge_hi_gain.calibOffset = GeHiGain_Offset;

  // Target - Layer 1
  args.target.name = "Target";
  
  args.target.layer1_channels.push_back(&SiL1_2);
  args.target.layer1_calibGains.push_back(SiL1_Gain);
  args.target.layer1_calibOffsets.push_back(SiL1_Offset);

  args.target.layer1_channels.push_back(&SiL1_3);
  args.target.layer1_calibGains.push_back(SiL1_Gain);
  args.target.layer1_calibOffsets.push_back(SiL1_Offset);

  args.target.layer1_channels.push_back(&SiL1_4);
  args.target.layer1_calibGains.push_back(SiL1_Gain);
  args.target.layer1_calibOffsets.push_back(SiL1_Offset);

  args.target.layer1_channels.push_back(&SiL1_5);
  args.target.layer1_calibGains.push_back(SiL1_Gain);
  args.target.layer1_calibOffsets.push_back(SiL1_Offset);

  args.target.layer1_channels.push_back(&SiL1_6);
  args.target.layer1_calibGains.push_back(SiL1_Gain);
  args.target.layer1_calibOffsets.push_back(SiL1_Offset);
  
  args.target.layer1_channels.push_back(&SiL1_7);
  args.target.layer1_calibGains.push_back(SiL1_Gain);
  args.target.layer1_calibOffsets.push_back(SiL1_Offset);
  
  args.target.layer1_channels.push_back(&SiL1_8);
  args.target.layer1_calibGains.push_back(SiL1_Gain);
  args.target.layer1_calibOffsets.push_back(SiL1_Offset);
  
  args.target.layer1_channels.push_back(&SiL1_9);
  args.target.layer1_calibGains.push_back(SiL1_Gain);
  args.target.layer1_calibOffsets.push_back(SiL1_Offset);

  args.target.layer1_channels.push_back(&SiL1_10);
  args.target.layer1_calibGains.push_back(SiL1_Gain);
  args.target.layer1_calibOffsets.push_back(SiL1_Offset);
  
  args.target.layer1_channels.push_back(&SiL1_11);
  args.target.layer1_calibGains.push_back(SiL1_Gain);
  args.target.layer1_calibOffsets.push_back(SiL1_Offset);

  args.target.layer1_channels.push_back(&SiL1_12);
  args.target.layer1_calibGains.push_back(SiL1_Gain);
  args.target.layer1_calibOffsets.push_back(SiL1_Offset);

  args.target.layer1_channels.push_back(&SiL1_13);
  args.target.layer1_calibGains.push_back(SiL1_Gain);
  args.target.layer1_calibOffsets.push_back(SiL1_Offset);

  args.target.layer1_channels.push_back(&SiL1_14);
  args.target.layer1_calibGains.push_back(SiL1_Gain);
  args.target.layer1_calibOffsets.push_back(SiL1_Offset);
  
  args.target.layer1_channels.push_back(&SiL1_15);
  args.target.layer1_calibGains.push_back(SiL1_Gain);
  args.target.layer1_calibOffsets.push_back(SiL1_Offset);
  
  // Target - Layer 2
  args.target.layer2_channels.push_back(&SiR1_1);
  args.target.layer2_calibGains.push_back(SiR1_1_Gain);
  args.target.layer2_calibOffsets.push_back(SiR1_1_Offset);

  args.target.layer2_channels.push_back(&SiR1_2);
  args.target.layer2_calibGains.push_back(SiR1_2_Gain);
  args.target.layer2_calibOffsets.push_back(SiR1_2_Offset);

  args.target.layer2_channels.push_back(&SiR1_3);
  args.target.layer2_calibGains.push_back(SiR1_3_Gain);
  args.target.layer2_calibOffsets.push_back(SiR1_3_Offset);

  args.target.layer2_channels.push_back(&SiR1_4);
  args.target.layer2_calibGains.push_back(SiR1_4_Gain);
  args.target.layer2_calibOffsets.push_back(SiR1_4_Offset);

  // Target - Layer 3
  args.target.layer3_channels.push_back(&SiR2);
  args.target.layer3_calibGains.push_back(SiR2_Gain);
  args.target.layer3_calibOffsets.push_back(SiR2_Offset);

  // SiT - Layer 1
  args.sit.name = "SiT";
  args.sit.layer1_channels.push_back(&SiT_1);
  args.sit.layer1_calibGains.push_back(SiT_1_Gain);
  args.sit.layer1_calibOffsets.push_back(SiT_1_Offset);
  
  args.sit.layer1_channels.push_back(&SiT_2);
  args.sit.layer1_calibGains.push_back(SiT_2_Gain);
  args.sit.layer1_calibOffsets.push_back(SiT_2_Offset);

  args.sit.layer1_channels.push_back(&SiT_3);
  args.sit.layer1_calibGains.push_back(SiT_3_Gain);
  args.sit.layer1_calibOffsets.push_back(SiT_3_Offset);

  args.sit.layer1_channels.push_back(&SiT_4);
  args.sit.layer1_calibGains.push_back(SiT_4_Gain);
  args.sit.layer1_calibOffsets.push_back(SiT_4_Offset);

  // SiT - Layer 2
  args.sit.layer2_channels.push_back(&empty);
  args.sit.layer2_calibGains.push_back(0);
  args.sit.layer2_calibOffsets.push_back(0);

  // SiT - Layer 3
  args.sit.layer3_channels.push_back(&empty);
  args.sit.layer3_calibGains.push_back(0);
  args.sit.layer3_calibOffsets.push_back(0);
    
  // Event vetos
  // old PP definition
  /*  args.veto_any_double_counts = true;
  args.veto_max_muon_channel_pulses = true;
  args.max_muon_channel_pulses = 1;
  args.veto_pp_window = false;
  */
  // new PP definition
  args.reqd_run_id = -1;
  args.req_tgt_muon = false;
  args.veto_any_double_counts = false;
  args.veto_max_muon_channel_pulses = false;
  args.veto_pp_window = true;
  args.pp_window = 10000;
  //  args.pp_window = 20000;

  // require a hit in the target
  args.req_tgt_muon = true;
  args.tgt_layer = 1;
  args.min_tgt_pulses = 1;
  
  TMELoop(args);
}
