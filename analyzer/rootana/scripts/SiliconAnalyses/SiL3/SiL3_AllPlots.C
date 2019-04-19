#include "scripts/SiliconAnalyses/SiL3/plot/SiL3_XRaySpectrum.C"
#include "scripts/SiliconAnalyses/SiL3/plot/SiL3_TargetSpectrum.C"
#include "scripts/SiliconAnalyses/SiL3/plot/SiL3_XRaySpectrum_wTargetCoinc.C"
#include "scripts/SiliconAnalyses/SiL3/plot/SiL3_RemoveFlatBkg.C"

void SiL3_AllPlots() {

  //  std::string infilename = "~/data/results/SiL3/subtrees.root";
  //  std::string outfilename = "~/data/results/SiL3/plots.root";
  //  std::string infilename = "~/data/results/SiL3/subtrees_v13.root";
  //  std::string outfilename = "~/data/results/SiL3/plots_v13.root";
  //  std::string infilename = "~/data/results/SiL3/subtrees_wMuScCut_3000-3500ADC.root";
  //  std::string outfilename = "~/data/results/SiL3/plots_wMuScCut_3000-3500ADC.root";
  //  std::string infilename = "~/data/results/SiL3_active_no-templates/subtrees_geq1TgtPulse.root";
  //  std::string outfilename = "~/data/results/SiL3_active_no-templates/plots_geq1TgtPulse.root";
  std::string infilename = "~/data/results/SiL3/subtrees_geq1TgtPulse.root";
  std::string outfilename = "~/data/results/SiL3/plots_geq1TgtPulse.root";

  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  SiL3_XRaySpectrum(infilename, outfilename);
  SiL3_TargetSpectrum(infilename, outfilename);
  SiL3_XRaySpectrum_wTargetCoinc(infilename, outfilename);

  //  SiL3_RemoveFlatBkg(infilename, outfilename);
}
