//#include "plot/Ti50_SiTEnergy.C"
#include "plot/Ti50_EvdEPlot.C"
#include "plot/Ti50_XRaySpectrum.C"
//#include "plot/Ti50_TargetSpectrum.C"

void Ti50_AllPlots() {

  std::string infilename = "~/data/results/Ti50/subtrees_newPP.root";
  std::string outfilename = "~/data/results/Ti50/plots_newPP_3sigma_layerCoinc500ns.root";

  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  //  Ti50_SiTEnergy(infilename, outfilename);
  Ti50_EvdEPlot(infilename, outfilename);
  Ti50_XRaySpectrum(infilename, outfilename);
  //  Ti50_TargetSpectrum(infilename, outfilename);
}
