#include "scripts/SiliconAnalyses/Si16a_useful/Si16a_CountStoppedMuons_XRaySpectrum_wTargetCoinc.C"

#include <sstream>

void Si16a_SpecialNormalisation(int run_number = 9743) {

  std::stringstream run_str;
  run_str << "0" << run_number;

  std::string infilename = "~/data/results/Si16a_useful/plots" + run_str.str() + ".root";
  std::string outfilename = "~/data/results/Si16a_useful/special-normalisation" + run_str.str() + ".root";

  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  Si16a_CountStoppedMuons_XRaySpectrum_wTargetCoinc(infilename, outfilename, 500, 1500);
}
