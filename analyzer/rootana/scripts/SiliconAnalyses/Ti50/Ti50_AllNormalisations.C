#include "normalisation/Ti50_CountStoppedMuons_XRaySpectrum.C"

#include <sstream>

void Ti50_AllNormalisations() {

  std::string infilename = "~/data/results/Ti50/plots_newPP_3sigma.root";
  std::string outfilename = "~/data/results/Ti50/normalisation_newPP_rebin2_largerFitWindow_wJohnFit.root";
  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  Ti50_CountStoppedMuons_XRaySpectrum(infilename, outfilename);
}
