#include "scripts/SiliconAnalyses/Utils/SpecialTMELoop.C"

void SiL3_SpecialTMELoop() {
  SpecialTMELoopArgs args;
  args.infilename = "~/data/out/v13/SiL3.root";
  args.tmetreename = "TMETree/TMETree";
  args.outfilename = "~/data/results/SiL3/subtrees_special.root";

  //  args.infilename = "~/data/out/v5/SiL3_tmetree.root"; // without templates
  //  args.tmetreename = "TMETree/TMETree";
  //  args.outfilename = "~/data/results/SiL3_active_no-templates/subtrees.root";
  args.n_entries = -1;
  //  args.n_entries = 1000000;

  ///////////////////////////////////////////
  // Calibration Constants
  // from elog:138, with Am241 energy subtraction
  double SiL3_Gain = 4.36673;   double SiL3_Offset = 35.17;

  //////////////////////////
  // Channel configuration
  args.muon_channels.push_back(&muSc);

  args.target.name = "Target";
  args.target.channel = &SiL3;
  args.target.calibGain = SiL3_Gain;
  args.target.calibOffset = SiL3_Offset;
  
  ///////////////////////////
  // Event vetos
  args.veto_any_double_counts = true;
  args.veto_max_muon_channel_pulses = 1;
  args.max_muon_channel_pulses = 1;
  args.min_muon_energy = 3000;
  args.max_muon_energy = 6000;
  args.min_muon_time = -200;
  args.max_muon_time = 200;
  
  SpecialTMELoop(args);
}
