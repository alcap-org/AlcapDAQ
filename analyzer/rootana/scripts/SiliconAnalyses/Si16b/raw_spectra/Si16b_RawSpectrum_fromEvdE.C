#include "../../Utils/RawSpectrum_fromEvdE.C"

void Si16b_RawSpectrum_fromEvdE(std::string infilename, std::string outfilename) {

  RawSpectrum_fromEvdEArgs args;
  args.datafilename = infilename;
  args.outfilename = outfilename;

  const int n_hists = 2;
  std::string histnames[n_hists] = {"hEvdE", "hEvt"};
  std::string outdirnames[n_hists] = {"", "Time_"};

  const int n_pids = 1;//2;
  std::string pids[n_pids] = {"TCutG"};//, "PSel"};

  const int n_sigmas = 3;
  std::string sigmas[n_sigmas] = {"3", "2", "1"};

  const int n_layer_coincs = 2;
  std::string layer_coincs[n_layer_coincs] = {"200ns", "500ns"};
  
  const int n_timecuts = 2;
  std::string timecuts[n_timecuts] = {"400ns", "0ns"};
  double min_times[n_timecuts] = {400, 0};

  // For protons, deuterons etc
  const int n_particles = 4;
  std::string particle_names[n_particles] = {"proton", "deuteron", "triton", "alpha"};//, "proton_combined"};
  double max_energy_cutoffs[n_particles] = {25,17.3,17.3,25};//{25, 25, 25, 25};//{20, 17, 17, 20};//{100, 20, 20, 20, 20};//17, 17, 17, 19};
  //  std::string particle_names[n_particles] = {"all", "proton", "deuteron", "triton", "alpha"};//, "proton_combined"};
  //  double max_energy_cutoffs[n_particles] = {100, 20, 20, 20, 20};//17, 17, 17, 19};
  double SiR3_veto_efficiency = 0.74;//0.81;//1.00;//0.71;//0.68;//0.67;//0.68;
  double SiR3_veto_eff_uncertainty = 0.07;//0.05;//0.02;//0.02;//0.03;//0.0
  
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

	  for (int i_sigma = 0; i_sigma < n_sigmas; ++i_sigma) {
	    std::string sigma = sigmas[i_sigma];

	    for (int i_layer_coinc = 0; i_layer_coinc < n_layer_coincs; ++i_layer_coinc) {
	      std::string layer_coinc = layer_coincs[i_layer_coinc];
	  
	      // Get the raw spectrum
	      args.outdirname = outdirname + i_particle_name + "_" + pid + "_" + sigma + "sig_layerCoinc" + layer_coinc + "_" + "tGT" + timecut;
	      std::string indirname = i_particle_name + "_" + sigma + "sig_SiR_timecut" + min_time.str() + "_10000ns_layerCoinc" + layer_coinc;
	      if (pid == "PSel" && i_particle_name != "all") {
		indirname += "_" + pid;
	      }			    
	      args.datacuttreename = indirname + "/cuttree";
	      args.max_energy_cutoff = max_energy_cutoffs[i_particle];
	  
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
		  args.datahistnames.push_back(i_particle_name + "3L_" + sigma + "sig_SiR_timecut" + min_time.str() + "_10000ns_layerCoinc" + layer_coinc + "/" + histname + "_ThreeLayer_123");
		}
		else {
		  args.datahistnames.push_back(i_particle_name + "_" + sigma + "sig_SiR_timecut" + min_time.str() + "_10000ns_layerCoinc" + layer_coinc + "_" + pid + "/" + histname + "_ThreeLayer_123");
		}
		double scale_factor = SiR3_veto_efficiency;
		double scale_factor_err = (SiR3_veto_eff_uncertainty);///SiR3_veto_efficiency)*scale_factor;
		args.scale_ratios.push_back(scale_factor);
		args.scale_ratio_errors.push_back(scale_factor_err);
		
		if (pid == "PSel") {
		  args.datahistnames.push_back(i_particle_name + "_" + sigma + "sig_SiR_timecut" + min_time.str() + "_10000ns_layerCoinc" + layer_coinc + "_" + pid + "/" + histname + "_TwoLayer_123");
		}
		else {
		  args.datahistnames.push_back(i_particle_name + "_" + sigma + "sig_SiR_timecut" + min_time.str() + "_10000ns_layerCoinc" + layer_coinc + "/" + histname + "_TwoLayer_123");
		}
		scale_factor = (SiR3_veto_efficiency)/(1-SiR3_veto_efficiency);
		scale_factor_err = std::sqrt( (SiR3_veto_eff_uncertainty*SiR3_veto_eff_uncertainty)*( 1/(1-SiR3_veto_efficiency) + 2/(SiR3_veto_efficiency*(1-SiR3_veto_efficiency))) );//(SiR3_veto_eff_uncertainty/SiR3_veto_efficiency)*scale_factor;
		args.scale_ratios.push_back(-1*scale_factor);
		args.scale_ratio_errors.push_back(scale_factor_err);

		args.min_add_error_energy = 15.5;
		args.max_add_error_energy = 16.5;
		args.additional_error = 0.10; // add 10% due to dead layer uncertainty

		// Correct for SiR3 punch-throughs
		args.energies_to_scale.clear();
		args.scales.clear();
		args.err_scales.clear();
		args.energies_to_scale.push_back(16); args.scales.push_back(0.052); args.err_scales.push_back(0.10);
		args.energies_to_scale.push_back(16.5); args.scales.push_back(0.001); args.err_scales.push_back(0.10);
		args.energies_to_scale.push_back(17); args.scales.push_back(0.001); args.err_scales.push_back(0.10);
		args.energies_to_scale.push_back(17.5); args.scales.push_back(0.049); args.err_scales.push_back(0.10);
		args.energies_to_scale.push_back(18); args.scales.push_back(0.110); args.err_scales.push_back(0.10);
		args.energies_to_scale.push_back(18.5); args.scales.push_back(0.097); args.err_scales.push_back(0.10);
		args.energies_to_scale.push_back(19); args.scales.push_back(0.086); args.err_scales.push_back(0.10);
		args.energies_to_scale.push_back(19.5); args.scales.push_back(0.059); args.err_scales.push_back(0.10);
		args.energies_to_scale.push_back(20); args.scales.push_back(0.054); args.err_scales.push_back(0.10);
		args.energies_to_scale.push_back(20.5); args.scales.push_back(0.077); args.err_scales.push_back(0.10);
	      }
	      else {
		args.min_add_error_energy = 1000000;
		args.max_add_error_energy = 1000000;
		args.additional_error = 1;

		args.energies_to_scale.clear();
		args.scales.clear();
		args.err_scales.clear();
	      }
	      args.projection_x = true;
	      RawSpectrum_fromEvdE(args);
	    }
	  }
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
