#include "plot/Si16b_SiTEnergy.C"
#include "plot/Si16b_EvdEPlot.C"
#include "plot/Si16b_XRaySpectrum.C"
#include "plot/Si16b_TargetSpectrum.C"
#include "plot/Si16b_XRaySpectrum_wTargetCoinc.C"

void Si16b_AllPlots() {

  std::string infilename = "~/data/results/Si16b/subtrees_newPP_geq1TgtPulse_1.root";
  std::string outfilename = "~/data/results/Si16b/plots_newPP_geq1TgtPulse_1.root";

  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  //  Si16b_LayerTDiff(infilename, outfilename);
  //  Si16b_SiTEnergy(infilename, outfilename);
  Si16b_EvdEPlot(infilename, outfilename);
  Si16b_XRaySpectrum(infilename, outfilename);
  Si16b_TargetSpectrum(infilename, outfilename);
  //  Si16b_XRaySpectrum_wTargetCoinc(infilename, outfilename);

  //  Si16b_RemoveFlatBkg(infilename, outfilename);
}
