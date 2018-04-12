#include "scripts/SiliconAnalyses/Utils/EvdEPlot.C"

void Si16b_EvdEPlot(std::string infilename, std::string outfilename) {

  EvdEPlotArgs args;
  args.infilename = infilename;
  args.treename = "armtree_SiR";
  args.outfilename = outfilename;
  
  args.outhistname = "hEvdE_all_SiR";
  args.min_x_energy = 0;
  args.max_x_energy = 20000;
  args.x_energy_width = 10;
  args.min_y_energy = 0;
  args.max_y_energy = 10000;
  args.y_energy_width = 10;
  args.layer_coincidence_veto = true;
  args.layer_coincidence_time = 200;
  args.third_layer_veto = false;
  args.early_time_veto = true;
  args.early_time_cut = 200;
  args.do_proton_cut = false;
  EvdEPlot(args);

  args.outhistname = "hEvdE_all_SiR_veto";
  args.third_layer_veto = true;
  EvdEPlot(args);

  args.outhistname = "hEvdE_proton_SiR";
  args.third_layer_veto = false;
  args.do_proton_cut = true;
  double x_1 = 0, y_1 = 2250, x_2 = 5500, y_2 = 0; // parameters for the diagonal electron spot cut
  double electron_spot_gradient = (y_2 - y_1) / (x_2 - x_1);
  double electron_spot_yoffset = y_1;
  args.electron_spot_cut = new TF1("proton_cut_electron_spot", "[0]*x + [1]", args.min_x_energy, args.max_x_energy);
  args.electron_spot_cut->SetParameters(electron_spot_gradient, electron_spot_yoffset);
  double punchthrough_cut = 300; // parameters for the horizontal punch through proton cut
  args.punchthrough_cut = new TF1("proton_cut_punchthrough", "[0]", args.min_x_energy, args.max_x_energy);
  args.punchthrough_cut->SetParameter(0, punchthrough_cut);
  double deuteron_cut_peak = 4500; // parameters for the exponential remove deuteron cut
  double deuteron_cut_slope = -0.0004;
  double deuteron_cut_yoffset = 500;
  args.deuteron_removal_cut = new TF1("proton_cut_deuteron_removal", "[0]*TMath::Exp([1]*x) + [2]", args.min_x_energy, args.max_x_energy);
  args.deuteron_removal_cut->SetParameters(deuteron_cut_peak, deuteron_cut_slope, deuteron_cut_yoffset);
  EvdEPlot(args);

  args.outhistname = "hEvdE_proton_SiR_veto";
  args.third_layer_veto = true;
  EvdEPlot(args);
}
