#include "scripts/SiliconAnalyses/Utils/EvdEPlot.C"

void Si16b_EvdEPlot(std::string infilename, std::string outfilename) {

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

  args.treename = "siBlockTree_SiR";
  args.outfilename = outfilename;

  args.outdirname = "all_SiR";
  args.layer_coincidence_veto = true;
  args.layer_coincidence_time = 200;
  args.early_time_veto = false;
  args.do_cut = false;
  EvdEPlot(args);
  
  args.outdirname = "all_SiR_timecut";
  args.early_time_veto = true;
  args.early_time_cut = 200;
  EvdEPlot(args);

  args.outdirname = "proton_SiR";
  args.early_time_veto = false;
  args.do_cut = true;
  args.cutfilename = "~/data/results/Si16b_passive/cuts_new_threelayer.root";
  args.cutname = "proton_cut";
  EvdEPlot(args);

  args.outdirname = "proton_SiR_timecut";
  args.early_time_veto = true;
  args.early_time_cut = 200;
  EvdEPlot(args);

  args.outdirname = "deuteron_SiR";
  args.early_time_veto = false;
  args.do_cut = true;
  args.cutfilename = "~/data/results/Si16b_passive/cuts_new_threelayer.root";
  args.cutname = "deuteron_cut";
  EvdEPlot(args);

  args.outdirname = "deuteron_SiR_timecut";
  args.early_time_veto = true;
  args.early_time_cut = 200;
  EvdEPlot(args);

  args.outdirname = "triton_SiR";
  args.early_time_veto = false;
  args.do_cut = true;
  args.cutfilename = "~/data/results/Si16b_passive/cuts_new_threelayer.root";
  args.cutname = "triton_cut";
  EvdEPlot(args);

  args.outdirname = "triton_SiR_timecut";
  args.early_time_veto = true;
  args.early_time_cut = 200;
  EvdEPlot(args);

  args.outdirname = "alpha_SiR";
  args.early_time_veto = false;
  args.do_cut = true;
  args.cutfilename = "~/data/results/Si16b_passive/cuts_new_threelayer.root";
  args.cutname = "alpha_cut";
  EvdEPlot(args);

  args.outdirname = "alpha_SiR_timecut";
  args.early_time_veto = true;
  args.early_time_cut = 200;
  EvdEPlot(args);
}
