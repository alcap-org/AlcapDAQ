#include "scripts/SiliconAnalyses/Si16b/raw_spectra/Si16b_RawSpectrum_fromEvdE.C"
#include "scripts/SiliconAnalyses/Si16b/raw_spectra/Si16b_RawSpectrum_fromEnergyTime.C"

void Si16b_AllRawSpectra() {

  std::string infilename = "~/data/results/Si16b/plots_newPP.root";
  std::string outfilename = "~/data/results/Si16b/raw_spectra_newPP.root";
  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  Si16b_RawSpectrum_fromEvdE(infilename, outfilename);
  Si16b_RawSpectrum_fromEnergyTime(infilename, outfilename);
}
