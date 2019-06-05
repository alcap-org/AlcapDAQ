#include "scripts/SiliconAnalyses/Utils/RawSpectrum_fromEnergyTime.C"

void SiL3_RawSpectrum_fromEnergyTime(std::string infilename, std::string outfilename) {

  RawSpectrum_fromEnergyTimeArgs args;
  args.infilename = infilename;
  args.inhistname = "Target/hEnergyTime";
  //  args.inhistname = "Target_wFlatBkgRemoval/hEnergyTime";
  
  args.outfilename = outfilename;
  args.rebin_factor = 2;
  
  // For diagnostics
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
    args.outdirname = "SiL3_ActiveTarget_" + time_slice.str();  
    RawSpectrum_fromEnergyTime(args);

    args.min_time = -i_max_time;
    args.max_time = -i_min_time;
    args.outdirname = "SiL3_ActiveTarget_" + time_slice.str() + "_FlatBkg";
    RawSpectrum_fromEnergyTime(args);
  }

  // for analysis
  min_time = 2000;
  max_time = 4000;
  time_step = 500;
  for (double i_min_time = min_time; i_min_time < max_time; i_min_time += time_step) {
    double i_max_time = i_min_time+time_step;

    args.min_time = i_min_time;
    args.max_time = i_max_time;

    time_slice.str("");
    time_slice << "TimeSlice" << (int)args.min_time << "_" << (int)args.max_time;  
    args.outdirname = "SiL3_ActiveTarget_" + time_slice.str();  
    RawSpectrum_fromEnergyTime(args);

    args.min_time = -i_max_time;
    args.max_time = -i_min_time;
    args.outdirname = "SiL3_ActiveTarget_" + time_slice.str() + "_FlatBkg";
    RawSpectrum_fromEnergyTime(args);
  }
  // for analysis + full slice
  args.min_time = min_time;
  args.max_time = max_time;
  time_slice.str("");
  time_slice << "TimeSlice" << (int)args.min_time << "_" << (int)args.max_time;  
  args.outdirname = "SiL3_ActiveTarget_" + time_slice.str();  
  RawSpectrum_fromEnergyTime(args);

  args.min_time = -max_time;
  args.max_time = -min_time;
  args.outdirname = "SiL3_ActiveTarget_" + time_slice.str() + "_FlatBkg";  
  RawSpectrum_fromEnergyTime(args);

  args.min_time = 6000;
  args.max_time = 20000;
  time_slice.str("");
  time_slice << "TimeSlice" << (int)args.min_time << "_" << (int)args.max_time;  
  args.outdirname = "SiL3_ActiveTarget_" + time_slice.str();  
  RawSpectrum_fromEnergyTime(args);

  args.min_time = -20000;
  args.max_time = -6000;
  args.outdirname = "SiL3_ActiveTarget_" + time_slice.str() + "_FlatBkg";  
  RawSpectrum_fromEnergyTime(args);

  // Do the flat bkg
  //  args.inhistname = "FlatBkg/hEnergyTime";
  args.inhistname = "Target/hEnergyTime";
  args.min_time = -20000;
  args.max_time = -10000;
  args.outdirname = "SiL3_ActiveTarget_FlatBkg";
  RawSpectrum_fromEnergyTime(args);

  // for MIP peak
  args.inhistname = "Target/hEnergyTime";
  args.min_time = -200;
  args.max_time = 200;
  time_slice.str("");
  time_slice << "TimeSlice" << (int)args.min_time << "_" << (int)args.max_time;  
  args.outdirname = "SiL3_ActiveTarget_" + time_slice.str();
  RawSpectrum_fromEnergyTime(args);
}
