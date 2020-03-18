#include "../../Utils/LifetimeFit.C"

void Si16b_LifetimeFit(std::string infilename, std::string outfilename) {
  LifetimeFitArgs args;
  args.infilename = infilename;
  args.outfilename = outfilename;

  args.min_fit_time = 0;
  args.max_fit_time = 10000;
  args.min_flat_fit_time = 10000;
  args.max_flat_fit_time = 10000;
  args.rebin_factor = 20;
  args.ignore_region_min = -1;
  args.ignore_region_max = -1;
  args.ignore_region_flat_min = -1;
  args.ignore_region_flat_max = -1;
  args.include_resolution = false;
  

  const int n_particles = 5;
  std::string particle_names[n_particles] = {"all", "proton", "deuteron", "triton", "alpha"};
  double max_energy_cutoffs[n_particles] = {100000, 17000, 17000, 17000, 19000};
  
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string i_particle_name = particle_names[i_particle];

    args.datahistnames.clear();
    args.scale_ratios.clear();

    //    std::string histname = "hThickTime_TwoLayer_12not3";
    std::string histname = "hEvt_TwoLayer_12not3";
    std::string inhistname = i_particle_name + "_SiR_timecut0_10000ns_layerCoinc/" + histname;
    args.max_energy_cut = max_energy_cutoffs[i_particle];
    
    double scale_ratio = 1;
    args.datahistnames.push_back(inhistname);
    args.scale_ratios.push_back(scale_ratio);

    /*
    if (i_particle_name == "proton") {
      histname = "hThickTime_ThreeLayer_123";
      inhistname = i_particle_name + "_SiR_timecut0_10000ns_layerCoinc/" + histname;
      scale_ratio = 1.0 / 0.71;
      args.datahistnames.push_back(inhistname);
      args.scale_ratios.push_back(scale_ratio);
    }
    */
    
    args.project_y = false;
    args.project_x = true;
    args.outdirname = i_particle_name + "_SingleExp";
    args.double_exp = false;
    args.flat_bkg = false;
    LifetimeFit(args);

    args.outdirname = i_particle_name + "_SingleExp_FlatBkg";
    args.double_exp = false;
    args.flat_bkg = true;
    LifetimeFit(args);

    args.outdirname = i_particle_name + "_DoubleExp_FlatBkg";
    args.double_exp = true;
    args.flat_bkg = true;
    LifetimeFit(args);

    /*
    args.project_y = false;
    args.project_x = true;
    double min_energy_cut = 0;
    double max_energy_cut = 15000;
    double energy_cut_step = 1000;
    histname = "hEvt_TwoLayer_12not3";
    std::stringstream energy_cut_str;
    for (double i_min_energy_cut = min_energy_cut; i_min_energy_cut < max_energy_cut; i_min_energy_cut += energy_cut_step) {
      args.min_energy_cut = i_min_energy_cut;
      //      args.max_energy_cut = i_min_energy_cut + energy_cut_step;
      args.max_energy_cut = max_energy_cut;

      energy_cut_str.str("");
      energy_cut_str << args.min_energy_cut << "_" << args.max_energy_cut;
      
      std::string inhistname = i_particle_name + "_SiR_timecut0_10000ns_layerCoinc/" + histname;
      args.outdirname = i_particle_name + "_SingleExp_" + energy_cut_str.str();
      args.double_exp = false;
      args.flat_bkg = false;
      LifetimeFit(args);
      
      std::string inhistname = i_particle_name + "_SiR_timecut0_10000ns_layerCoinc/" + histname;
      args.outdirname = i_particle_name + "_SingleExp_FlatBkg_" + energy_cut_str.str();
      args.double_exp = false;
      args.flat_bkg = true;
      LifetimeFit(args);
    }
    */
  }





  /*
  std::string inhistname = "SiL3/hEnergyTime";
  args.max_energy_cut = -1;
  args.rebin_factor = 10;
  args.double_exp = false;
  args.project_y = false;
  args.project_x = true;

  double min_energy_cut = 0;
  double max_energy_cut = 15500;
  double energy_cut_step = 500;
  std::stringstream energy_cut_str;
  for (double i_min_energy_cut = min_energy_cut; i_min_energy_cut < max_energy_cut; i_min_energy_cut += energy_cut_step) {

    energy_cut_str.str("");
    energy_cut_str << i_min_energy_cut;

    args.min_energy_cut = i_min_energy_cut;

    args.flat_bkg = false;
    args.min_fit_time = 1000;
    args.max_fit_time = 5000;
    args.outdirname = "SiL3_" + energy_cut_str.str() + "keVCut";
    LifetimeFit(args);

    args.flat_bkg = true;
    args.min_fit_time = 1000;
    args.max_fit_time = 20000;
    args.min_flat_fit_time = 10000;
    args.max_flat_fit_time = 20000;
    args.outdirname = "SiL3_" + energy_cut_str.str() + "keVCut_wFlatBkg";
    LifetimeFit(args);
  }


  std::string inhistname = "proton_SiR_timecut_wSiL1Coinc/hTime_12";
  args.min_fit_time = 200;
  args.max_fit_time = 20000;
  args.rebin_factor = 10;
  args.project_y = false;
  args.project_x = true;
  args.outdirname = "Proton_wSiL1Coinc_12";
  args.double_exp = false;
  args.flat_bkg = false;
  LifetimeFit(args);

  std::string inhistname = "proton_SiR_timecut_wSiL1Coinc/hTime_123";
  args.outdirname = "Proton_wSiL1Coinc_123";
  LifetimeFit(args);

  std::string inhistname = "proton_SiR_timecut_wSiL1Coinc/hTime_TwoLayer_12not3";
  args.outdirname = "Proton_wSiL1Coinc_TwoLayer_12not3";
  LifetimeFit(args);
  */
}
