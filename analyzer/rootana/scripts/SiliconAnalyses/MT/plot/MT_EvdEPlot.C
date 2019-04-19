#include "scripts/SiliconAnalyses/Utils/EvdEPlot.C"

void MT_EvdEPlot(std::string infilename, std::string outfilename) {

  EvdEPlotArgs args;
  args.infilename = infilename;

  args.min_x_energy = 0;
  args.max_x_energy = 25000;
  args.x_energy_width = 10;
  args.min_y_energy = 0;
  args.max_y_energy = 20000;
  args.y_energy_width = 10;

  args.min_time = -30000;
  args.max_time = 30000;
  args.time_width = 10;

  args.outfilename = outfilename;  
  args.thin_layer = "thin";
  args.thick_layer = "thick";
  args.third_layer = "third";
  args.veto_layer = "";
  
  const int n_arms = 2;
  std::string arms[n_arms] = {"SiR", "SiL"};
  
  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {

    std::string this_arm = arms[i_arm];
    args.treename = "siBlockTree_" + this_arm;

    args.outdirname = "all_" + this_arm;
    args.layer_coincidence_vetos = true;
    args.layer12_coincidence_time = 200;
    args.layer23_coincidence_time = 200;
    args.early_time_veto = false;
    args.do_cut = false;
    args.do_psel = false;
    EvdEPlot(args);

    args.outdirname = "all_" + this_arm + "_timecut";
    args.early_time_veto = true;
    args.early_time_cut = 200;
    EvdEPlot(args);

    args.outdirname = "proton_" + this_arm;
    args.early_time_veto = false;
    args.do_cut = true;
    args.cutfilename = "~/data/results/Si16b/cuts_new_threelayer.root";
    args.cutname = "proton_cut";
    EvdEPlot(args);

    args.outdirname = "proton_" + this_arm + "_timecut";
    args.early_time_veto = true;
    args.early_time_cut = 200;
    EvdEPlot(args);

    args.outdirname = "deuteron_" + this_arm;
    args.early_time_veto = false;
    args.do_cut = true;
    args.cutfilename = "~/data/results/Si16b/cuts_new_threelayer.root";
    args.cutname = "deuteron_cut";
    EvdEPlot(args);

    args.outdirname = "deuteron_" + this_arm + "_timecut";
    args.early_time_veto = true;
    args.early_time_cut = 200;
    EvdEPlot(args);

    args.outdirname = "triton_" + this_arm;
    args.early_time_veto = false;
    args.do_cut = true;
    args.cutfilename = "~/data/results/Si16b/cuts_new_threelayer.root";
    args.cutname = "triton_cut";
    EvdEPlot(args);

    args.outdirname = "triton_" + this_arm + "_timecut";
    args.early_time_veto = true;
    args.early_time_cut = 200;
    EvdEPlot(args);

    args.outdirname = "alpha_" + this_arm;
    args.early_time_veto = false;
    args.do_cut = true;
    args.cutfilename = "~/data/results/Si16b/cuts_new_threelayer.root";
    args.cutname = "alpha_cut";
    EvdEPlot(args);

    args.outdirname = "alpha_" + this_arm + "_timecut";
    args.early_time_veto = true;
    args.early_time_cut = 200;
    EvdEPlot(args);

  }    
}
