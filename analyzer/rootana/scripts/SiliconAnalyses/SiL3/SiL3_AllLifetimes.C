#include "scripts/SiliconAnalyses/SiL3_active/lifetime/SiL3_LifetimeFit.C"

#include <sstream>

void SiL3_AllLifetimes() {

  std::string infilename = "~/data/results/SiL3_active/plots.root";
  std::string outfilename = "~/data/results/SiL3_active/lifetime.root";
  //  std::string infilename = "~/data/results/SiL3_active_no-templates/plots.root";
  //  std::string outfilename = "~/data/results/SiL3_active_no-templates/lifetime.root";
  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  SiL3_LifetimeFit(infilename, outfilename);
}
