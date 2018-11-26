#include "scripts/SiliconAnalyses/Utils/Unfold_ResponseMatrix.C"

void Si16b_Unfold_ResponseMatrix(std::string infilename, std::string outfilename) {

  //  gROOT->ProcessLine(".L scripts/Unfolding/bayesian-unfolding/libRooUnfold.so");

  Unfold_ResponseMatrixArgs args;
  args.datafilename = infilename;
  args.outfilename = outfilename;

  args.datahistname = "proton/hRawSpectrum";
  args.datacuttreename = "proton/cuttree";
  args.lifetimefilename = "~/data/results/Si16b/lifetime.root";
  args.lifetimefitfnname = "Proton_wTimeCut_SingleExp/muonic_atom_lifetime";
  //  args.mcfilename = "~/data/mc/Si16b/R15b_Si16b_response-matrix_500keV-bins_protons_new.root";
  args.mcfilename = "~/data/mc/Si16b/R15b_Si16b_response-matrix_500keV-bins_protons_new_wBugFix.root";
  args.mcresponsename = "SiR_response";
  args.rebin_factor = 50;
  args.outdirname = "proton";
  Unfold_ResponseMatrix(args);

  args.datahistname = "deuteron/hRawSpectrum";
  args.datacuttreename = "deuteron/cuttree";
  args.lifetimefilename = "~/data/results/Si16b/lifetime.root";
  args.lifetimefitfnname = "Proton_wTimeCut_SingleExp/muonic_atom_lifetime";
  //  args.mcfilename = "~/data/mc/Si16b/R15b_Si16b_response-matrix_500keV-bins_deuterons_new.root";
  args.mcfilename = "~/data/mc/Si16b/R15b_Si16b_response-matrix_500keV-bins_deuterons_new_wBugFix.root";
  args.mcresponsename = "SiR_response";
  args.rebin_factor = 50;
  args.outdirname = "deuteron";
  Unfold_ResponseMatrix(args);

  args.datahistname = "triton/hRawSpectrum";
  args.datacuttreename = "triton/cuttree";
  args.lifetimefilename = "~/data/results/Si16b/lifetime.root";
  args.lifetimefitfnname = "Proton_wTimeCut_SingleExp/muonic_atom_lifetime";
  //  args.mcfilename = "~/data/mc/Si16b/R15b_Si16b_response-matrix_500keV-bins_tritons_new.root";
  args.mcfilename = "~/data/mc/Si16b/R15b_Si16b_response-matrix_500keV-bins_tritons_new_wBugFix.root";
  args.mcresponsename = "SiR_response";
  args.rebin_factor = 50;
  args.outdirname = "triton";
  Unfold_ResponseMatrix(args);
  
  args.datahistname = "alpha/hRawSpectrum";
  args.datacuttreename = "alpha/cuttree";
  args.lifetimefilename = "~/data/results/Si16b/lifetime.root";
  args.lifetimefitfnname = "Proton_wTimeCut_SingleExp/muonic_atom_lifetime";
  //  args.mcfilename = "~/data/mc/Si16b/R15b_Si16b_response-matrix_500keV-bins_alphas_new.root";
  args.mcfilename = "~/data/mc/Si16b/R15b_Si16b_response-matrix_500keV-bins_alphas_new_wBugFix.root";
  args.mcresponsename = "SiR_response";
  args.rebin_factor = 50;
  args.outdirname = "alpha";
  Unfold_ResponseMatrix(args);
}
