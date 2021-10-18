#ifndef Si16b_Unfold_ResponseMatrix_
#define Si16b_Unfold_ResponseMatrix_

#include "../..//Utils/Unfold_ResponseMatrix.C"

void Si16b_Unfold_ResponseMatrix(std::string infilename, std::string outfilename, std::string datahistname, std::string mcfilename, std::string mcresponsename, double bin_width_val, /*int rebin_factor,*/ std::string outdirname, double highE_cut = 10000, /*double n_gen_events,*/ std::string method = "bayes", int reg_parameter = 4) {

  //  gROOT->ProcessLine(".L scripts/Unfolding/bayesian-unfolding/libRooUnfold.so");

  Unfold_ResponseMatrixArgs args;
  args.datafilename = infilename;
  args.outfilename = outfilename;

  args.datahistname = datahistname;
  args.mcfilename = mcfilename;
  args.mcresponsename = mcresponsename;
  //  args.rebin_factor = rebin_factor;
  args.bin_width = bin_width_val;
  args.outdirname = outdirname;
  args.method = method;
  args.lowE_cut = -1;//4;
  args.highE_cut = highE_cut;//10000;//21;
  //  args.n_gen_events = n_gen_events;
  //  args.method = "svd";
  args.reg_parameter = reg_parameter;
  Unfold_ResponseMatrix(args);
}

#endif
