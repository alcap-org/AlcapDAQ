#include "scripts/SiliconAnalyses/Utils/RawSpectrum_fromEnergyTime.C"

void Si16b_RawSpectrum_fromEnergyTime(std::string infilename, std::string outfilename) {

  RawSpectrum_fromEnergyTimeArgs args;
  args.infilename = infilename;
  args.outfilename = outfilename;

  args.rebin_factor = 2;


  const int n_time_slices = 3;
  double min_time_slices[n_time_slices] = {1000, 2000, 1000};
  double max_time_slices[n_time_slices] = {2000, 5000, 5000};
  std::stringstream time_slice;

  const int n_detectors = 2;
  std::string detectors[n_detectors] = {"SiL3", "SiR2"};

  for (int i_detector = 0; i_detector < n_detectors; ++i_detector) {
    std::string detector = detectors[i_detector];

    for (int i_time_slice = 0; i_time_slice < n_time_slices; ++i_time_slice) {
      args.inhistname = detector + "/hEnergyTime";
      double min_time = min_time_slices[i_time_slice];
      double max_time = max_time_slices[i_time_slice];
      
      args.min_time = min_time;
      args.max_time = max_time;
      
      time_slice.str("");
      time_slice << "TimeSlice" << (int)args.min_time << "_" << (int)args.max_time;  
      args.outdirname = detector + "_" + time_slice.str();
      RawSpectrum_fromEnergyTime(args);
      
      args.min_time = -max_time;
      args.max_time = -min_time;
      args.outdirname = detector + "_" + time_slice.str() + "_FlatBkg";
      RawSpectrum_fromEnergyTime(args);

      if (detector == "SiR2") {
	args.min_time = min_time;
	args.max_time = max_time;
	args.inhistname = "SiR2_Michels/hEnergyTime";
	args.outdirname = "SiR2_Michels_" + time_slice.str();
	RawSpectrum_fromEnergyTime(args);
      }
    }

    // Do the flat bkg
    args.inhistname = detector + "/hEnergyTime";
    args.min_time = -20000;
    args.max_time = -500;
    args.outdirname = detector + "_FlatBkg";
    RawSpectrum_fromEnergyTime(args);

    if (detector == "SiR2") {
      args.inhistname = "SiR2_Michels/hEnergyTime";
      args.min_time = -20000;
      args.max_time = -500;
      args.outdirname = "SiR2_Michels_FlatBkg";
      RawSpectrum_fromEnergyTime(args);
    }
  }

  // Here is the target
  args.inhistname = "Target/hEnergyTime";
  args.min_time = -300;
  args.max_time = 300;
  args.outdirname = "Target";
  RawSpectrum_fromEnergyTime(args);
}
