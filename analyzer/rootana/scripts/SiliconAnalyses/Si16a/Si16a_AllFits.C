#include "scripts/SiliconAnalyses/Si16a/fits/Si16a_PunchthroughAndStopFit.C"
#include "scripts/SiliconAnalyses/Si16a/fits/Si16aMC_PunchthroughAndStopFit.C"

#include <sstream>

void Si16a_AllFits() {

  const int n_runs = 8;
  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743}; // want to do all runs individually
  bool add_punchthroughs[n_runs] = {true, false, false, true, true, true, true, true};
  //  const int n_runs = 1;
  //  int run_numbers[n_runs] = {9740}; // want to do all runs individually

  for (int i_run = 0; i_run < n_runs; ++i_run) {
    std::stringstream run_str;
    run_str << "0" << run_numbers[i_run];

    std::string infilename = "~/data/results/Si16a/raw_spectra" + run_str.str() + ".root";
    std::string outfilename = "~/data/results/Si16a/fits" + run_str.str() + ".root";
    
    TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
    outfile->Write();
    outfile->Close();

    Si16a_PunchthroughAndStopFit(infilename, outfilename, add_punchthroughs[i_run]);
  }

  // Now do MC
  std::string infilename = "~/data/mc/Si16a/MuonBeamSim_AllPlots.root";
  std::string outfilename = "~/data/mc/Si16a/MuonBeamSim_AllPlots_Fits.root";
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();
  Si16aMC_PunchthroughAndStopFit(infilename, outfilename, true);

}
