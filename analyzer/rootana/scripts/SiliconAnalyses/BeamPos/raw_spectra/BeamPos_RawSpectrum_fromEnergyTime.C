#include "scripts/SiliconAnalyses/Utils/RawSpectrum_fromEnergyTime.C"

void BeamPos_RawSpectrum_fromEnergyTime(std::string infilename, std::string outfilename) {

  RawSpectrum_fromEnergyTimeArgs args;
  args.infilename = infilename;  
  args.outfilename = outfilename;
  args.rebin_factor = 1;

  double min_time = -200;
  double max_time = 200;

  args.min_time = min_time;
  args.max_time = max_time;

  std::stringstream time_slice;
  time_slice.str("");
  time_slice << "TimeSlice" << (int)args.min_time << "_" << (int)args.max_time;

  args.inhistname = "Target_Thin_All/hEnergyTime";
  args.outdirname = "Thin_All_" + time_slice.str();  
  RawSpectrum_fromEnergyTime(args);

  args.inhistname = "SiT_All/hEnergyTime";
  args.outdirname = "SiT_All_" + time_slice.str();  
  RawSpectrum_fromEnergyTime(args);
}
