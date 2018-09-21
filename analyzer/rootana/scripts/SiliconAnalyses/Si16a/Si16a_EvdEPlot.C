#include "scripts/SiliconAnalyses/Utils/EvdEPlot.C"

void Si16a_EvdEPlot(std::string infilename, std::string outfilename) {

  EvdEPlotArgs args;
  args.infilename = infilename;
  args.treename = "siBlockTree_SiR";
  args.outfilename = outfilename;
  
  args.outhisttag = "all_SiR";
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
  args.do_cut = false;
  EvdEPlot(args);

  args.outhisttag = "all_SiR_veto";
  args.third_layer_veto = true;
  EvdEPlot(args);

  args.outhisttag = "proton_SiR";
  args.third_layer_veto = false;
  args.do_cut = true;
  args.cutfilename = "~/data/results/Si16a_useful/cuts.root";
  args.cutname = "proton_band";
  EvdEPlot(args);

  args.outhisttag = "proton_SiR_veto";
  args.third_layer_veto = true;
  EvdEPlot(args);

  args.treename = "siBlockTree_Target";
  args.outhisttag = "all_Target";
  args.third_layer_veto = false;
  args.early_time_veto = false;
  args.do_cut = false;
  EvdEPlot(args);
}
