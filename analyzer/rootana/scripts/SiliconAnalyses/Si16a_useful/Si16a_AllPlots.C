#include "scripts/SiliconAnalyses/Si16a_useful/Si16a_EvdEPlot.C"
#include "scripts/SiliconAnalyses/Si16a_useful/Si16a_XRaySpectrum.C"
#include "scripts/SiliconAnalyses/Si16a_useful/Si16a_TargetSpectrum.C"
#include "scripts/SiliconAnalyses/Si16a_useful/Si16a_CrossCheckSpectrum.C"

#include <sstream>

void Si16a_AllPlots(int run_number, double min_muon_energy, double max_muon_energy) {

  std::stringstream run_str;
  run_str << "0" << run_number;
  std::string infilename = "~/data/results/Si16a_useful/subtrees" + run_str.str() + ".root";
  std::string outfilename = "~/data/results/Si16a_useful/plots" + run_str.str() + ".root";

  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  Si16a_EvdEPlot(infilename, outfilename);
  Si16a_XRaySpectrum(infilename, outfilename);
  Si16a_TargetSpectrum(infilename, outfilename);
  Si16a_CrossCheckSpectrum(infilename, outfilename, min_muon_energy, max_muon_energy);
}
