#include "lifetime/Si16b_LifetimeFit.C"

#include <sstream>

void Si16b_AllLifetimes() {

  std::string infilename = "~/data/results/Si16b/plots_newPP_geq1TgtPulse_3sigma.root";
  std::string outfilename = "~/data/results/Si16b/lifetime_newPP_geq1TgtPulse_3sigma.root";
  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  Si16b_LifetimeFit(infilename, outfilename);
}
