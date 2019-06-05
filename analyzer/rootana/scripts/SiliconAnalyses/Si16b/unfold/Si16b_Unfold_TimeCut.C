#include "scripts/SiliconAnalyses/Utils/Unfold_TimeCut.C"

void Si16b_Unfold_TimeCut(std::string infilename, std::string outfilename, std::string inhistname, std::string incutfilename, std::string incuttreename, std::string outdirname, double decay_lifetime) {

  Unfold_TimeCutArgs args;
  args.infilename = infilename;
  args.inhistname = inhistname;
  args.incutfilename = incutfilename;
  args.incuttreename = incuttreename;
  
  args.decay_lifetime = decay_lifetime;

  args.outfilename = outfilename;
  args.outdirname = outdirname;
  Unfold_TimeCut(args);
}
