#include "normalisation/Al50_CountStoppedMuons_XRaySpectrum.C"

#include <sstream>

void Al50_AllNormalisations() {

  std::string infilename = "~/data/results/Al50/Al_XRaySpectra_fromJohn.root";
  std::string outfilename = "~/data/results/Al50/normalisation_forJohn.root";
  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  Al50_CountStoppedMuons_XRaySpectrum(infilename, outfilename);
}
