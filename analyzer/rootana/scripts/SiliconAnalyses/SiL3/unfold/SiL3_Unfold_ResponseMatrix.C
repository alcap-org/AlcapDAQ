#include "scripts/SiliconAnalyses/Utils/Unfold_ResponseMatrix.C"

void SiL3_Unfold_ResponseMatrix(std::string infilename, std::string outfilename, std::string inhistname, std::string outdirname) {

  //  gROOT->ProcessLine(".L scripts/Unfolding/bayesian-unfolding/libRooUnfold.so");

  Unfold_ResponseMatrixArgs args;
  args.datafilename = infilename;
  args.outfilename = outfilename;
  args.datahistname = inhistname;
  //  args.mcfilename = "~/data/mc/SiL3/corrections_wResponseMatrix.root";
  //  args.mcfilename = "~/data/mc/SiL3/corrections_wResponseMatrix-5MeV--30MeV.root";
  //  args.mcfilename = "~/data/mc/SiL3/corrections_wResponseMatrix_500keVBins.root";
  //  args.mcresponsename = "SiL3_response";
  //  args.mcfilename = "~/data/mc/SiL3/R15b_SiL3_response-matrix_50keV-bins_protons-SWSpec.root";
  args.mcfilename = "~/data/mc/SiL3/R15b_SiL3_response-matrix_100keV-bins_protons-SWSpec.root";
  args.mcresponsename = "Target_two_layer_response";
  args.rebin_factor = 2;
  args.outdirname = outdirname;
  Unfold_ResponseMatrix(args);
}
