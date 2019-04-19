//#include "scripts/SiliconAnalyses/Ti50/raw_spectra/Ti50_RawSpectrum_fromEvdE.C"
#include "scripts/SiliconAnalyses/Ti50/raw_spectra/Ti50_RawSpectrum_fromEnergyTime.C"

void Ti50_AllRawSpectra() {

  std::string infilename = "~/data/results/Ti50/plots.root";
  std::string outfilename = "~/data/results/Ti50/raw_spectra.root";
  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  //  Ti50_RawSpectrum_fromEvdE(infilename, outfilename);
  Ti50_RawSpectrum_fromEnergyTime(infilename, outfilename);
}
