#include "scripts/SiliconAnalyses/Utils/Unfold_TimeCut.C"

void SiL3_Unfold_TimeCut(std::string infilename, std::string outfilename, double min_time_slice, double max_time_slice) {

  std::stringstream time_slice_str;
  time_slice_str << "TimeSlice" << min_time_slice << "_" << max_time_slice;

  Unfold_TimeCutArgs args;
  args.infilename = infilename;
  std::string indirname = "DecayElectronCorrection_" + time_slice_str.str();
  args.inhistname = indirname + "/hCorrectedSpectrum";

  args.time_window_min = min_time_slice;
  args.time_window_max = max_time_slice;  

  args.outfilename = outfilename;
  args.outdirname = "TimeCut_" + time_slice_str.str();
  Unfold_TimeCut(args);
}
