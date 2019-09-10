//#include "scripts/SiliconAnalyses/BeamPos/plot/BeamPos_EvdEPlot.C"
#include "scripts/SiliconAnalyses/BeamPos/plot/BeamPos_XRaySpectrum.C"
#include "scripts/SiliconAnalyses/BeamPos/plot/BeamPos_TargetSpectrum.C"
//#include "scripts/SiliconAnalyses/BeamPos/plot/BeamPos_XRaySpectrum_wTargetCoinc.C"

#include <sstream>

void BeamPos_AllPlots() {

  const int n_runs = 3;
  int run_numbers[n_runs] = {10473, 10477, 10404};

  for (int i_run = 0; i_run < n_runs; ++i_run) {
    std::stringstream run_str;
    run_str << run_numbers[i_run];
    
    std::string infilename = "~/data/results/BeamPos/subtrees" + run_str.str() + "_newPP.root";
    std::string outfilename = "~/data/results/BeamPos/plots" + run_str.str() + "_newPP.root";
    
    TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
    outfile->Write();
    outfile->Close();
    
    //    BeamPos_EvdEPlot(infilename, outfilename);
    BeamPos_XRaySpectrum(infilename, outfilename);
    BeamPos_TargetSpectrum(infilename, outfilename);
    //    BeamPos_XRaySpectrum_wTargetCoinc(infilename, outfilename);
  }
}
