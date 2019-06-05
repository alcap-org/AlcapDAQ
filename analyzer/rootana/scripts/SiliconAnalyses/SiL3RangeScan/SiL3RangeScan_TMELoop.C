#include "scripts/SiliconAnalyses/Utils/TMELoop.C"

#include <sstream>

void SiL3RangeScan_TMELoop() {
  const int n_runs = 11;
  int run_numbers[n_runs] = {8986, 8987, 8988, 8989, 8990, 8991, 8992, 8993, 8994, 8995, 8996};

  for (int i_run = 0; i_run < n_runs; ++i_run) {
    std::stringstream run_str;
    run_str << "0" << run_numbers[i_run];

    std::cout << "Run #" << run_str.str() << ": " << std::endl;
    
    TMELoopArgs args;
    args.infilename = "~/data/out/local/out" + run_str.str() + "_tme-tree_no-templates.root";
    args.tmetreename = "TMETree/TMETree";
    args.outfilename = "~/data/results/SiL3RangeScan/subtrees" + run_str.str() + ".root";
    args.n_entries = -1; // run for all TMEs
    
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

    // Event vetos
    args.veto_any_double_counts = true;
    args.veto_max_muon_channel_pulses = 1;
    args.max_muon_channel_pulses = 1;

  
    TMELoop(args);
  }
}
