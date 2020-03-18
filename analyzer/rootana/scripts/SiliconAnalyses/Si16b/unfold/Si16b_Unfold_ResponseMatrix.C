#ifndef Si16b_Unfold_ResponseMatrix_
#define Si16b_Unfold_ResponseMatrix_

#include "../..//Utils/Unfold_ResponseMatrix.C"

void Si16b_Unfold_ResponseMatrix(std::string infilename, std::string outfilename, std::string datahistname, std::string mcfilename, std::string mcresponsename, int rebin_factor, std::string outdirname, std::string method = "bayes", int reg_parameter = 4) {

  //  gROOT->ProcessLine(".L scripts/Unfolding/bayesian-unfolding/libRooUnfold.so");

  Unfold_ResponseMatrixArgs args;
  args.datafilename = infilename;
  args.outfilename = outfilename;

  args.datahistname = datahistname;
  args.mcfilename = mcfilename;
  args.mcresponsename = mcresponsename;
  args.rebin_factor = rebin_factor;
  args.outdirname = outdirname;
  args.method = method;
  //  args.method = "svd";
  args.reg_parameter = reg_parameter;
  Unfold_ResponseMatrix(args);
}

#endif
