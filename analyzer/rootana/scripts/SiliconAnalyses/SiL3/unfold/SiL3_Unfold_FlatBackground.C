#include "scripts/SiliconAnalyses/Utils/Unfold_FlatBackground.C"

void SiL3_Unfold_FlatBackground(std::string infilename, std::string outfilename, double min_time_slice, double max_time_slice) {

  std::stringstream time_slice_str;
  time_slice_str << "TimeSlice" << min_time_slice << "_" << max_time_slice;

  Unfold_FlatBackgroundArgs args;
  args.infilename = infilename;
  std::string indirname = "RawSpectrum_fromEnergyTime_" + time_slice_str.str();
  args.inhistname = indirname + "/hRawSpectrum";
  args.intreename = indirname + "/cuttree";
  
  args.corrfilename = args.infilename;
  args.corrhistname = "RawSpectrum_fromEnergyTime_FlatBkg/hRawSpectrum";
  args.corrtreename = "RawSpectrum_fromEnergyTime_FlatBkg/cuttree";

  args.outfilename = outfilename;
  args.outdirname = "FlatBackground_" + time_slice_str.str();
  Unfold_FlatBackground(args);
}
