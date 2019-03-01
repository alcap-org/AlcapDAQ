#include "scripts/SiliconAnalyses/Utils/Unfold_GeometricAcceptance.C"

void SiL3_Unfold_GeometricAcceptance(std::string infilename, std::string outfilename, std::string inhistname, std::string outdirname, double geom_acceptance) {

  Unfold_GeometricAcceptanceArgs args;
  args.infilename = infilename;
  args.inhistname = inhistname;
  
  args.geom_acceptance = geom_acceptance;

  args.outfilename = outfilename;
  args.outdirname = outdirname;
  Unfold_GeometricAcceptance(args);
}
