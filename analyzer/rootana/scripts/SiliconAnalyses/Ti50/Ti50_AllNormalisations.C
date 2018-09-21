#include "scripts/SiliconAnalyses/Ti50/Ti50_CountStoppedMuons_XRaySpectrum.C"

#include <sstream>

void Ti50_AllNormalisations() {

  std::string infilename = "~/data/results/Ti50/plots_test.root";
  std::string outfilename = "~/data/results/Ti50/normalisation_test.root";
  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  Ti50_CountStoppedMuons_XRaySpectrum(infilename, outfilename);
}
