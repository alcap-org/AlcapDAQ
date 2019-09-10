#include "../..//Utils/Unfold_ResponseMatrix.C"

void Si16b_Unfold_ResponseMatrix(std::string infilename, std::string outfilename, std::string datahistname, std::string mcfilename, std::string mcresponsename, int rebin_factor, std::string outdirname) {

  //  gROOT->ProcessLine(".L scripts/Unfolding/bayesian-unfolding/libRooUnfold.so");

  Unfold_ResponseMatrixArgs args;
  args.datafilename = infilename;
  args.outfilename = outfilename;

  args.datahistname = datahistname;
  args.mcfilename = mcfilename;
  args.mcresponsename = mcresponsename;
  args.rebin_factor = rebin_factor;
  args.outdirname = outdirname;
  args.method = "bayes";
  //  args.method = "svd";
  Unfold_ResponseMatrix(args);
}
