#include "scripts/SiliconAnalyses/SiL3_active/SiL3_CountStoppedMuons_XRaySpectrum.C"
#include "scripts/SiliconAnalyses/SiL3_active/SiL3_CountStoppedMuons_TargetSpectrum.C"
#include "scripts/SiliconAnalyses/SiL3_active/SiL3_CountStoppedMuons_CrossCheckSpectrum.C"

void SiL3_AllNormalisations() {

  std::string infilename = "~/data/results/SiL3_active/plots.root";
  std::string outfilename = "~/data/results/SiL3_active/normalisation.root";

  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  SiL3_CountStoppedMuons_XRaySpectrum(infilename, outfilename);
  SiL3_CountStoppedMuons_TargetSpectrum(infilename, outfilename);
  SiL3_CountStoppedMuons_CrossCheckSpectrum(infilename, outfilename);
}
