#include "scripts/SiliconAnalyses/Utils/Unfold_DecayElectronCorrection.C"

void SiL3_Unfold_DecayElectronCorrection(std::string infilename, std::string outfilename, double min_time_slice, double max_time_slice) {

  std::stringstream time_slice_str;
  time_slice_str << "TimeSlice" << min_time_slice << "_" << max_time_slice;

  Unfold_DecayElectronCorrectionArgs args;
  args.infilename = infilename;
  //  args.inhistname = "FlatBackground_" + time_slice_str.str() + "/hCorrectedSpectrum";
  args.inhistname = "RawSpectrum_fromEnergyTime_" + time_slice_str.str() + "/hRawSpectrum";

  //  args.corrfilename = "~/data/results/Si16a_useful/plots09743.root";
  //  args.corrhistname = "hThick_DecayProds";
  //  args.corr_time_cut_min = 1000;
  //  args.corr_time_cut_max = 2000;

  args.corrfilename = "~/data/mc/SiL3_active/corrections_new_1M_wRes.root";
  args.corrhistname = "hEDep_muplus";

  args.time_window_min = min_time_slice;
  args.time_window_max = max_time_slice;  
  args.geom_acceptance = 1;

  args.outfilename = outfilename;
  args.outdirname = "DecayElectronCorrection_" + time_slice_str.str();
  args.countfilename = "~/data/results/SiL3/normalisation.root";
  args.counttreename = "XRaySpectrum_GeLoGain_wTimeCut/counttree";
  Unfold_DecayElectronCorrection(args);
}
