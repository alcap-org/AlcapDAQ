#include "scripts/SiliconAnalyses/Si16a/normalisation/Si16a_CountStoppedMuons_XRaySpectrum.C"
#include "scripts/SiliconAnalyses/Si16a/normalisation/Si16a_CountStoppedMuons_TargetSpectrum.C"
#include "scripts/SiliconAnalyses/Si16a/normalisation/Si16a_CountStoppedMuons_XRaySpectrum_wTargetCoinc.C"

#include <sstream>

void Si16a_AllNormalisations() {

  const int n_runs = 8;
  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743}; // want to do all runs individually

  for (int i_run = 0; i_run < n_runs; ++i_run) {
    std::stringstream run_str;
    run_str << "0" << run_numbers[i_run];

    std::string infilename = "~/data/results/Si16a/plots" + run_str.str() + ".root";
    std::string outfilename = "~/data/results/Si16a/normalisation" + run_str.str() + ".root";
    
    TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
    outfile->Write();
    outfile->Close();
    
    Si16a_CountStoppedMuons_XRaySpectrum(infilename, outfilename);
    Si16a_CountStoppedMuons_TargetSpectrum(infilename, outfilename);
    Si16a_CountStoppedMuons_XRaySpectrum_wTargetCoinc(infilename, outfilename);
  }
}
