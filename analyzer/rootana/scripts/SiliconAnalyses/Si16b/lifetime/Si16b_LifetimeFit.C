#include "scripts/SiliconAnalyses/Utils/LifetimeFit.C"

void Si16b_LifetimeFit(std::string infilename, std::string outfilename) {
  LifetimeFitArgs args;
  args.infilename = infilename;
  args.outfilename = outfilename;

  args.inhistname = "proton_SiR_timecut/hTime";
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

  args.inhistname = "SiL3/hEnergyTime";
  args.outdirname = "SiL3";
  args.min_energy_cut = -1;
  args.max_energy_cut = -1;
  args.min_fit_time = 1000;
  args.max_fit_time = 4000;
  args.rebin_factor = 1;
  args.double_exp = false;
  args.flat_bkg = false;
  args.project_y = false;
  args.project_x = true;
  LifetimeFit(args);

  args.flat_bkg = false;
  args.inhistname = "SiL3_wFlatBkgRemoval/hEnergyTime";
  args.outdirname = "SiL3_wFlatBkgRemoval";
  LifetimeFit(args);
}
