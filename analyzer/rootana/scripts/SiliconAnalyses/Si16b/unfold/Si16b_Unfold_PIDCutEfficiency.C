#include "scripts/SiliconAnalyses/Utils/Unfold_PIDCutEfficiency.C"

void Si16b_Unfold_PIDCutEfficiency(std::string infilename, std::string outfilename, std::string inhistname, std::string outdirname, double efficiency) {

  Unfold_PIDCutEfficiencyArgs args;
  args.infilename = infilename;
  args.inhistname = inhistname;

  args.efficiency = efficiency;
  
  args.outfilename = outfilename;
  args.outdirname = outdirname;
  Unfold_PIDCutEfficiency(args);
}
