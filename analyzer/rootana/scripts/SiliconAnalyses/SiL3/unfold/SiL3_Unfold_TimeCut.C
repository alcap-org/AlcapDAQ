#include "scripts/SiliconAnalyses/Utils/Unfold_TimeCut.C"

void SiL3_Unfold_TimeCut(std::string infilename, std::string outfilename, std::string inhistname, std::string incutfilename, std::string incuttreename, std::string outdirname) {

  Unfold_TimeCutArgs args;
  args.infilename = infilename;
  args.inhistname = inhistname;
  args.incutfilename = incutfilename;
  args.incuttreename = incuttreename;
  
  args.decay_lifetime = 756;

  args.outfilename = outfilename;
  args.outdirname = outdirname;
  Unfold_TimeCut(args);
}
