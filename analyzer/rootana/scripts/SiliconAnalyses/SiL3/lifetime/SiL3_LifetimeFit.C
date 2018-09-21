#include "scripts/SiliconAnalyses/Utils/LifetimeFit.C"

void SiL3_LifetimeFit(std::string infilename, std::string outfilename) {
  LifetimeFitArgs args;
  args.infilename = infilename;
  args.outfilename = outfilename;
  args.min_fit_time = 1000;
  args.max_fit_time = 4000;
  args.rebin_factor = 10;
  args.double_exp = false;
  args.flat_bkg = false;
  args.project_y = false;
  args.project_x = true;
  //  args.inhistname = "Target/hEnergyTime";
  args.inhistname = "Target_wFlatBkgRemoval/hEnergyTime";

  args.max_energy_cut = 30000;
  double min_energy_cut = 0;
  double max_energy_cut = 5000;
  double energy_cut_step = 500;
  std::stringstream energy_cut_str;
  for (double i_min_energy_cut = min_energy_cut; i_min_energy_cut < max_energy_cut; i_min_energy_cut += energy_cut_step) {

    energy_cut_str.str("");
    energy_cut_str << i_min_energy_cut;

    args.min_energy_cut = i_min_energy_cut;
    args.outdirname = "Target_" + energy_cut_str.str() + "_30000";
    
    LifetimeFit(args);
  }
}
