#include "../../Utils/RawSpectrum_fromEvdE.C"

void Si16b_RawSpectrum_fromEvdE(std::string infilename, std::string outfilename) {

  RawSpectrum_fromEvdEArgs args;
  args.datafilename = infilename;
  args.outfilename = outfilename;

  const int n_hists = 2;
  std::string histnames[n_hists] = {"hEvdE", "hEvt"};
  std::string outdirnames[n_hists] = {"", "Time_"};

  const int n_pids = 2;
  std::string pids[n_pids] = {"TCutG", "PSel"};

  const int n_timecuts = 2;
  std::string timecuts[n_timecuts] = {"", "_noTimeCut"};
  double min_times[n_timecuts] = {400, 0};

  // For protons, deuterons etc
  const int n_particles = 5;
  std::string particle_names[n_particles] = {"all", "proton", "deuteron", "triton", "alpha"};//, "proton_combined"};
  double SiR3_veto_efficiency = 0.67;//0.68;
  double SiR3_veto_eff_uncertainty = 0.03;//0.0
  
  for (int i_hist = 0; i_hist < n_hists; ++i_hist) {
    std::string histname = histnames[i_hist];
    std::string outdirname = outdirnames[i_hist];

    for (int i_pid = 0; i_pid < n_pids; ++i_pid) {
      std::string pid = pids[i_pid];

      for (int i_timecut = 0; i_timecut < n_timecuts; ++i_timecut) {
	std::string timecut = timecuts[i_timecut];
	std::stringstream min_time; min_time.str("");
	min_time << min_times[i_timecut];
      
	for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
	  std::string i_particle_name = particle_names[i_particle];

	  // Get the raw spectrum
	  args.outdirname = outdirname + i_particle_name + "_" + pid + timecut;
	  std::string indirname = i_particle_name + "_SiR_timecut" + min_time.str() + "_10000ns_layerCoinc";
	  if (pid == "PSel" && i_particle_name != "all") {
	    indirname += "_" + pid;
	  }			    
	  args.datacuttreename = indirname + "/cuttree";
	  
	  args.datahistnames.clear();
	  args.scale_ratios.clear();
	  args.scale_ratio_errors.clear();
	  
	  std::string datahistname = indirname + "/" + histname + "_TwoLayer_12not3";
	  double scale_ratio = 1;
	  double scale_ratio_error = 0;
	  args.datahistnames.push_back(datahistname);
	  args.scale_ratios.push_back(scale_ratio);
	  args.scale_ratio_errors.push_back(scale_ratio_error);
	  if (i_particle_name == "proton") {
	    if (pid == "TCutG") {
	      args.datahistnames.push_back(i_particle_name + "3L_SiR_timecut" + min_time.str() + "_10000ns_layerCoinc/" + histname + "_ThreeLayer_123");
	    }
	    else {
	      args.datahistnames.push_back(i_particle_name + "_SiR_timecut" + min_time.str() + "_10000ns_layerCoinc_" + pid + "/" + histname + "_ThreeLayer_123");
	    }
	    double scale_factor = 1.0/SiR3_veto_efficiency;
	    double scale_factor_err = (SiR3_veto_eff_uncertainty/SiR3_veto_efficiency)*scale_factor;
	    args.scale_ratios.push_back(scale_factor);
	    args.scale_ratio_errors.push_back(scale_factor_err);
	    
	    if (pid == "PSel") {
	      args.datahistnames.push_back(i_particle_name + "_SiR_timecut" + min_time.str() + "_10000ns_layerCoinc_" + pid + "/" + histname + "_TwoLayer_123");
		
	      double scale_factor = (1-SiR3_veto_efficiency)/(SiR3_veto_efficiency);
	      double scale_factor_err = (SiR3_veto_eff_uncertainty/SiR3_veto_efficiency)*scale_factor;
	      args.scale_ratios.push_back(-1*scale_factor);
	      args.scale_ratio_errors.push_back(scale_factor_err);
	    }
	  }
	  args.projection_x = true;
	  RawSpectrum_fromEvdE(args);
	}
      }
    }
  }
}
      /*
    // Get TCutG spectra with SiL1 coincidence cut
    args.outdirname = outdirname + i_particle_name + "_TCutG_wSiL1Coinc";
    indirname = i_particle_name + "_SiR_timecut" + min_time.str() + "_10000ns_layerCoinc_wSiL1_XCoinc";
    args.datacuttreename = indirname + "/cuttree";

    args.datahistnames.clear();
    args.scale_ratios.clear();
    args.scale_ratio_errors.clear();

    datahistname = indirname + "/" + histname + "_TwoLayer_123";
    scale_ratio = 1;
    scale_ratio_error = 0;
    args.datahistnames.push_back(datahistname);
    args.scale_ratios.push_back(scale_ratio);
    args.scale_ratio_errors.push_back(scale_ratio_error);
    args.projection_x = true;
    RawSpectrum_fromEvdE(args);
*/
    /*    // Combine a do individual channels
    std::stringstream channel_str;
    args.datahistnames.clear();
    args.scale_ratios.clear();
    for (int i_channel = 2; i_channel < 16; ++i_channel) {
      channel_str.str("");
      channel_str << i_channel;
      std::string indirname = i_particle_name + "_SiR_timecut400_10000ns_layerCoinc_wSiL1_" + channel_str.str() + "Coinc";//"_PSel";
      args.datacuttreename = indirname + "/cuttree";

      datahistname = indirname + "/hEvdE_TwoLayer_123";
      scale_ratio = 1;
      args.datahistnames.push_back(datahistname);
      args.scale_ratios.push_back(scale_ratio);
    }
    args.outdirname = outdirname + i_particle_name + "_wSiL1Coinc";
    args.projection_x = true;
    RawSpectrum_fromEvdE(args);
    */
//    }
//  }
  
    /* // for combined two layer and three layer analysis    
  args.datahistnames.push_back("proton_SiR/hEvdE_TwoLayer_12not3");
  args.scale_ratios.push_back(1);
  args.datahistnames.push_back("proton_SiR/hEvdE_ThreeLayer_123");
  args.scale_ratios.push_back(1.0 / 0.64);
  RawSpectrum_fromEvdE(args);
    */

  /* // for three layer only
  args.datahistnames.clear();
  args.scale_ratios.clear();
  args.outdirname = "proton_ThreeLayer";
  args.datacuttreename = "proton_SiR/cuttree";
  args.datahistnames.push_back("proton_SiR/hEvdE_ThreeLayer_123");
  args.scale_ratios.push_back(1);
  RawSpectrum_fromEvdE(args);
  */
//}
