#include "scripts/SiliconAnalyses/Utils/FlatBkgFit.C"

void SiL3_FlatBkgFits(std::string infilename, std::string outfilename) {

  FlatBkgFitArgs args;
  args.infilename = infilename;
  args.outfilename = outfilename;

  
  args.edep_min = 200;
  args.edep_max = 5000;

  args.landau_mean_min = 200;
  args.landau_mean_max = 400;
  args.landau_sigma_min = 0;
  args.landau_sigma_max = 500;
  args.resolution_sigma_min = 31;
  args.resolution_sigma_max = 31;

  args.inhistname = "RawSpectrum_fromEnergyTime_FlatBkg/hRawSpectrum";
  args.outdirname = "FlatBkgFit";
  FlatBkgFit(args);
}
