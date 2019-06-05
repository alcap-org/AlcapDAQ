#include "scripts/SiliconAnalyses/Utils/RawSpectrum_fromEnergyTime.C"

void Ti50_RawSpectrum_fromEnergyTime(std::string infilename, std::string outfilename) {

  RawSpectrum_fromEnergyTimeArgs args;
  args.infilename = infilename;
  args.inhistname = "SiL3/hEnergyTime";
  
  args.outfilename = outfilename;

  args.rebin_factor = 1;

  const int n_time_slices = 7;
  double min_time_slices[n_time_slices] = {-200, 200, 1000, 2000, 3000, 4000, 1000};
  double max_time_slices[n_time_slices] = {200, 400, 2000, 3000, 4000, 5000, 5000};

  std::stringstream time_slice;

  for (int i_time_slice = 0; i_time_slice < n_time_slices; ++i_time_slice) {
    double min_time = min_time_slices[i_time_slice];
    double max_time = max_time_slices[i_time_slice];

    args.min_time = min_time;
    args.max_time = max_time;

    time_slice.str("");
    time_slice << "TimeSlice" << (int)args.min_time << "_" << (int)args.max_time;  
    args.outdirname = "SiL3_" + time_slice.str();
    RawSpectrum_fromEnergyTime(args);

    args.inhistname = "SiL3_Michels/hEnergyTime";
    args.outdirname = "SiL3_Michels_" + time_slice.str();
    RawSpectrum_fromEnergyTime(args);
  }

  // Do the flat bkg
  args.inhistname = "SiL3/hEnergyTime";
  args.min_time = -20000;
  args.max_time = -500;
  args.outdirname = "SiL3_FlatBkg";
  RawSpectrum_fromEnergyTime(args);

  args.inhistname = "SiL3_Michels/hEnergyTime";
  args.outdirname = "SiL3_Michels_FlatBkg";
  RawSpectrum_fromEnergyTime(args);
  
  // Now do SiR2
  for (int i_time_slice = 0; i_time_slice < n_time_slices; ++i_time_slice) {
    double min_time = min_time_slices[i_time_slice];
    double max_time = max_time_slices[i_time_slice];

    args.min_time = min_time;
    args.max_time = max_time;

    time_slice.str("");
    time_slice << "TimeSlice" << (int)args.min_time << "_" << (int)args.max_time;

    args.inhistname = "SiR2/hEnergyTime";
    args.outdirname = "SiR2_" + time_slice.str();
    RawSpectrum_fromEnergyTime(args);

    args.inhistname = "SiR2_Michels/hEnergyTime";
    args.outdirname = "SiR2_Michels_" + time_slice.str();
    RawSpectrum_fromEnergyTime(args);
  }

  args.inhistname = "SiR2/hEnergyTime";
  args.min_time = -20000;
  args.max_time = -500;
  args.outdirname = "SiR2_FlatBkg";
  RawSpectrum_fromEnergyTime(args);

  //  args.inhistname = "SiR2_Michels/hEnergyTime";
  //  args.min_time = -20000;
  //  args.max_time = -500;
  //  args.outdirname = "SiR2_Michels_FlatBkg";
  //  RawSpectrum_fromEnergyTime(args);
}
