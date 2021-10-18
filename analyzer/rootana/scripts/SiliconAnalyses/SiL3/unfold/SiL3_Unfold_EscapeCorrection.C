#ifndef SiL3_Unfold_EscapeCorrection_
#define SiL3_Unfold_EscapeCorrection_

#include "../../Utils/Unfold_EscapeCorrection.C"

void SiL3_Unfold_EscapeCorrection(std::string infilename, std::string outfilename, std::string inhistname, std::string outdirname, std::string particle, std::string mustop_position, double min_E, double max_E, double bin_width) {

  //  gROOT->ProcessLine(".L scripts/Unfolding/bayesian-unfolding/libRooUnfold.so");

  Unfold_EscapeCorrectionArgs args;
  args.datafilename = infilename;
  args.outfilename = outfilename;
  args.datahistname = inhistname;
  args.mcfilename = "~/data/mc/SiL3/" + particle + "Corr_1M_Geom-P1_" + particle + "-flat_Thresh0-1MeV_" + mustop_position + "_MeV.root";
  args.mcresponsename = "SiL_middle_layer_response";
  args.min_E = min_E;
  args.max_E = max_E;
  //  args.rebin_factor = rebin_factor;
  args.bin_width = bin_width;
  args.hist_input = false;
  args.outdirname = outdirname;
  Unfold_EscapeCorrection(args);
}

#endif
