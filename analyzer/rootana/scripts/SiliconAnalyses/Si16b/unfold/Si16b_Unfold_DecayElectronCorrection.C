#include "scripts/SiliconAnalyses/Utils/Unfold_DecayElectronCorrection.C"

void Si16b_Unfold_DecayElectronCorrection(std::string infilename, std::string outfilename) {

  Unfold_DecayElectronCorrectionArgs args;
  args.infilename = infilename;
  std::string indirname = "SiL3_wFlatBkgRemoval_TimeSlice1000_5000";
  args.inhistname = indirname + "/hRawSpectrum";

  //  args.corrfilename = "~/data/results/Si16a_useful/plots09743.root";
  //  args.corrhistname = "hThick_DecayProds";
  //  args.corr_time_cut_min = 1000;
  //  args.corr_time_cut_max = 2000;

  args.corrfilename = "~/data/mc/Si16b/R15b_Si16b-InclusiveAnalysis_MCCorrections_1M.root";
  args.corrhistname = "hEDep_muplus";

  args.time_window_min = 1000;
  args.time_window_max = 5000;  
  args.geom_acceptance = 1.0;//0.012572;//7.7176e-3;

  args.outfilename = outfilename;
  args.outdirname = "DecayElectronCorrection";
  args.countfilename = "~/data/results/Si16b/normalisation.root";
  args.counttreename = "XRaySpectrum_GeLoGain_wTimeCut/counttree";
  Unfold_DecayElectronCorrection(args);
}
