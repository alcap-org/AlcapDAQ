#include <fstream>

// #include "Si16b/final/Si16b_FinalPlot_TwoLayer_EvdE_Rotated.C"
// #include "Si16b/final/Si16b_FinalPlot_TwoLayer_EvdE_TCutG_2sigma_forJobTalk.C"
// #include "Si16b/final/Si16b_FinalPlot_TwoLayer_Lifetime_forJobTalk.C"
// #include "Si16b/final/Si16b_FinalPlot_ResponseMatrix_ELoss.C"
// #include "Si16b/final/Si16b_FinalPlot_UnfoldedAndFoldedSpectra_forJobTalk.C"
#include "Si16b/final/Si16b_FinalPlot_XRaySpectrum_wFit.C"
#include "Si16b/final/Si16b_FinalPlot_SiR3VetoEfficiency.C"
#include "Si16b/final/Si16b_FinalPlot_TimeCutEfficiency.C"
#include "Si16b/final/Si16b_FinalPlot_TwoLayer_EvdE_NoVeto_forPaper.C"
#include "Si16b/final/Si16b_FinalPlot_TwoLayer_EvdE_forPaper.C"
#include "Si16b/final/Si16b_FinalPlot_ThreeLayer_EvdE_forPaper.C"
#include "Si16b/final/Si16b_FinalPlot_TwoLayer_EvdE_PSel_forPaper.C"
#include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum_forPaper.C"
#include "Si16b/final/Si16b_FinalPlot_SpectrumFit.C"
#include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum_wSiL3_forPaper.C"
#include "SiL3/final/SiL3_FinalPlot_XRaySpectrum_wFit.C"
#include "SiL3/final/SiL3_FinalPlot_RawSpectrum_RecoilComparison.C"
#include "SiL3/final/SiL3_FinalPlot_NormalisedSpectrum.C"
// #include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum_wSiL3_RecoilComparison_forJobTalk.C"
// #include "SiL3/final/SiL3_FinalPlot_NormalisedSpectrum_wSW_forJobTalk.C"

void PrintPaperPlots() {

  std::string outdir_base = "~/plots/paper/";
  std::ofstream numbers_file("/home/edmonds/plots/paper/silicon-numbers.tex", std::ofstream::out);
  if (!numbers_file.is_open()) {
    std::cout << "ERROR with numbers_file" << std::endl;
    return;
  }

  numbers_file << "\\newcommand\\SiXRayE{400.177(5)}" << std::endl;
  numbers_file << "\\newcommand\\SiXRayI{$80.3(8)$}" << std::endl;
  numbers_file << "\\newcommand\\SiLifetime{$756(1)$}" << std::endl;
  numbers_file << "\\newcommand\\SiLifetimeNoErr{756 ns}" << std::endl;
  //  numbers_file << "\\newcommand\\SiCaptureFraction{$0.658$}" << std::endl;
  numbers_file << "\\newcommand\\SiCaptureFraction{$65.8$}" << std::endl;
  numbers_file << std::endl;
  
  std::string outdir = outdir_base;
  Si16b_FinalPlot_XRaySpectrum_wFit("", numbers_file); // don't want the plots
  Si16b_FinalPlot_SiR3VetoEfficiency("", numbers_file);
  Si16b_FinalPlot_TimeCutEfficiency("", numbers_file);
  Si16b_FinalPlot_TwoLayer_EvdE_NoVeto_forPaper(outdir);
  Si16b_FinalPlot_TwoLayer_EvdE_forPaper(outdir);
  Si16b_FinalPlot_ThreeLayer_EvdE_forPaper(outdir);
  Si16b_FinalPlot_TwoLayer_EvdE_PSel_forPaper(outdir);
  Si16b_FinalPlot_NormalisedSpectrum_forPaper(outdir, numbers_file);
  Si16b_FinalPlot_SpectrumFit("", numbers_file);
  Si16b_FinalPlot_NormalisedSpectrum_wSiL3_forPaper(outdir);
  SiL3_FinalPlot_XRaySpectrum_wFit("", numbers_file); // don't want the plots
  SiL3_FinalPlot_RawSpectrum_RecoilComparison("", numbers_file);
  SiL3_FinalPlot_NormalisedSpectrum("", numbers_file);
  // Si16b_FinalPlot_TwoLayer_EvdE_Rotated(outdir);
  // Si16b_FinalPlot_TwoLayer_EvdE_TCutG_2sigma_forJobTalk(outdir);
  // Si16b_FinalPlot_TwoLayer_Lifetime_forJobTalk(outdir);
  // Si16b_FinalPlot_ResponseMatrix_ELoss(outdir);
  // Si16b_FinalPlot_UnfoldedAndFoldedSpectra_forJobTalk(outdir);
  // Si16b_FinalPlot_NormalisedSpectrum_wSiL3_RecoilComparison_forJobTalk(outdir);
  // SiL3_FinalPlot_NormalisedSpectrum_wSW_forJobTalk(outdir);
  numbers_file.close();
}
