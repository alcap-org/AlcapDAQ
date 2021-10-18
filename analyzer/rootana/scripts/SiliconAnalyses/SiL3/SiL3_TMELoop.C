#include "../Utils/TMELoop.C"

void SiL3_TMELoop() {
  TMELoopArgs args;
  args.tmetreename = "TMETree/TMETree";
  //  args.infilename = "~/data/out/v13/SiL3.root";

  args.infilename = "~/data/out/v14/SiL3.root";
  args.outfilename = "~/data/results/SiL3/subtrees_geq0TgtPulse_newPP20us_1.root";
  //  args.reqd_run_id = 9040;
  //  args.outfilename = "~/data/results/SiL3/subtrees09040_geq0TgtPulse_newPP20us.root";

  //  args.infilename = "~/data/out/v5/SiL3_tmetree.root"; // without templates
  //  args.tmetreename = "TMETree/TMETree";
  //  args.outfilename = "~/data/results/SiL3_active_no-templates/subtrees_geq2TgtPulse.root";
  args.n_entries = -1;
  //  args.n_entries = 1000000;

  ///////////////////////////////////////////
  // Calibration Constants
  // from elog:138, with Am241 energy subtraction
  double SiL3_Gain = 4.36673;   double SiL3_Offset = 35.17;
  double SiT_1_Gain = 2.02316;  double SiT_1_Offset = -8.02;
  double SiT_2_Gain = 1.91805;  double SiT_2_Offset = 12.48;
  double SiT_3_Gain = 1.94223;  double SiT_3_Offset = -11.79;
  double SiT_4_Gain = 1.95524;  double SiT_4_Offset = 0.85;
  double SiR1_1_Gain = 1.77056; double SiR1_1_Offset = -23.15;
  double SiR1_2_Gain = 1.75883; double SiR1_2_Offset = -9.26;
  double SiR1_3_Gain = 1.79166; double SiR1_3_Offset = -36.47;
  double SiR1_4_Gain = 1.76022; double SiR1_4_Offset = -50.47;
  double SiR2_Gain = 4.2416;    double SiR2_Offset = 107.35;

  // from Ge calibration
  double GeLoGain_Gain = 0.396515; double GeLoGain_Offset = 0.261953;
  double GeHiGain_Gain = 0.184892; double GeHiGain_Offset = -0.896645;

  //////////////////////////
  // Channel configuration
  args.muon_channels.push_back(&muSc);

  args.ge_lo_gain.name = "GeLoGain";
  args.ge_lo_gain.channel = &GeLoGain;
  args.ge_lo_gain.calibGain = GeLoGain_Gain;
  args.ge_lo_gain.calibOffset = GeLoGain_Offset;
  args.ge_hi_gain.name = "GeHiGain";
  args.ge_hi_gain.channel = &GeHiGain;
  args.ge_hi_gain.calibGain = GeHiGain_Gain;
  args.ge_hi_gain.calibOffset = GeHiGain_Offset;

  args.sit.name = "muSc";
  args.sit.layer1_channels.push_back(&muSc);
  args.sit.layer1_calibGains.push_back(1.0);
  args.sit.layer1_calibOffsets.push_back(0.0);
  args.sit.layer2_channels.push_back(&empty);
  args.sit.layer2_calibGains.push_back(0);
  args.sit.layer2_calibOffsets.push_back(0);
  args.sit.layer3_channels.push_back(&empty);
  args.sit.layer3_calibGains.push_back(0);
  args.sit.layer3_calibOffsets.push_back(0);

  
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

  args.right_arm.layer3_channels.push_back(&empty);
  args.right_arm.layer3_calibGains.push_back(0);
  args.right_arm.layer3_calibOffsets.push_back(0);

  // SiT is at beam left
  args.left_arm.name = "SiT";
  args.left_arm.layer1_channels.push_back(&SiT_1);
  args.left_arm.layer1_calibGains.push_back(SiT_1_Gain);
  args.left_arm.layer1_calibOffsets.push_back(SiT_1_Offset);
  
  args.left_arm.layer1_channels.push_back(&SiT_2);
  args.left_arm.layer1_calibGains.push_back(SiT_2_Gain);
  args.left_arm.layer1_calibOffsets.push_back(SiT_2_Offset);

  args.left_arm.layer1_channels.push_back(&SiT_3);
  args.left_arm.layer1_calibGains.push_back(SiT_3_Gain);
  args.left_arm.layer1_calibOffsets.push_back(SiT_3_Offset);

  args.left_arm.layer1_channels.push_back(&SiT_4);
  args.left_arm.layer1_calibGains.push_back(SiT_4_Gain);
  args.left_arm.layer1_calibOffsets.push_back(SiT_4_Offset);

  args.left_arm.layer2_channels.push_back(&empty);
  args.left_arm.layer2_calibGains.push_back(0);
  args.left_arm.layer2_calibOffsets.push_back(0);

  args.left_arm.layer3_channels.push_back(&empty);
  args.left_arm.layer3_calibGains.push_back(0);
  args.left_arm.layer3_calibOffsets.push_back(0);


  args.target.name = "Target";
  //  args.target.layer1_channels.push_back(&muSc);
  //  args.target.layer1_calibGains.push_back(1.0);
  //  args.target.layer1_calibOffsets.push_back(0.0);
  args.target.layer1_channels.push_back(&empty);
  args.target.layer1_calibGains.push_back(0);
  args.target.layer1_calibOffsets.push_back(0);
  args.target.layer2_channels.push_back(&SiL3);
  args.target.layer2_calibGains.push_back(SiL3_Gain);
  args.target.layer2_calibOffsets.push_back(SiL3_Offset);
  args.target.layer3_channels.push_back(&empty);
  args.target.layer3_calibGains.push_back(0);
  args.target.layer3_calibOffsets.push_back(0);
  
  ///////////////////////////
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
  args.pp_window = 20000;

  args.req_tgt_muon = false;//true;
  args.tgt_layer = 2;
  args.min_tgt_pulses = 0;
  //  args.muScCutLo = 3000;
  //  args.muScCutHi = 3500;
  
  TMELoop(args);
}
