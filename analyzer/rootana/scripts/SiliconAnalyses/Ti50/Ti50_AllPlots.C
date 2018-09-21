#include "scripts/SiliconAnalyses/Ti50/plot/Ti50_SiTEnergy.C"
#include "scripts/SiliconAnalyses/Ti50/plot/Ti50_EvdEPlot.C"
#include "scripts/SiliconAnalyses/Ti50/plot/Ti50_XRaySpectrum.C"

void Ti50_AllPlots() {

  std::string infilename = "~/data/results/Ti50/subtrees.root";
  std::string outfilename = "~/data/results/Ti50/plots.root";

  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  Ti50_SiTEnergy(infilename, outfilename);
  Ti50_EvdEPlot(infilename, outfilename);
  Ti50_XRaySpectrum(infilename, outfilename);
}
