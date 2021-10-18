#include "../../Utils/RawSpectrum_fromEnergyTime.C"

void SiL3_RawSpectrum_fromEnergyTime(std::string infilename, std::string outfilename) {

  RawSpectrum_fromEnergyTimeArgs args;
  args.infilename = infilename;
  double min_time = -1;
  double max_time = -1;
  double time_step = 0;
  std::stringstream time_slice;
  
  const int n_recoils = 8;
  std::string recoil_names[n_recoils] = {"noRecoil", "nuRecoil", "pRecoil", "dRecoil", "tRecoil", "aRecoil", "allRecoil", "systRecoil"};//, "lowAllRecoil", "highAllRecoil"};

  for (int i_recoil = 0; i_recoil < n_recoils; ++i_recoil) {
    
    args.inhistname = "Target_" + recoil_names[i_recoil] + "/hEnergyTime";
    args.incuttreename = "Target_" + recoil_names[i_recoil] + "/cuttree";
    //  args.inhistname = "Target_wFlatBkgRemoval/hEnergyTime";
  
    args.outfilename = outfilename;
    args.rebin_factor = 2;
  
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
      args.outdirname = "SiL3_ActiveTarget_" + time_slice.str() + "_" + recoil_names[i_recoil];  
      RawSpectrum_fromEnergyTime(args);
      
      args.min_time = -i_max_time;
      args.max_time = -i_min_time;
      args.outdirname = "SiL3_ActiveTarget_" + time_slice.str() + "_FlatBkg_" + recoil_names[i_recoil];
      RawSpectrum_fromEnergyTime(args);
    }
    
    // for analysis + full slice
    args.min_time = min_time;
    args.max_time = max_time;
    time_slice.str("");
    time_slice << "TimeSlice" << (int)args.min_time << "_" << (int)args.max_time;  
    args.outdirname = "SiL3_ActiveTarget_" + time_slice.str() + "_" + recoil_names[i_recoil];  
    RawSpectrum_fromEnergyTime(args);
    
    args.min_time = -max_time;
    args.max_time = -min_time;
    args.outdirname = "SiL3_ActiveTarget_" + time_slice.str() + "_FlatBkg_" + recoil_names[i_recoil];  
    RawSpectrum_fromEnergyTime(args);

    // for analysis + do some other big slides
    args.min_time = 2500;
    args.max_time = 4000;
    time_slice.str("");
    time_slice << "TimeSlice" << (int)args.min_time << "_" << (int)args.max_time;  
    args.outdirname = "SiL3_ActiveTarget_" + time_slice.str() + "_" + recoil_names[i_recoil];  
    RawSpectrum_fromEnergyTime(args);
    args.min_time = -4000;
    args.max_time = -2500;
    args.outdirname = "SiL3_ActiveTarget_" + time_slice.str() + "_FlatBkg_" + recoil_names[i_recoil];  
    RawSpectrum_fromEnergyTime(args);

    args.min_time = 3000;
    args.max_time = 4000;
    time_slice.str("");
    time_slice << "TimeSlice" << (int)args.min_time << "_" << (int)args.max_time;  
    args.outdirname = "SiL3_ActiveTarget_" + time_slice.str() + "_" + recoil_names[i_recoil];  
    RawSpectrum_fromEnergyTime(args);
    args.min_time = -4000;
    args.max_time = -3000;
    args.outdirname = "SiL3_ActiveTarget_" + time_slice.str() + "_FlatBkg_" + recoil_names[i_recoil];  
    RawSpectrum_fromEnergyTime(args);
  }

  args.inhistname = "Target_noRecoil/hEnergyTime";
  
  args.min_time = -10000;
  args.max_time = -6000;
  args.outdirname = "SiL3_ActiveTarget_FlatBkg";  
  RawSpectrum_fromEnergyTime(args);
  
  args.min_time = -200;
  args.max_time = 200;
  time_slice.str("");
  time_slice << "TimeSlice" << (int)args.min_time << "_" << (int)args.max_time;  
  args.outdirname = "SiL3_ActiveTarget_" + time_slice.str();
  RawSpectrum_fromEnergyTime(args);

  /*
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
*/
  
  /*
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
  */

  /*
  // Do the flat bkg
  //  args.inhistname = "FlatBkg/hEnergyTime";
  args.inhistname = "Target/hEnergyTime";
  args.min_time = -20000;
  args.max_time = -10000;
  args.outdirname = "SiL3_ActiveTarget_FlatBkg";
  RawSpectrum_fromEnergyTime(args);

  // for MIP peak
  */
}
