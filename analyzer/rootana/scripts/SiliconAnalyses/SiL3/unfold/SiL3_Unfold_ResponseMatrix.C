#ifndef SiL3_Unfold_ResponseMatrix_
#define SiL3_Unfold_ResponseMatrix_

#include "../../Utils/Unfold_ResponseMatrix.C"

void SiL3_Unfold_ResponseMatrix(std::string infilename, std::string outfilename, std::string inhistname, std::string outdirname, std::string particle, std::string setting = "", int reg_parameter = 250) {

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
  //  args.mcfilename = "~/data/mc/SiL3/R15b_SiL3_response-matrix_100keV-bins_protons-SWSpec.root";
  //  args.mcresponsename = "Target_two_layer_response";
  //  args.mcfilename = "~/data/mc/SiL3/protonCorr_1M_Geom-P1_proton-flat.root";
  //  args.mcfilename = "~/data/mc/SiL3/protonCorr_1M_Geom-P1_proton-flat_Thresh0-1MeV.root";
  //  args.mcfilename = "~/data/mc/SiL3/" + particle + "Corr_1M_Geom-P1_" + particle + "-sw.root";
  //  args.mcfilename = "~/data/mc/SiL3/protonCorr_1M_Geom-P1-3mm_proton-flat_Thresh0-1MeV.root";
  //  args.mcfilename = "~/data/mc/SiL3/" + particle + "Corr_1M_Geom-P1_" + particle + "-flat_Thresh1-4MeV.root";
  //  args.mcfilename = "~/data/mc/SiL3/" + particle + "Corr_1M_Geom-P1_" + particle + "-flat_Thresh0-1MeV.root";
  //  args.mcfilename = "~/data/mc/SiL3/" + particle + "Corr_1M_Geom-P1_" + particle + "-flat_Thresh0-1MeV_cutObsE.root";
  //  args.mcfilename = "~/data/mc/SiL3/" + particle + "Corr_1M_Geom-P1_" + particle + "-flat_Thresh0-1MeV_noPID.root";
  //  args.mcfilename = "~/data/mc/SiL3/" + particle + "Corr_1M_Geom-P1_" + particle + "-flat_Thresh0-1MeV_missingAlpha.root";
  //  args.mcfilename = "~/data/mc/SiL3/" + particle + "Corr_1M_Geom-P1_" + particle + "-flat_Thresh0-1MeV.root";
  args.mcfilename = "~/data/mc/SiL3/" + particle + "Corr_1M_Geom-P1_" + particle + "-flat_Thresh0-1MeV_" + setting + "_MeV.root";
  args.mcresponsename = "SiL_middle_layer_response";
  args.rebin_factor = 2;
  args.reg_parameter = reg_parameter;
  args.outdirname = outdirname;
  args.method = "svd";
  //  args.method = "bayes";
  Unfold_ResponseMatrix(args);
}

#endif
