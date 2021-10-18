#ifndef Ti50_Unfold_EscapeCorrection_
#define Ti50_Unfold_EscapeCorrection_

#include "../../Utils/Unfold_EscapeCorrection.C"

void Ti50_Unfold_EscapeCorrection(std::string infilename, std::string outfilename, std::string inhistname, std::string outdirname, double min_E, double max_E, std::string particle, double bin_width = 1, std::string arm = "SiR") {

  //  gROOT->ProcessLine(".L scripts/Unfolding/bayesian-unfolding/libRooUnfold.so");

  Unfold_EscapeCorrectionArgs args;
  args.datafilename = infilename;
  args.outfilename = outfilename;
  args.datahistname = inhistname;
  args.hist_input = true;
  args.mcfilename = "/data/R15bMC/respMatrix_100M_Ti50_" + particle + "_100keVBins.root";
  //  args.mcfilename = "~/g4sim/alcap/respMatrix_100M_Ti50_" + particle + "_100keVBins_max20MeV.root";
  if (particle == "proton" && arm=="SiR") {
    args.mcresponsename = arm + "_three_layer_response";
  }
  else {
    args.mcresponsename = arm + "_two_layer_response";
  }
  args.min_E = min_E;
  args.max_E = max_E;
  args.bin_width = bin_width;
  args.outdirname = outdirname;
  Unfold_EscapeCorrection(args);
}

#endif
