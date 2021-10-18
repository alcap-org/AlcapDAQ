#include "../../Utils/EvdEPlot.C"

struct Setting {
  Setting(std::string name, std::string cutfilename, std::string cutname, PSelParticle psel_part = invalid)
    : name(name), cutfilename(cutfilename), cutname(cutname), psel_part(psel_part) {}
  std::string name;
  std::string cutfilename;
  std::string cutname;
  PSelParticle psel_part;
};

void Si16b_EvdEPlot(std::string infilename, std::string outfilename) {

  EvdEPlotArgs args;
  args.infilename = infilename;

  args.min_x_energy = 0;
  args.max_x_energy = 25;
  args.x_energy_width = 0.01;
  args.min_y_energy = 0;
  args.max_y_energy = 20;
  args.y_energy_width = 0.01;

  args.min_time = -30000;
  args.max_time = 30000;
  args.time_width = 10;

  args.min_tdiff = -2000;
  args.max_tdiff = 2000;
  args.tdiff_width = 10;

  args.debug = false;

  Setting all("all", "", "");
  Setting all_proton("all_proton", "~/data/results/Ti50/cut_all_proton_tighter.root", "all_proton");
  //  Setting protons("proton", "~/data/results/Si16b/cuts_new_threelayer.root", "proton_cut", proton);
  //  Setting deuterons("deuteron", "~/data/results/Si16b/cuts_new_threelayer.root", "deuteron_cut", deuteron);
  //  Setting tritons("triton", "~/data/results/Si16b/cuts_new_threelayer.root", "triton_cut", triton);
  //  Setting alphas("alpha", "~/data/results/Si16b/cuts_new_threelayer.root", "alpha_cut", alpha);
  Setting protons_3sig("proton_3sig", "~/data/results/Si16b/si16b-cuts-cutoff.root", "r_hLg_SiR_EvDeltaE_proton_3sigma_cutoff", proton);
  Setting deuterons_3sig("deuteron_3sig", "~/data/results/Si16b/si16b-cuts-cutoff.root", "r_hLg_SiR_EvDeltaE_deuteron_3sigma_cutoff", deuteron);
  Setting tritons_3sig("triton_3sig", "~/data/results/Si16b/si16b-cuts-cutoff.root", "r_hLg_SiR_EvDeltaE_triton_3sigma_cutoff", triton);
  Setting alphas_3sig("alpha_3sig", "~/data/results/Si16b/si16b-cuts-cutoff.root", "r_hLg_SiR_EvDeltaE_alpha_3sigma_cutoff", alpha);
  //  Setting alphas("alpha", "~/data/results/Si16b/new_alpha_cut_9MeV.root", "alpha_cut", alpha);
  Setting muspot_both("muspot_both", "~/data/results/Si16b/cut_muon-spots.root", "both");
  Setting muspot_lower("muspot_lower", "~/data/results/Si16b/cut_muon-spots.root", "lower");
  Setting muspot_upper("muspot_upper", "~/data/results/Si16b/cut_muon-spots.root", "upper");
  //  Setting protons3L("proton3L", "~/data/results/Si16b/cuts_new_threelayer.root", "proton_cut", proton);
  //  Setting protons3L("proton3L", "~/data/results/Si16b/proton_cut_threelayer.root", "proton_cut", proton);
  //  Setting protons3L("proton3L", "~/data/results/Si16b/mark-al50-sir3-cuts-keV.root", "r_sir3_hLg_SiR_EvDeltaE_proton_3sigma_keV", proton);
  //  Setting protons3L_3sig("proton3L_3sig", "~/data/results/Si16b/mark-si16b-sir3-cuts.root", "r_sir3_hLg_SiR_EvDeltaE_proton_3sigma_cut", proton);
  Setting protons3L_3sig("proton3L_3sig", "~/data/results/Si16b/si16b-sir3-cuts.root", "r_hLg_SiR_EvDeltaE_proton_3sigma", proton);

  Setting protons_2sig("proton_2sig", "~/data/results/Si16b/si16b-cuts-cutoff.root", "r_hLg_SiR_EvDeltaE_proton_2sigma_cutoff", proton);
  Setting deuterons_2sig("deuteron_2sig", "~/data/results/Si16b/si16b-cuts-cutoff.root", "r_hLg_SiR_EvDeltaE_deuteron_2sigma_cutoff", deuteron);
  Setting tritons_2sig("triton_2sig", "~/data/results/Si16b/si16b-cuts-cutoff.root", "r_hLg_SiR_EvDeltaE_triton_2sigma_cutoff", triton);
  Setting alphas_2sig("alpha_2sig", "~/data/results/Si16b/si16b-cuts-cutoff.root", "r_hLg_SiR_EvDeltaE_alpha_2sigma_cutoff", alpha);
  //  Setting protons3L_2sig("proton3L_2sig", "~/data/results/Si16b/mark-si16b-sir3-cuts.root", "r_sir3_hLg_SiR_EvDeltaE_proton_2sigma", proton);
  Setting protons3L_2sig("proton3L_2sig", "~/data/results/Si16b/si16b-sir3-cuts.root", "r_hLg_SiR_EvDeltaE_proton_2sigma", proton);

  Setting protons_1sig("proton_1sig", "~/data/results/Si16b/si16b-cuts-cutoff.root", "r_hLg_SiR_EvDeltaE_proton_1sigma_cutoff", proton);
  Setting deuterons_1sig("deuteron_1sig", "~/data/results/Si16b/si16b-cuts-cutoff.root", "r_hLg_SiR_EvDeltaE_deuteron_1sigma_cutoff", deuteron);
  Setting tritons_1sig("triton_1sig", "~/data/results/Si16b/si16b-cuts-cutoff.root", "r_hLg_SiR_EvDeltaE_triton_1sigma_cutoff", triton);
  Setting alphas_1sig("alpha_1sig", "~/data/results/Si16b/si16b-cuts-cutoff.root", "r_hLg_SiR_EvDeltaE_alpha_1sigma_cutoff", alpha);
  //  Setting protons3L_1sig("proton3L_1sig", "~/data/results/Si16b/mark-si16b-sir3-cuts.root", "r_sir3_hLg_SiR_EvDeltaE_proton_1sigma", proton);
  Setting protons3L_1sig("proton3L_1sig", "~/data/results/Si16b/si16b-sir3-cuts.root", "r_hLg_SiR_EvDeltaE_proton_1sigma", proton);

  std::vector<Setting> settings;
  settings.push_back(all);
  //  settings.push_back(all_proton);
  settings.push_back(protons_3sig);
  settings.push_back(deuterons_3sig);
  settings.push_back(tritons_3sig);
  settings.push_back(alphas_3sig);
  settings.push_back(protons3L_3sig);
  settings.push_back(protons_2sig);
  settings.push_back(deuterons_2sig);
  settings.push_back(tritons_2sig);
  settings.push_back(alphas_2sig);
  settings.push_back(protons3L_2sig);
  settings.push_back(protons_1sig);
  settings.push_back(deuterons_1sig);
  settings.push_back(tritons_1sig);
  settings.push_back(alphas_1sig);
  settings.push_back(protons3L_1sig);
  // //  settings.push_back(muspot_both);
  // //  args.debug = true;
  // //  settings.push_back(muspot_lower);
  // //  settings.push_back(muspot_upper);

  
  const int n_timecuts = 7;
  double min_timecuts[n_timecuts] = {0, 200, 400, 600, 800, 1000, 2000};
  double max_timecuts[n_timecuts] = {10000, 10000, 10000, 10000, 10000, 10000, 10000};

  const int n_layer_coincidences = 4;
  std::string layer_coincidences[n_layer_coincidences] = {"noLayerCoinc", "layerCoinc200ns", "layerCoinc500ns", "thickLateCoinc"};
  double min_layer_coincidences[n_layer_coincidences] = {-10000, -200, -500, 200};
  double max_layer_coincidences[n_layer_coincidences] = {10000, 200, 500, 10000};

  std::string tree = "SiR";
  std::stringstream outdirname;
  for (std::vector<Setting>::const_iterator i_setting = settings.begin(); i_setting != settings.end(); ++i_setting) {
    args.outfilename = outfilename;
    
    args.treename = "siBlockTree_" + tree;

    args.thin_layer = "thin";
    args.thick_layer = "thick";
    args.third_layer = "third";
    args.veto_layer = "";

    if (i_setting->name == "alpha") {
      args.max_x_energy = 30;
    }
    else {
      args.max_x_energy = 25;
    }

    bool original_do_cut = false;
    if (i_setting->name == "all") {
      original_do_cut = false;
    }
    else {
      original_do_cut = true;
      args.cutfilename = i_setting->cutfilename;
      args.cutname = i_setting->cutname;
    }

    for (int i_layer_coincidence = 0; i_layer_coincidence < n_layer_coincidences; ++i_layer_coincidence) {
      std::string layer_coincidence = layer_coincidences[i_layer_coincidence];

      args.do_thin_time_cut = false;
      args.do_thick_time_cut = false;
      args.do_cut = original_do_cut;
      args.do_psel = false;
      args.do_thin_channel_cut = false;
      args.do_third_channel_cut = false;
	
      // With and without layer coinc
      args.outdirname = i_setting->name + "_" + tree + "_" + layer_coincidence + "_TCutG";
      args.layer_coincidence_vetos = true;
      args.min_layer12_coincidence_time = min_layer_coincidences[i_layer_coincidence];
      args.max_layer12_coincidence_time = max_layer_coincidences[i_layer_coincidence];
      args.min_layer23_coincidence_time = min_layer_coincidences[i_layer_coincidence];
      args.max_layer23_coincidence_time = max_layer_coincidences[i_layer_coincidence];
      EvdEPlot(args);

      if (i_setting->psel_part != invalid) {
	args.do_cut = false;
	args.do_psel = true;
	args.psel_particle = i_setting->psel_part;
	
	outdirname.str("");
	outdirname << i_setting->name << "_" << tree << "_" << layer_coincidence << "_PSel";
	args.outdirname = outdirname.str();
	EvdEPlot(args);
	args.do_psel = false;
	args.do_cut = original_do_cut;
      }
      
      // By thin-channel (for "all" only)
      //    if (i_setting->name == "all") {
      const int n_thin_channels = 4;
      for (int i_channel = 0; i_channel < n_thin_channels; ++i_channel) {
	outdirname.str("");
	outdirname << i_setting->name << "_SiR1_" << i_channel+1 << "_" << layer_coincidence << "_TCutG";
	args.outdirname = outdirname.str();
	args.do_thin_channel_cut = true;
	args.thin_channel_cut = i_channel;
	EvdEPlot(args);
      }
      args.do_thin_channel_cut = false;
            
      // Time cuts
      for (int i_timecut = 0; i_timecut < n_timecuts; ++i_timecut) {
	outdirname.str("");
	outdirname << i_setting->name << "_" << tree << "_timecut" << min_timecuts[i_timecut] << "_" << max_timecuts[i_timecut] << "ns_" << layer_coincidence;
	args.outdirname = outdirname.str();
	args.do_thick_time_cut = true;
	args.min_thick_time_cut = min_timecuts[i_timecut];
	args.max_thick_time_cut = max_timecuts[i_timecut];
	EvdEPlot(args);

	if (i_setting->psel_part != invalid) {
	  args.do_cut = false;
	  args.do_psel = true;
	  args.psel_particle = i_setting->psel_part;

	  outdirname.str("");
	  outdirname << i_setting->name << "_" << tree << "_timecut" << min_timecuts[i_timecut] << "_" << max_timecuts[i_timecut] << "ns_"
		     << layer_coincidence << "_PSel";
	  args.outdirname = outdirname.str();
	  EvdEPlot(args);

	  args.do_psel = false;
	  args.do_cut = original_do_cut;
	}
      }
      args.do_thick_time_cut = false;
    }

    /*
    // With individual SiL1 channel coincs
    args.treename = "siBlockTree_Target";
    args.thin_layer = "thick"; // SiR1
    args.thick_layer = "third"; //SiR2
    args.third_layer = "thin"; // SiL1
    args.veto_layer = "";
    args.layer_coincidence_vetos = true;
    args.min_layer12_coincidence_time = -100;
    args.max_layer12_coincidence_time = 100;
    args.min_layer23_coincidence_time = -10000; // third layer is SiL1, which we want to be before SiR2,
    args.max_layer23_coincidence_time = 0;      // EvdEPlot.C does third layer - second lyaer
    args.do_thin_time_cut = true;
    args.min_thin_time_cut = 400;
    args.max_thin_time_cut = 10000;

    // with any SiL1 channels
    args.do_cut = original_do_cut;
    args.do_psel = false;
    args.do_third_channel_cut = true;
    args.third_channel_cut = -1;
    if (args.do_cut) {
      args.cutfilename = i_setting->cutfilename;
      args.cutname = i_setting->cutname;
    }
    outdirname.str("");
    outdirname << i_setting->name << "_SiR_timecut" << args.min_thin_time_cut << "_" << args.max_thin_time_cut << "ns_layerCoinc"
	       << "_wSiL1_XCoinc";
    args.outdirname = outdirname.str();
    EvdEPlot(args);
    */
    /*
    const int n_SiL1_channels = 14; // only analysed channels SiL1-2 to SiL1-15 in TMELoop (these are then numbered from 0 to 13 so I need to add 2)
    for (int i_SiL1_channel = 0; i_SiL1_channel < n_SiL1_channels; ++i_SiL1_channel) {
      args.do_cut = original_do_cut;
      args.do_psel = false;
      args.do_third_channel_cut = true;
      args.third_channel_cut = i_SiL1_channel;
      if (args.do_cut) {
	args.cutfilename = i_setting->cutfilename;
	args.cutname = i_setting->cutname;
	outdirname.str("");
	outdirname << i_setting->name << "_SiR_timecut" << args.min_thin_time_cut << "_" << args.max_thin_time_cut << "ns_layerCoinc"
		   << "_wSiL1_" << i_SiL1_channel+2 << "Coinc";
	args.outdirname = outdirname.str();
	EvdEPlot(args);
      }

      if (i_setting->psel_part != invalid) {
	args.do_cut = false;
	args.do_psel = true;
	args.psel_particle = i_setting->psel_part;
	outdirname.str("");
	outdirname << i_setting->name << "_SiR_timecut" << args.min_thin_time_cut << "_" << args.max_thin_time_cut << "ns_layerCoinc"
		   << "_wSiL1_" << i_SiL1_channel+2 << "Coinc_PSel";
	args.outdirname = outdirname.str();
	EvdEPlot(args);
	
	args.do_psel = false;
	args.do_cut = original_do_cut;
      }
    }
    */
  }
}
