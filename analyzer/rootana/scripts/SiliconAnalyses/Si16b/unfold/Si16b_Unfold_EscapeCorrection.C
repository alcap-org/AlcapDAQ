#ifndef Si16b_Unfold_EscapeCorrection_
#define Si16b_Unfold_EscapeCorrection_

#include "../../Utils/Unfold_EscapeCorrection.C"

void Si16b_Unfold_EscapeCorrection(std::string infilename, std::string outfilename, std::string inhistname, std::string outdirname, double min_E, double max_E, std::string particle, double bin_width_val, std::string arm) {

  //  gROOT->ProcessLine(".L scripts/Unfolding/bayesian-unfolding/libRooUnfold.so");

  Unfold_EscapeCorrectionArgs args;
  args.datafilename = infilename;
  args.outfilename = outfilename;
  args.datahistname = inhistname;
  args.hist_input = true;
  //  args.mcfilename = "~/data/mc/Si16b/respMatrix_100M_Geom-P5_" + particle + "_100keVBins.root";
  args.mcfilename = "/data/R15bMC/respMatrix_100M_Si16b_" + particle + "_100keVBins.root";
  args.mcresponsename = arm + "_two_layer_response";
  if (particle == "proton" && arm == "SiR") {
    args.mcresponsename = arm + "_three_layer_response";
  }
  args.min_E = min_E;
  args.max_E = max_E;
  //  args.rebin_factor = rebin_factor;
  args.bin_width = bin_width_val;
  args.outdirname = outdirname;
  Unfold_EscapeCorrection(args);
}

#endif
