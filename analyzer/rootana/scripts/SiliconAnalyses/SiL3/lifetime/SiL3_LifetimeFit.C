#include "../../Utils/LifetimeFit.C"

void SiL3_LifetimeFit(std::string infilename, std::string outfilename) {
  LifetimeFitArgs args;
  args.infilename = infilename;
  args.outfilename = outfilename;
  args.rebin_factor = 5;
  args.double_exp = false;
  args.project_y = false;
  args.project_x = true;
  args.include_resolution = false;

  args.datahistnames.clear();
  args.scale_ratios.clear();
  std::string inhistname = "Target_noRecoil/hEnergyTime";
  double scale_ratio = 1;
  args.datahistnames.push_back(inhistname);
  args.scale_ratios.push_back(scale_ratio);
  
  //  args.inhistname = "Target_wFlatBkgRemoval/hEnergyTime";

  double pp_window = 20000;
  double start_fit = 2000;
  double start_ignore = 4000;
  double end_ignore = 12000;
  
  //  double pp_window = 10000;
  //  double start_fit = 2000;
  //  double start_ignore = 4000;
  //  double end_ignore = 8000;
  
  args.max_energy_cut = -1;
  double min_energy_cut = 0;
  double max_energy_cut = 30500;
  double energy_cut_step = 500;
  std::stringstream energy_cut_str;
  for (double i_min_energy_cut = min_energy_cut; i_min_energy_cut < max_energy_cut; i_min_energy_cut += energy_cut_step) {

    energy_cut_str.str("");
    energy_cut_str << i_min_energy_cut;

    args.min_energy_cut = i_min_energy_cut;
    //    args.max_energy_cut = i_min_energy_cut + energy_cut_step;

    args.min_fit_time = start_fit;
    //    args.max_fit_time = 3750;
    //    args.ignore_region_min = 0;
    //    args.ignore_region_max = 0;
    args.max_fit_time = pp_window - 1000;
    args.ignore_region_min = start_ignore;
    args.ignore_region_max = end_ignore;
    args.double_exp = false;
    args.flat_bkg = false;
    args.outdirname = "Target_" + energy_cut_str.str() + "keVCut";
    LifetimeFit(args);

    args.double_exp = false;
    args.flat_bkg = true;
    // 10us PP
    //    args.min_flat_fit_time = -2500;
    //    args.max_flat_fit_time = -1000;
    //    args.ignore_region_flat_min = 0;
    //    args.ignore_region_flat_max = 0;
    // 20us PP
    args.min_flat_fit_time = -10000;
    args.max_flat_fit_time = 10000;
    args.ignore_region_flat_min = -1000;
    args.ignore_region_flat_max = 10000;//12000;
    
    args.min_fit_time = start_fit;
    //    args.max_fit_time = 4000;
    //    args.ignore_region_min = 0;
    //    args.ignore_region_max = 0;
    args.max_fit_time = pp_window - 1000;
    args.ignore_region_min = start_ignore;
    args.ignore_region_max = end_ignore;//6000;
    
    args.outdirname = "Target_" + energy_cut_str.str() + "keVCut_wFlatBkg";
    LifetimeFit(args);
  }
}
