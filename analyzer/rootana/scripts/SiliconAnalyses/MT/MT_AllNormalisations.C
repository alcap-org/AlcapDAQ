#include "scripts/SiliconAnalyses/MT/normalisation/MT_CountStoppedMuons_XRaySpectrum.C"

#include <sstream>

void MT_AllNormalisations() {

  const int n_settings = 2;
  std::string settings[n_settings] = {"SF1-05", "SF1-025"};

  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = settings[i_setting];

    std::string infilename = "~/data/results/MT/plots_" + setting + ".root";
    std::string outfilename = "~/data/results/MT/normalisation_" + setting + ".root";
  
    TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
    outfile->Write();
    outfile->Close();

    MT_CountStoppedMuons_XRaySpectrum(infilename, outfilename);
  }
}
