#include "scripts/SiliconAnalyses/Si16b_passive/raw_spectra/Si16b_RawSpectrum_fromEvdE.C"
#include "scripts/SiliconAnalyses/Si16b_passive/raw_spectra/Si16b_RawSpectrum_fromEnergyTime.C"

void Si16b_AllRawSpectra() {

  std::string infilename = "~/data/results/Si16b_passive/plots.root";
  std::string outfilename = "~/data/results/Si16b_passive/raw_spectra.root";
  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  Si16b_RawSpectrum_fromEvdE(infilename, outfilename);
  Si16b_RawSpectrum_fromEnergyTime(infilename, outfilename);
}
