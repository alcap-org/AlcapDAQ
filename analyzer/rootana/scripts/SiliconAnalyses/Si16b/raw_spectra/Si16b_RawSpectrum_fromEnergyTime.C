#include "scripts/SiliconAnalyses/Utils/RawSpectrum_fromEnergyTime.C"

void Si16b_RawSpectrum_fromEnergyTime(std::string infilename, std::string outfilename) {

  RawSpectrum_fromEnergyTimeArgs args;
  args.infilename = infilename;
  args.inhistname = "SiL3/hEnergyTime";
  
  args.outfilename = outfilename;

  double start_time = 1000;
  double end_time = 5000;
  double time_step = 4000;
  std::stringstream time_slice;

  for (double min_time = start_time; min_time < end_time; min_time += time_step) {
    double max_time = min_time + time_step;

    args.min_time = min_time;
    args.max_time = max_time;

    time_slice.str("");
    time_slice << "TimeSlice" << (int)args.min_time << "_" << (int)args.max_time;  
    args.outdirname = "SiL3_" + time_slice.str();
    
    RawSpectrum_fromEnergyTime(args);
  }

  // Do the flat bkg
  args.inhistname = "SiL3_FlatBkg/hEnergyTime";
  args.min_time = -15000;
  args.max_time = -10000;
  args.outdirname = "RawSpectrum_fromEnergyTime_FlatBkg";
  RawSpectrum_fromEnergyTime(args);
}
