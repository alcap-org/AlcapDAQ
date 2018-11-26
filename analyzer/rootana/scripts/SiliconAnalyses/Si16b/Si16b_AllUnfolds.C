#include "scripts/SiliconAnalyses/Si16b/unfold/Si16b_Unfold_ResponseMatrix.C"
#include "scripts/SiliconAnalyses/Si16b/unfold/Si16b_Unfold_FlatBackground.C"
#include "scripts/SiliconAnalyses/Si16b/unfold/Si16b_Unfold_DecayElectronCorrection.C"

void Si16b_AllUnfolds() {

  //  gROOT->ProcessLine(".L scripts/Unfolding/bayesian-unfolding/libRooUnfold.so");

  std::string infilename = "~/data/results/Si16b/raw_spectra.root";
  std::string outfilename = "~/data/results/Si16b/unfold.root";


  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  // For protons, deuterons etc
  Si16b_Unfold_ResponseMatrix(infilename, outfilename);

  // For inclusive analysis
  Si16b_Unfold_FlatBackground(infilename, outfilename);
  //  Si16b_Unfold_DecayElectronCorrection(infilename, outfilename);
}
