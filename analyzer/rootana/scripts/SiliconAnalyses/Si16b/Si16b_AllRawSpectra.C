#include "raw_spectra/Si16b_RawSpectrum_fromEvdE.C"
#include "raw_spectra/Si16b_RawSpectrum_fromEnergyTime.C"

void Si16b_AllRawSpectra() {

  std::string infilename = "~/data/results/Si16b/plots_newPP_geq1TgtPulse_1.root";
  std::string outfilename = "~/data/results/Si16b/raw_spectra_newPP_geq1TgtPulse_2.root";
  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  Si16b_RawSpectrum_fromEvdE(infilename, outfilename);
  //  Si16b_RawSpectrum_fromEnergyTime(infilename, outfilename);
}
