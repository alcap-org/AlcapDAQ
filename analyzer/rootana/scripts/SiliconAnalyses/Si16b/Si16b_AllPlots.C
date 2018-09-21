#include "scripts/SiliconAnalyses/Si16b_passive/plot/Si16b_SiTEnergy.C"
#include "scripts/SiliconAnalyses/Si16b_passive/plot/Si16b_EvdEPlot.C"
#include "scripts/SiliconAnalyses/Si16b_passive/plot/Si16b_XRaySpectrum.C"
#include "scripts/SiliconAnalyses/Si16b_passive/plot/Si16b_TargetSpectrum.C"
#include "scripts/SiliconAnalyses/Si16b_passive/plot/Si16b_XRaySpectrum_wTargetCoinc.C"
#include "scripts/SiliconAnalyses/Si16b_passive/plot/Si16b_RemoveFlatBkg.C"

void Si16b_AllPlots() {

  std::string infilename = "~/data/results/Si16b_passive/subtrees.root";
  std::string outfilename = "~/data/results/Si16b_passive/plots.root";

  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  //  Si16b_SiTEnergy(infilename, outfilename);
  //  Si16b_EvdEPlot(infilename, outfilename);
  //  Si16b_XRaySpectrum(infilename, outfilename);
  Si16b_TargetSpectrum(infilename, outfilename);
  //  Si16b_XRaySpectrum_wTargetCoinc(infilename, outfilename);

  Si16b_RemoveFlatBkg(infilename, outfilename);
}
