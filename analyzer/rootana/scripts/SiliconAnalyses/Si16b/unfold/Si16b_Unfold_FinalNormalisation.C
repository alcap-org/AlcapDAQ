#include "scripts/SiliconAnalyses/Utils/Unfold_FinalNormalisation.C"

void Si16b_Unfold_FinalNormalisation(std::string infilename, std::string inhistname, std::string countfilename, std::string counttreename, std::string outfilename, std::string outdirname) {

  Unfold_FinalNormalisationArgs args;
  args.infilename = infilename;
  args.inhistname = inhistname;


  args.outfilename = outfilename;
  args.outdirname = outdirname;
  
  args.countfilename = countfilename;
  args.counttreename = counttreename;
  args.capture_fraction = 0.658;

  args.rebin_factor = 1;
  Unfold_FinalNormalisation(args);
}
