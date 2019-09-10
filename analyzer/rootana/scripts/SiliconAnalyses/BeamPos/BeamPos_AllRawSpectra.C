#include "scripts/SiliconAnalyses/BeamPos/raw_spectra/BeamPos_RawSpectrum_fromEnergyTime.C"
//#include "scripts/SiliconAnalyses/BeamPos/raw_spectra/BeamPos_RawSpectrum_fromEvdE.C"

#include <sstream>

void BeamPos_AllRawSpectra() {

  const int n_runs = 3;
  int run_numbers[n_runs] = {10473, 10477, 10404};

  for (int i_run = 0; i_run < n_runs; ++i_run) {
    std::stringstream run_str;
    run_str << run_numbers[i_run];

    std::string infilename = "~/data/results/BeamPos/plots" + run_str.str() + "_newPP.root";
    std::string outfilename = "~/data/results/BeamPos/raw_spectra" + run_str.str() + "_newPP.root";
    
    TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
    outfile->Write();
    outfile->Close();

    BeamPos_RawSpectrum_fromEnergyTime(infilename, outfilename);
    //    BeamPos_RawSpectrum_fromEvdE(infilename, outfilename);
  }
}
