#include "scripts/SiliconAnalyses/Utils/LifetimeFit.C"

void Ti50_LifetimeFit(std::string infilename, std::string outfilename) {
  LifetimeFitArgs args;
  args.infilename = infilename;
  args.outfilename = outfilename;

  const int n_arms = 2;
  std::string arms[n_arms] = {"SiR", "SiL"};
  
  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {

    std::string this_arm = arms[i_arm];

    args.inhistname = "proton_" + this_arm + "_timecut/hTime_12not3";
    args.min_fit_time = 200;
    args.max_fit_time = 20000;
    args.rebin_factor = 10;
    args.project_y = false;
    args.project_x = false;

    args.outdirname = this_arm + "_Proton_wTimeCut_SingleExp";
    args.double_exp = false;
    args.flat_bkg = false;
    LifetimeFit(args);
    
    args.outdirname = this_arm + "_Proton_wTimeCut_DoubleExp";
    args.double_exp = true;
    args.flat_bkg = false;
    LifetimeFit(args);
  
    args.outdirname = this_arm + "_Proton_wTimeCut_SingleExp_FlatBkg";
    args.double_exp = false;
    args.flat_bkg = true;
    LifetimeFit(args);

    args.outdirname = this_arm + "_Proton_wTimeCut_DoubleExp_FlatBkg";
    args.double_exp = true;
    args.flat_bkg = true;
    LifetimeFit(args);

    args.inhistname = "deuteron_" + this_arm + "_timecut/hTime_12not3";
    args.min_fit_time = 200;
    args.max_fit_time = 20000;
    args.rebin_factor = 10;
    args.project_y = false;
    args.project_x = false;

    args.outdirname = this_arm + "_Deuteron_wTimeCut_SingleExp";
    args.double_exp = false;
    args.flat_bkg = false;
    LifetimeFit(args);

    args.inhistname = "triton_" + this_arm + "_timecut/hTime_12not3";
    args.min_fit_time = 200;
    args.max_fit_time = 20000;
    args.rebin_factor = 10;
    args.project_y = false;
    args.project_x = false;

    args.outdirname = this_arm + "_Triton_wTimeCut_SingleExp";
    args.double_exp = false;
    args.flat_bkg = false;
    LifetimeFit(args);

    args.inhistname = "alpha_" + this_arm + "_timecut/hTime_12not3";
    args.min_fit_time = 200;
    args.max_fit_time = 20000;
    args.rebin_factor = 10;
    args.project_y = false;
    args.project_x = false;

    args.outdirname = this_arm + "_Alpha_wTimeCut_SingleExp";
    args.double_exp = false;
    args.flat_bkg = false;
    LifetimeFit(args);
  }


  args.inhistname = "SiL3/hEnergyTime";
  args.max_energy_cut = -1;
  args.min_fit_time = 500;
  args.max_fit_time = 20000;
  args.rebin_factor = 20;
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
    args.outdirname = "SiL3_" + energy_cut_str.str() + "keVCut";
    LifetimeFit(args);

    args.flat_bkg = true;
    args.outdirname = "SiL3_" + energy_cut_str.str() + "keVCut_wFlatBkg";
    LifetimeFit(args);
  }
}
