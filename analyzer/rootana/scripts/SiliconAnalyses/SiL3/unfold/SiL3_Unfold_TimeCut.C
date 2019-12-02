#include "../../Utils/Unfold_TimeCut.C"

void SiL3_Unfold_TimeCut(std::string infilename, std::string outfilename, std::string inhistname, std::string incutfilename, std::string incuttreename, std::string outdirname, double decay_lifetime) {

  Unfold_TimeCutArgs args;
  args.infilename = infilename;
  args.inhistname = inhistname;
  args.incutfilename = incutfilename;
  args.incuttreename = incuttreename;
  
  args.decay_lifetime = decay_lifetime;

  args.outfilename = outfilename;
  args.outdirname = outdirname;
  args.min_time_cut_branch = "min_time";
  args.max_time_cut_branch = "max_time";
  args.extra_efficiency = 1;
  Unfold_TimeCut(args);
}
