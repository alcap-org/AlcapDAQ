#include "../../Utils/Unfold_PIDCutEfficiency.C"

void Si16b_Unfold_PIDCutEfficiency(std::string infilename, std::string outfilename, std::string inhistname, std::string outdirname, std::string corrfilename, std::string particle, int rebin_factor) {

  Unfold_PIDCutEfficiencyArgs args;
  args.infilename = infilename;
  args.inhistname = inhistname;

  args.corrfilename = corrfilename;
  args.particle = particle;
  args.rebin_factor = rebin_factor;
  
  args.outfilename = outfilename;
  args.outdirname = outdirname;
  Unfold_PIDCutEfficiency(args);
}
