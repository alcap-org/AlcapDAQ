#include "normalisation/Si16b_CountStoppedMuons_XRaySpectrum.C"
#include "normalisation/Si16b_CountStoppedMuons_TargetSpectrum.C"
#include "normalisation/Si16b_CountStoppedMuons_XRaySpectrum_wTargetCoinc.C"

#include <sstream>

void Si16b_AllNormalisations() {

  std::string infilename = "~/data/results/Si16b/plots_newPP_geq1TgtPulse_1.root";
  std::string outfilename = "~/data/results/Si16b/normalisation_newPP_geq1TgtPulse_1.root";
  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  Si16b_CountStoppedMuons_XRaySpectrum(infilename, outfilename);
  Si16b_CountStoppedMuons_TargetSpectrum(infilename, outfilename);
  Si16b_CountStoppedMuons_XRaySpectrum_wTargetCoinc(infilename, outfilename);
}
