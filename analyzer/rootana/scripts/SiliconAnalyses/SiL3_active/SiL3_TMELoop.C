#include "scripts/SiliconAnalyses/Utils/TMELoop.C"

void SiL3_TMELoop() {
  TMELoopArgs args;
  args.infilename = "~/data/out/v13/SiL3.root";
  args.tmetreename = "TMETree/TMETree";
  args.outfilename = "~/data/results/SiL3_active/results.root";
  args.n_entries = 5000000;

  // Channel configuration
  args.muon_channels.push_back(&muSc);

  args.ge_lo_gain.name = "GeLoGain";
  args.ge_lo_gain.channel = &GeLoGain;
  args.ge_hi_gain.name = "GeHiGain";
  args.ge_hi_gain.channel = &GeHiGain;

  args.target_channels.push_back(&SiL3);
  
  
  // Event vetos
  args.veto_any_double_counts = true;
  args.veto_max_muon_channel_pulses = 1;
  args.max_muon_channel_pulses = 1;

  // Don't want the EvdE plot
  args.produceEvdEPlots = false;

  // Want to produce the EvstTME plots for the ge channels
  args.produceGeEvstTMEPlots = true;
  PlotParams ge_x_axis(-30000, 30000, 10); // time
  PlotParams ge_y_axis(0, 1500, 0.5); // energy
  args.params_GeEvstTME[0] = ge_x_axis;
  args.params_GeEvstTME[1] = ge_y_axis;

  TMELoop(args);
}
