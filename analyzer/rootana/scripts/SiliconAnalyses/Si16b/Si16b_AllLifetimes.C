#include "scripts/SiliconAnalyses/Si16b/lifetime/Si16b_LifetimeFit.C"

#include <sstream>

void Si16b_AllLifetimes() {

  std::string infilename = "~/data/results/Si16b/plots.root";
  std::string outfilename = "~/data/results/Si16b/lifetime.root";
  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  Si16b_LifetimeFit(infilename, outfilename);
}
