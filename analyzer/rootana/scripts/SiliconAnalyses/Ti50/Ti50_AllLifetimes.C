#include "scripts/SiliconAnalyses/Ti50/lifetime/Ti50_LifetimeFit.C"

#include <sstream>

void Ti50_AllLifetimes() {

  std::string infilename = "~/data/results/Ti50/plots.root";
  std::string outfilename = "~/data/results/Ti50/lifetime.root";
  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  Ti50_LifetimeFit(infilename, outfilename);
}
