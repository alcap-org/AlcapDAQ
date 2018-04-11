#include "scripts/SiliconAnalyses/Utils/TMELoop.C"

void Si16b_TMELoop() {
  TMELoopArgs args;
  args.infilename = "~/data/out/v10/Si16b.root";
  args.tmetreename = "TMETree/TMETree";
  args.outfilename = "~/data/results/Si16b_passive/results.root";
  args.n_entries = 5000000;

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
  args.left_arm.layer1_channels.clear();
  args.left_arm.layer2_channels.push_back(&SiL3);
  args.left_arm.layer3_channels.clear();

  args.ge_lo_gain.name = "GeLoGain";
  args.ge_lo_gain.channel = &GeLoGain;
  args.ge_hi_gain.name = "GeHiGain";
  args.ge_hi_gain.channel = &GeHiGain;

  args.target.channels.push_back(&SiL1_2);
  args.target.channels.push_back(&SiL1_3);
  args.target.channels.push_back(&SiL1_4);
  args.target.channels.push_back(&SiL1_5);
  args.target.channels.push_back(&SiL1_6);
  args.target.channels.push_back(&SiL1_7);
  args.target.channels.push_back(&SiL1_8);
  args.target.channels.push_back(&SiL1_9);
  args.target.channels.push_back(&SiL1_10);
  args.target.channels.push_back(&SiL1_11);
  args.target.channels.push_back(&SiL1_12);
  args.target.channels.push_back(&SiL1_13);
  args.target.channels.push_back(&SiL1_14);
  args.target.channels.push_back(&SiL1_15);

  
  // Event vetos
  args.veto_any_double_counts = true;
  args.veto_max_muon_channel_pulses = 1;
  args.max_muon_channel_pulses = 1;

  // Want the EvdE plot
  args.produceEvdEPlots = true;
  PlotParams x_axis(0, 20000, 10);
  PlotParams y_axis(0, 10000, 10);
  args.params_EvdE[0] = x_axis;
  args.params_EvdE[1] = y_axis;
  args.evde_layer_coincidence_time = 200; // coincidence time between layers
  args.evde_time_cut = 200; // cut out protons from early times

  args.extractProtons = true;
  double x_1 = 0, y_1 = 2250, x_2 = 5500, y_2 = 0; // parameters for the diagonal electron spot cut
  double electron_spot_gradient = (y_2 - y_1) / (x_2 - x_1);
  double electron_spot_yoffset = y_1;
  args.proton_cuts.electron_spot = new TF1("proton_cut_electron_spot", "[0]*x + [1]", x_axis.min, x_axis.max);
  args.proton_cuts.electron_spot->SetParameters(electron_spot_gradient, electron_spot_yoffset);
  double punchthrough_cut = 300; // parameters for the horizontal punch through proton cut
  args.proton_cuts.punchthrough = new TF1("proton_cut_punchthrough", "[0]", x_axis.min, x_axis.max);
  args.proton_cuts.punchthrough->SetParameter(0, punchthrough_cut);
  double deuteron_cut_peak = 4500; // parameters for the exponential remove deuteron cut
  double deuteron_cut_slope = -0.0004;
  double deuteron_cut_yoffset = 500;
  args.proton_cuts.deuteron_removal = new TF1("proton_cut_deuteron_removal", "[0]*TMath::Exp([1]*x) + [2]", x_axis.min, x_axis.max);
  args.proton_cuts.deuteron_removal->SetParameters(deuteron_cut_peak, deuteron_cut_slope, deuteron_cut_yoffset);


  // Want to produce the EvstTME plots for the ge channels
  args.produceGeEvstTMEPlots = true;
  PlotParams ge_x_axis(-30000, 30000, 10); // time
  PlotParams ge_y_axis(0, 1500, 0.5); // energy
  args.params_GeEvstTME[0] = ge_x_axis;
  args.params_GeEvstTME[1] = ge_y_axis;

  // Not currently an active target analysis
  args.active_target_analysis = false;
  
  TMELoop(args);
}
