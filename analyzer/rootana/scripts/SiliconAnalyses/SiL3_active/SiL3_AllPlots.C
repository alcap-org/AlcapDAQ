//#include "scripts/SiliconAnalyses/SiL3_active/SiL3_EvdEPlot.C"
#include "scripts/SiliconAnalyses/SiL3_active/SiL3_XRaySpectrum.C"
#include "scripts/SiliconAnalyses/SiL3_active/SiL3_TargetSpectrum.C"
#include "scripts/SiliconAnalyses/SiL3_active/SiL3_CrossCheckSpectrum.C"

void SiL3_AllPlots() {

  std::string infilename = "~/data/results/SiL3_active/subtrees.root";
  std::string outfilename = "~/data/results/SiL3_active/plots.root";

  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  SiL3_XRaySpectrum(infilename, outfilename);
  SiL3_TargetSpectrum(infilename, outfilename);
  SiL3_CrossCheckSpectrum(infilename, outfilename);
}
