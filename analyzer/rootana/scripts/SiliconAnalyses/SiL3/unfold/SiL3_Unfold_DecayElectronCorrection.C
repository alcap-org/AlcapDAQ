#include "scripts/SiliconAnalyses/Utils/Unfold_DecayElectronCorrection.C"

void SiL3_Unfold_DecayElectronCorrection(std::string infilename, std::string inhistname, std::string outfilename, std::string outdirname) {

  Unfold_DecayElectronCorrectionArgs args;
  args.infilename = infilename;
  args.inhistname = inhistname;
  //  args.inhistname = "RawSpectrum_fromEnergyTime_" + time_slice_str.str() + "/hRawSpectrum";

  //  args.corrfilename = "~/data/results/Si16a_useful/plots09743.root";
  //  args.corrhistname = "hThick_DecayProds";
  //  args.corr_time_cut_min = 1000;
  //  args.corr_time_cut_max = 2000;

  //  args.corrfilename = "~/data/mc/SiL3/corrections_new_1M_wRes.root";
  args.corrfilename = "~/data/mc/SiL3/corrections_1M_wRes_wThresh_50keVBins.root";
  //  args.corrfilename = "~/data/mc/SiL3/corrections_1M_wRes_wThresh_wKFactor_50keVBins.root";
  //  args.corrfilename = "~/data/mc/SiL3/corrections_1M_wRes_wThresh_wKFactor1-2_50keVBins.root";
  //  args.corrfilename = "~/data/mc/SiL3/corrections_1M_wRes_wThresh_wKFactor0-9_50keVBins.root";
  args.corrhistname = "hEDep_muplus";

  double min_time_slice, max_time_slice;
  args.time_window_min = min_time_slice;
  args.time_window_max = max_time_slice;  
  args.geom_acceptance = 1;

  args.outfilename = outfilename;
  args.outdirname = outdirname;
  
  args.countfilename = "~/data/results/SiL3/normalisation.root";
  args.counttreename = "XRaySpectrum_GeLoGain_wTimeCut/counttree";
  Unfold_DecayElectronCorrection(args);
}
