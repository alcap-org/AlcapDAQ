#include "scripts/SiliconAnalyses/SiL3/plot/SiL3_SpecialTargetSpectrum.C"

void SiL3_SpecialAllPlots() {

  std::string infilename = "~/data/results/SiL3/subtrees_special.root";
  std::string outfilename = "~/data/results/SiL3/plots_special.root";


  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  SiL3_SpecialTargetSpectrum(infilename, outfilename);
}
