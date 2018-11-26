#include "scripts/SiliconAnalyses/Utils/RawSpectrum_fromEnergyTime.C"

void SiL3_RawSpectrum_fromEnergyTime(std::string infilename, std::string outfilename) {

  RawSpectrum_fromEnergyTimeArgs args;
  args.infilename = infilename;
  args.inhistname = "Target/hEnergyTime";
  //  args.inhistname = "Target_wFlatBkgRemoval/hEnergyTime";
  
  args.outfilename = outfilename;

  double min_time = 1000;
  double max_time = 5000;
  double time_step = 100;
  
  std::stringstream time_slice;
  for (double i_min_time = min_time; i_min_time < max_time; i_min_time += time_step) {
    double i_max_time = i_min_time+time_step;

    args.min_time = i_min_time;
    args.max_time = i_max_time;

    time_slice.str("");
    time_slice << "TimeSlice" << (int)args.min_time << "_" << (int)args.max_time;  
    args.outdirname = "RawSpectrum_fromEnergyTime_" + time_slice.str();  

    RawSpectrum_fromEnergyTime(args);
  }

  // Do the full time slice
  args.min_time = min_time;
  args.max_time = max_time;
  time_slice.str("");
  time_slice << "TimeSlice" << (int)args.min_time << "_" << (int)args.max_time;  
  args.outdirname = "RawSpectrum_fromEnergyTime_" + time_slice.str();  
  
  RawSpectrum_fromEnergyTime(args);


  // Do the flat bkg
  args.inhistname = "FlatBkg/hEnergyTime";
  args.min_time = -15000;
  args.max_time = -10000;
  args.outdirname = "RawSpectrum_fromEnergyTime_FlatBkg";
  RawSpectrum_fromEnergyTime(args);
}
