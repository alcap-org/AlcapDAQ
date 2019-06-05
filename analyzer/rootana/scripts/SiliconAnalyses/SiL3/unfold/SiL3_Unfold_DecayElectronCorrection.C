#include "scripts/SiliconAnalyses/Utils/Unfold_DecayElectronCorrection.C"

void SiL3_Unfold_DecayElectronCorrection(std::string infilename, std::string inhistname, std::string corrfilename, std::string corrhistname, std::string countfilename, std::string counttreename, std::string outfilename, std::string outdirname) {

  Unfold_DecayElectronCorrectionArgs args;
  args.infilename = infilename;
  args.inhistname = inhistname;

  //  args.corrfilename = "~/data/results/Si16a_useful/plots09743.root";
  //  args.corrhistname = "hThick_DecayProds";
  //  args.corr_time_cut_min = 1000;
  //  args.corr_time_cut_max = 2000;

  args.corrfilename = corrfilename;
  args.corrhistname = corrhistname;

  args.outfilename = outfilename;
  args.outdirname = outdirname;
  
  args.countfilename = countfilename;
  args.counttreename = counttreename;
  args.capture_fraction = 0.658;
  Unfold_DecayElectronCorrection(args);
}
