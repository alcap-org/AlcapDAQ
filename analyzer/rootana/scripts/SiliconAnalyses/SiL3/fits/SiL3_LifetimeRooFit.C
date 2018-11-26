#include "scripts/SiliconAnalyses/Utils/LifetimeRooFit.C"

void SiL3_LifetimeRooFit(std::string infilename, std::string outfilename) {

  LifetimeRooFitArgs args;
  args.infilename = infilename;
  args.outfilename = outfilename;

  
  //  args.time_min = -200;
  //  args.time_max = 1000;
  //  args.time_min = 1000;
  //  args.time_max = 4000;
  //  args.time_min = 8000;
  //  args.time_max = 10000;
  args.time_min = -1000;
  args.time_max = 20000;

  args.landau_mean_min = 0;
  args.landau_mean_max = 1000;
  args.landau_sigma_min = 0;
  args.landau_sigma_max = 500;
  args.resolution_sigma_min = 0;
  args.resolution_sigma_max = 200;

  args.lifetime_min = 10;
  args.lifetime_max = 1000;
  
  //  args.flat_bkg_min = 0;
  //  args.flat_bkg_max = 10000;

  args.rebin_factor = 4;

  args.inhistname = "Target/hEnergyTime";
  args.outdirname = "LifetimeRooFit";
  LifetimeRooFit(args);
}
