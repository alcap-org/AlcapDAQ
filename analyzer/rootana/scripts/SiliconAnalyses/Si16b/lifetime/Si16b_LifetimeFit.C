#include "scripts/SiliconAnalyses/Utils/LifetimeFit.C"

void Si16b_LifetimeFit(std::string infilename, std::string outfilename) {
  LifetimeFitArgs args;
  args.infilename = infilename;
  args.outfilename = outfilename;

  args.inhistname = "proton_SiR_timecut/hTime_12not3";
  args.min_fit_time = 200;
  args.max_fit_time = 20000;
  args.rebin_factor = 10;
  args.project_y = false;
  args.project_x = false;

  args.outdirname = "Proton_wTimeCut_SingleExp";
  args.double_exp = false;
  args.flat_bkg = false;
  LifetimeFit(args);

  args.outdirname = "Proton_wTimeCut_DoubleExp";
  args.double_exp = true;
  args.flat_bkg = false;
  LifetimeFit(args);

  args.outdirname = "Proton_wTimeCut_SingleExp_FlatBkg";
  args.double_exp = false;
  args.flat_bkg = true;
  LifetimeFit(args);

  args.outdirname = "Proton_wTimeCut_DoubleExp_FlatBkg";
  args.double_exp = true;
  args.flat_bkg = true;
  LifetimeFit(args);

  args.inhistname = "deuteron_SiR_timecut/hTime_12not3";
  args.min_fit_time = 200;
  args.max_fit_time = 20000;
  args.rebin_factor = 10;
  args.project_y = false;
  args.project_x = false;

  args.outdirname = "Deuteron_wTimeCut_SingleExp";
  args.double_exp = false;
  args.flat_bkg = false;
  LifetimeFit(args);

  args.inhistname = "triton_SiR_timecut/hTime_12not3";
  args.min_fit_time = 200;
  args.max_fit_time = 20000;
  args.rebin_factor = 10;
  args.project_y = false;
  args.project_x = false;

  args.outdirname = "Triton_wTimeCut_SingleExp";
  args.double_exp = false;
  args.flat_bkg = false;
  LifetimeFit(args);

  args.inhistname = "alpha_SiR_timecut/hTime_12not3";
  args.min_fit_time = 200;
  args.max_fit_time = 20000;
  args.rebin_factor = 10;
  args.project_y = false;
  args.project_x = false;

  args.outdirname = "Alpha_wTimeCut_SingleExp";
  args.double_exp = false;
  args.flat_bkg = false;
  LifetimeFit(args);



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


  args.inhistname = "proton_SiR_timecut_wSiL1Coinc/hTime_12";
  args.min_fit_time = 200;
  args.max_fit_time = 20000;
  args.rebin_factor = 10;
  args.project_y = false;
  args.project_x = false;
  args.outdirname = "Proton_wTimeCut_wSiL1Coinc_12";
  args.double_exp = false;
  args.flat_bkg = false;
  LifetimeFit(args);

  args.inhistname = "proton_SiR_timecut_wSiL1Coinc/hTime_123";
  args.outdirname = "Proton_wTimeCut_wSiL1Coinc_123";
  LifetimeFit(args);

  args.inhistname = "proton_SiR_timecut_wSiL1Coinc/hTime_12not3";
  args.outdirname = "Proton_wTimeCut_wSiL1Coinc_12not3";
  LifetimeFit(args);
}
