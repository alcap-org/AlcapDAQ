#include "../..//Utils/Unfold_DecayElectronCorrection.C"

void Si16b_Unfold_DecayElectronCorrection(std::string infilename, std::string inhistname, std::string corrfilename, std::string corrhistname, std::string countfilename, std::string counttreename, std::string outfilename, std::string outdirname) {

  Unfold_DecayElectronCorrectionArgs args;
  args.infilename = infilename;
  args.inhistname = inhistname;

  //  args.corrfilename = "~/data/results/Si16a_useful/plots09743.root";
  //  args.corrhistname = "hThick_DecayProds";
  //  args.corr_time_cut_min = 1000;
  //  args.corr_time_cut_max = 2000;

  //  args.corrfilename = "~/data/mc/Si16b/R15b_Si16b-InclusiveAnalysis_MCCorrections_1M.root";
  //  args.corrhistname = "hEDep_muplus";

  args.corrfilename = corrfilename;
  args.corrhistname = corrhistname;

  //  args.time_window_min = 1000;
  //  args.time_window_max = 5000;  
  //  args.geom_acceptance = 0.01002;//1.0;//0.012572;//7.7176e-3;

  args.outfilename = outfilename;
  args.outdirname = outdirname;
  args.countfilename = countfilename;
  args.counttreename = counttreename;
  args.capture_fraction = 0.658;
  Unfold_DecayElectronCorrection(args);
}
