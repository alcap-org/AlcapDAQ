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
  args.thin_layer = "thin";
  args.thick_layer = "thick";
  args.third_layer = "third";
  args.veto_layer = "";

  args.outdirname = "all_SiR";
  args.layer_coincidence_vetos = true;
  args.layer12_coincidence_time = 200;
  args.layer23_coincidence_time = 200;
  args.early_time_veto = false;
  args.do_cut = false;
  args.do_psel = false;
  EvdEPlot(args);
  
  args.outdirname = "all_SiR_timecut";
  args.early_time_veto = true;
  args.early_time_cut = 200;
  EvdEPlot(args);

  args.outdirname = "proton_SiR";
  args.early_time_veto = false;
  args.do_cut = true;
  args.cutfilename = "~/data/results/Si16b/cuts_new_threelayer.root";
  args.cutname = "proton_cut";
  EvdEPlot(args);

  args.outdirname = "proton_SiR_timecut";
  args.early_time_veto = true;
  args.early_time_cut = 200;
  EvdEPlot(args);

  args.outdirname = "proton_SiR_timecut400ns";
  args.early_time_veto = true;
  args.early_time_cut = 400;
  EvdEPlot(args);

  args.outdirname = "proton_SiR_timecut600ns";
  args.early_time_veto = true;
  args.early_time_cut = 600;
  EvdEPlot(args);

  args.outdirname = "proton_SiR_timecut800ns";
  args.early_time_veto = true;
  args.early_time_cut = 800;
  EvdEPlot(args);

  args.outdirname = "proton_SiR_timecut1000ns";
  args.early_time_veto = true;
  args.early_time_cut = 1000;
  EvdEPlot(args);

  args.outdirname = "proton_SiR_timecut2000ns";
  args.early_time_veto = true;
  args.early_time_cut = 2000;
  EvdEPlot(args);

  args.outdirname = "deuteron_SiR";
  args.early_time_veto = false;
  args.do_cut = true;
  args.cutfilename = "~/data/results/Si16b/cuts_new_threelayer.root";
  args.cutname = "deuteron_cut";
  EvdEPlot(args);

  args.outdirname = "deuteron_SiR_timecut";
  args.early_time_veto = true;
  args.early_time_cut = 200;
  EvdEPlot(args);

  args.outdirname = "triton_SiR";
  args.early_time_veto = false;
  args.do_cut = true;
  args.cutfilename = "~/data/results/Si16b/cuts_new_threelayer.root";
  args.cutname = "triton_cut";
  EvdEPlot(args);

  args.outdirname = "triton_SiR_timecut";
  args.early_time_veto = true;
  args.early_time_cut = 200;
  EvdEPlot(args);

  args.outdirname = "alpha_SiR";
  args.early_time_veto = false;
  args.do_cut = true;
  args.cutfilename = "~/data/results/Si16b/cuts_new_threelayer.root";
  args.cutname = "alpha_cut";
  EvdEPlot(args);

  args.outdirname = "alpha_SiR_timecut";
  args.early_time_veto = true;
  args.early_time_cut = 200;
  EvdEPlot(args);

  args.do_cut = false;
  args.do_psel = true;
  args.psel_particle = proton;
  args.outdirname = "proton_SiR_timecut_psel";
  EvdEPlot(args);

  args.psel_particle = deuteron;
  args.outdirname = "deuteron_SiR_timecut_psel";
  EvdEPlot(args);

  args.psel_particle = triton;
  args.outdirname = "triton_SiR_timecut_psel";
  EvdEPlot(args);

  args.psel_particle = alpha;
  args.outdirname = "alpha_SiR_timecut_psel";
  EvdEPlot(args);


  args.do_psel = false;
  args.treename = "siBlockTree_Target";
  args.thin_layer = "thick";
  args.thick_layer = "third";
  args.third_layer = "thin";
  args.veto_layer = "";
  args.layer_coincidence_vetos = true;
  args.layer12_coincidence_time = 200;
  args.layer23_coincidence_time = 50000;

  args.outdirname = "all_SiR_wSiL1Coinc";
  args.early_time_veto = false;
  EvdEPlot(args);

  args.outdirname = "all_SiR_timecut_wSiL1Coinc";
  args.early_time_veto = true;
  args.early_time_cut = 200;
  EvdEPlot(args);

  args.outdirname = "proton_SiR_wSiL1Coinc";
  args.early_time_veto = false;
  args.do_cut = true;
  args.cutfilename = "~/data/results/Si16b/cuts_new_threelayer.root";
  args.cutname = "proton_cut";
  EvdEPlot(args);

  args.outdirname = "proton_SiR_timecut_wSiL1Coinc";
  args.early_time_veto = true;
  args.early_time_cut = 200;
  EvdEPlot(args);

  args.outdirname = "proton_SiR_timecut400ns_wSiL1Coinc";
  args.early_time_veto = true;
  args.early_time_cut = 400;
  EvdEPlot(args);

  args.outdirname = "proton_SiR_timecut600ns_wSiL1Coinc";
  args.early_time_veto = true;
  args.early_time_cut = 600;
  EvdEPlot(args);

  args.outdirname = "proton_SiR_timecut800ns_wSiL1Coinc";
  args.early_time_veto = true;
  args.early_time_cut = 800;
  EvdEPlot(args);

  args.outdirname = "proton_SiR_timecut1000ns_wSiL1Coinc";
  args.early_time_veto = true;
  args.early_time_cut = 1000;
  EvdEPlot(args);

  args.outdirname = "proton_SiR_timecut2000ns_wSiL1Coinc";
  args.early_time_veto = true;
  args.early_time_cut = 2000;
  EvdEPlot(args);

}
