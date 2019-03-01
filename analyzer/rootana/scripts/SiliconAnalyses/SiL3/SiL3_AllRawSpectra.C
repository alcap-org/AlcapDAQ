#include "scripts/SiliconAnalyses/SiL3/raw_spectra/SiL3_RawSpectrum_fromEnergyTime.C"

void SiL3_AllRawSpectra() {

  //  std::string infilename = "~/data/results/SiL3/plots.root";
  //  std::string outfilename = "~/data/results/SiL3/raw_spectra.root";
  //  std::string infilename = "~/data/results/SiL3/plots_wMuScCut_3000-3500ADC.root";
  //  std::string outfilename = "~/data/results/SiL3/raw_spectra_wMuScCut_3000-3500ADC.root";
  std::string infilename = "~/data/results/SiL3/plots_special.root";
  std::string outfilename = "~/data/results/SiL3/raw_spectra_special.root";

  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  SiL3_RawSpectrum_fromEnergyTime(infilename, outfilename);  
}
