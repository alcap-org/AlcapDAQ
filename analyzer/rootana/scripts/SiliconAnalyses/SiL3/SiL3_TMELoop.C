#include "scripts/SiliconAnalyses/Utils/TMELoop.C"

void SiL3_TMELoop() {
  TMELoopArgs args;
  //  args.infilename = "~/data/out/v13/SiL3.root";
  //  args.tmetreename = "TMETree/TMETree";
  //  args.outfilename = "~/data/results/SiL3_active/subtrees.root";

  args.infilename = "~/data/out/v5/SiL3_tmetree.root"; // without templates
  args.tmetreename = "TMETree/TMETree";
  args.outfilename = "~/data/results/SiL3_active_no-templates/subtrees.root";
  args.n_entries = -1;

  ///////////////////////////////////////////
  // Calibration Constants
  // from elog:138, with Am241 energy subtraction
  double SiL3_Gain = 4.36673;   double SiL3_Offset = 35.17;

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

  args.sit.name = "SiT";
  args.sit.layer1_channels.push_back(&empty);
  args.sit.layer2_channels.push_back(&empty);
  args.sit.layer3_channels.push_back(&empty);

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
  args.veto_any_double_counts = true;
  args.veto_max_muon_channel_pulses = 1;
  args.max_muon_channel_pulses = 1;
  
  TMELoop(args);
}
