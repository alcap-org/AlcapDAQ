#include "scripts/SiliconAnalyses/Si16a_useful/Si16a_CountStoppedMuons_XRaySpectrum.C"
#include "scripts/SiliconAnalyses/Si16a_useful/Si16a_CountStoppedMuons_TargetSpectrum.C"
#include "scripts/SiliconAnalyses/Si16a_useful/Si16a_CountStoppedMuons_CrossCheckSpectrum.C"

#include <sstream>

void Si16a_AllNormalisations(int run_number, double min_muon_energy, double max_muon_energy) {

  std::stringstream run_str;
  run_str << "0" << run_number;

  std::string infilename = "~/data/results/Si16a_useful/plots" + run_str.str() + ".root";
  std::string outfilename = "~/data/results/Si16a_useful/normalisation" + run_str.str() + ".root";

  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  Si16a_CountStoppedMuons_XRaySpectrum(infilename, outfilename);
  Si16a_CountStoppedMuons_TargetSpectrum(infilename, outfilename);
  Si16a_CountStoppedMuons_CrossCheckSpectrum(infilename, outfilename, min_muon_energy, max_muon_energy);
}
