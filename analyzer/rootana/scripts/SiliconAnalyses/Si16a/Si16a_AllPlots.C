#include "scripts/SiliconAnalyses/Si16a/plot/Si16a_EvdEPlot.C"
#include "scripts/SiliconAnalyses/Si16a/plot/Si16a_XRaySpectrum.C"
#include "scripts/SiliconAnalyses/Si16a/plot/Si16a_TargetSpectrum.C"
#include "scripts/SiliconAnalyses/Si16a/plot/Si16a_XRaySpectrum_wTargetCoinc.C"

#include <sstream>

void Si16a_AllPlots() {

  const int n_runs = 8;
  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743}; // want to do all runs individually

  for (int i_run = 0; i_run < n_runs; ++i_run) {
    std::stringstream run_str;
    run_str << "0" << run_numbers[i_run];
    
    std::string infilename = "~/data/results/Si16a/subtrees" + run_str.str() + "_newPP.root";
    std::string outfilename = "~/data/results/Si16a/plots" + run_str.str() + "_newPP.root";
    
    TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
    outfile->Write();
    outfile->Close();
    
    Si16a_EvdEPlot(infilename, outfilename);
    Si16a_XRaySpectrum(infilename, outfilename);
    Si16a_TargetSpectrum(infilename, outfilename);
    //    Si16a_XRaySpectrum_wTargetCoinc(infilename, outfilename);
  }
}
