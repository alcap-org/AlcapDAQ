#include "scripts/SiliconAnalyses/SiL3/normalisation/SiL3_CountStoppedMuons_XRaySpectrum.C"
#include "scripts/SiliconAnalyses/SiL3/normalisation/SiL3_CountStoppedMuons_TargetSpectrum.C"
#include "scripts/SiliconAnalyses/SiL3/normalisation/SiL3_CountStoppedMuons_XRaySpectrum_wTargetCoinc.C"

void SiL3_AllNormalisations() {

  std::string infilename = "~/data/results/SiL3/plots.root";
  std::string outfilename = "~/data/results/SiL3/normalisation.root";
  //  std::string infilename = "~/data/results/SiL3_active_no-templates/plots.root";
  //  std::string outfilename = "~/data/results/SiL3_active_no-templates/normalisation.root";

  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  SiL3_CountStoppedMuons_XRaySpectrum(infilename, outfilename);
  SiL3_CountStoppedMuons_TargetSpectrum(infilename, outfilename);
  SiL3_CountStoppedMuons_XRaySpectrum_wTargetCoinc(infilename, outfilename);
}
