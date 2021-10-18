#include "../../Utils/EvdEPlot.C"

struct Setting {
  Setting(std::string name, std::string cutfilename, std::string cutname) : name(name), cutfilename(cutfilename), cutname(cutname) {}
  std::string name;
  std::string cutfilename;
  std::string cutname;
};


void SiL3_EvdEPlot(std::string infilename, std::string outfilename) {

  EvdEPlotArgs args;
  args.infilename = infilename;
  args.outfilename = outfilename;

  args.min_x_energy = 0;
  args.max_x_energy = 25;
  args.x_energy_width = 0.050;
  args.min_y_energy = 0;
  args.max_y_energy = 25;
  args.y_energy_width = 0.050;

  args.min_time = -30000;
  args.max_time = 30000;
  args.time_width = 10;

  args.min_tdiff = -2000;
  args.max_tdiff = 2000;
  args.tdiff_width = 10;

  args.debug = false;

  Setting all("all", "", "");
  Setting all_proton("all_proton", "~/data/results/Ti50/cut_all_proton_tighter.root", "all_proton");
  Setting proton("proton", "~/data/results/Si16b/cuts_new_threelayer.root", "proton_cut");
  Setting deuteron("deuteron", "~/data/results/Si16b/cuts_new_threelayer.root", "deuteron_cut");
  Setting triton("triton", "~/data/results/Si16b/cuts_new_threelayer.root", "triton_cut");
  Setting alpha("alpha", "~/data/results/Si16b/cuts_new_threelayer.root", "alpha_cut");
  Setting muspot_both("muspot_both", "~/data/results/Si16b/cut_muon-spots.root", "both");
  Setting muspot_lower("muspot_lower", "~/data/results/Si16b/cut_muon-spots.root", "lower");
  Setting muspot_upper("muspot_upper", "~/data/results/Si16b/cut_muon-spots.root", "upper");
  std::vector<Setting> settings;
  settings.push_back(all);
  //  settings.push_back(all_proton);
  settings.push_back(proton);
  //  settings.push_back(deuteron);
  //  settings.push_back(triton);
  //  settings.push_back(alpha);
  //  settings.push_back(muspot_both);
  //  args.debug = true;
  //  settings.push_back(muspot_lower);
  //  settings.push_back(muspot_upper);

  const int n_timecuts = 7;
  double min_timecuts[n_timecuts] = {-200, 200, 400, 600, 800, 1000, 2000};
  double max_timecuts[n_timecuts] = {200, 10000, 10000, 10000, 10000, 10000, 10000};
  
  const int n_trees = 2;
  std::string treenames[n_trees] = {"SiR", "Target"};

  const int n_layer_coincidences = 3;
  std::string layer_coincidences[n_layer_coincidences] = {"noLayerCoinc", "layerCoinc", "thickLateCoinc"};
  double min_layer_coincidences[n_layer_coincidences] = {-10000, -100, 200};
  double max_layer_coincidences[n_layer_coincidences] = {10000, 100, 10000};

  for (int i_tree = 0; i_tree < n_trees; ++i_tree) {
    std::string tree = treenames[i_tree];

    args.treename = "siBlockTree_" + tree;

    std::stringstream outdirname;
    for (std::vector<Setting>::const_iterator i_setting = settings.begin(); i_setting != settings.end(); ++i_setting) {
      args.thin_layer = "thin";
      args.thick_layer = "thick";
      args.third_layer = "third";
      args.veto_layer = "";

      for (int i_layer_coincidence = 0; i_layer_coincidence < n_layer_coincidences; ++i_layer_coincidence) {
	std::string layer_coincidence = layer_coincidences[i_layer_coincidence];

	args.do_thin_time_cut = false;
	args.do_thick_time_cut = false;
	if (i_setting->name == "all") {
	  args.do_cut = false;
	}
	else {
	  args.do_cut = true;
	  args.cutfilename = i_setting->cutfilename;
	  args.cutname = i_setting->cutname;
	}
	args.do_psel = false;
	args.do_thin_channel_cut = false;
	
	// With and without layer coinc
	args.outdirname = i_setting->name + "_" + tree + "_" + layer_coincidence;
	args.layer_coincidence_vetos = true;
	args.min_layer12_coincidence_time = min_layer_coincidences[i_layer_coincidence];
	args.max_layer12_coincidence_time = max_layer_coincidences[i_layer_coincidence];
	args.min_layer23_coincidence_time = min_layer_coincidences[i_layer_coincidence];
	args.max_layer23_coincidence_time = max_layer_coincidences[i_layer_coincidence];
	EvdEPlot(args);
      
	// By thin-channel (for "all" only)
	//    if (i_setting->name == "all") {
	if (tree == "SiR") {
	  const int n_thin_channels = 4;
	  for (int i_channel = 0; i_channel < n_thin_channels; ++i_channel) {
	    outdirname.str("");
	    outdirname << i_setting->name << "_SiR1_" << i_channel+1 << "_" << layer_coincidence;
	    args.outdirname = outdirname.str();
	    args.do_thin_channel_cut = true;
	    args.thin_channel_cut = i_channel;
	    EvdEPlot(args);
	  }
	  args.do_thin_channel_cut = false;
	}

	// Time cuts
	for (int i_timecut = 0; i_timecut < n_timecuts; ++i_timecut) {
	  outdirname.str("");
	  outdirname << i_setting->name << "_" << tree << "_timecut" << min_timecuts[i_timecut] << "_" << max_timecuts[i_timecut] << "ns_" << layer_coincidence;
	  args.outdirname = outdirname.str();
	  args.do_thin_time_cut = true;
	  args.min_thin_time_cut = min_timecuts[i_timecut];
	  args.max_thin_time_cut = max_timecuts[i_timecut];
	  EvdEPlot(args);
	}
	args.do_thin_time_cut = false;
      
	/*    // With and without SiL1 coinc
	      args.treename = "siBlockTree_Target";
	      args.thin_layer = "thick";
	      args.thick_layer = "third";
	      args.third_layer = "thin";
	      args.veto_layer = "";
	      args.layer_coincidence_vetos = true;
	      args.layer12_coincidence_time = layer_coincidences[i_tree];
	      args.layer23_coincidence_time = 50000;
	      args.outdirname = i_setting->name + "_SiR_wSiL1Coinc";
	      EvdEPlot(args);
	*/
	// possibly do it with timecuts here too
      }
    }
  }
}
