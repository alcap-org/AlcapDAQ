#include "scripts/SiliconAnalyses/SiL3/raw_spectra/SiL3_RawSpectrum_fromEnergyTime.C"

void SiL3_AllRawSpectra() {

  std::string infilename = "~/data/results/SiL3/plots.root";
  std::string outfilename = "~/data/results/SiL3/raw_spectra.root";

  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  SiL3_RawSpectrum_fromEnergyTime(infilename, outfilename);  
}
