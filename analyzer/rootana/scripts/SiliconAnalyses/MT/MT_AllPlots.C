#include "scripts/SiliconAnalyses/MT/plot/MT_SiTEnergy.C"
#include "scripts/SiliconAnalyses/MT/plot/MT_EvdEPlot.C"
#include "scripts/SiliconAnalyses/MT/plot/MT_XRaySpectrum.C"
#include "scripts/SiliconAnalyses/MT/plot/MT_TargetSpectrum.C"

void MT_AllPlots() {

  const int n_settings = 2;
  std::string settings[n_settings] = {"SF1-05", "SF1-025"};

  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = settings[i_setting];
    
    std::string infilename = "~/data/results/MT/subtrees_" + setting + ".root";
    std::string outfilename = "~/data/results/MT/plots_" + setting + ".root";

    TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
    outfile->Write();
    outfile->Close();
    
    MT_SiTEnergy(infilename, outfilename);
    MT_EvdEPlot(infilename, outfilename);
    MT_XRaySpectrum(infilename, outfilename);
    MT_TargetSpectrum(infilename, outfilename);
  }
}
