#include <fstream>
#include "RooUnfoldResponse.h"

#include "Si16a/final/Si16a_FinalPlot_DataVsMC_BeamEnergy_ThinAll.C"

// #include "Si16b/final/Si16b_FinalPlot_TwoLayer_EvdE_Rotated.C"
// #include "Si16b/final/Si16b_FinalPlot_TwoLayer_EvdE_TCutG_2sigma_forJobTalk.C"
// #include "Si16b/final/Si16b_FinalPlot_TwoLayer_Lifetime_forJobTalk.C"
// #include "Si16b/final/Si16b_FinalPlot_ResponseMatrix_ELoss.C"
// #include "Si16b/final/Si16b_FinalPlot_UnfoldedAndFoldedSpectra_forJobTalk.C"
#include "Si16b/final/Si16b_FinalPlot_CountTMEs.C"
#include "Si16b/final/Si16b_FinalPlot_XRaySpectrum_wFit.C"
//#include "Si16b/final/Si16b_FinalPlot_SiR3VetoEfficiency.C"
#include "Si16b/final/Si16b_FinalPlot_TimeCutEfficiency.C"
#include "Si16b/final/Si16b_FinalPlot_TwoLayer_EvdE_NoVeto_forPaper.C"
#include "Si16b/final/Si16b_FinalPlot_TwoLayer_EvdE_forPaper.C"
#include "Si16b/final/Si16b_FinalPlot_ThreeLayer_EvdE_forPaper.C"
#include "Si16b/final/Si16b_FinalPlot_TwoLayer_EvdE_PSel_forPaper.C"
#include "Si16b/final/Si16b_FinalPlot_UnfoldedAndFoldedSpectra.C"
#include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum_forPaper.C"
#include "Si16b/final/Si16b_FinalPlot_SpectrumFit.C"
//#include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum_wSiL3_forPaper.C"
#include "SiL3/final/SiL3_FinalPlot_CountTMEs.C"
#include "SiL3/final/SiL3_FinalPlot_XRaySpectrum_wFit.C"
#include "SiL3/final/SiL3_FinalPlot_RawSpectrum.C"
#include "SiL3/final/SiL3_FinalPlot_RawSpectrum_RecoilComparison.C"
//#include "SiL3/final/SiL3_FinalPlot_NormalisedSpectrum.C"
// #include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum_wSiL3_RecoilComparison_forJobTalk.C"
// #include "SiL3/final/SiL3_FinalPlot_NormalisedSpectrum_wSW_forJobTalk.C"

#include "Si16b/final/Si16b_FinalPlot_SW.C"
#include "SiL3/final/SiL3_FinalPlot_ResponseMatrix.C"
#include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum_wSiL3NewCorr.C"

#include "DatasetComparisons/RawSpectra_forPaper.C"
//#include "DatasetComparisons/UnfoldedSpectra_SiLVsSiR_Al50_forPaper.C"
#include "Al50/final/Al50_FinalPlot_UnfoldedSpectra_DifferentMuStopPos_SiLSiR_forPaper.C"
//#include "Al50/final/Al50_FinalPlot_UnfoldedSpectra_Al100VsAl50_forPaper.C"
#include "DatasetComparisons/FinalPlots_Al100VsAl50_forPaper.C"
#include "DatasetComparisons/FinalPlots_forPaper.C"
#include "DatasetComparisons/FinalPlots_wExpoFits_forPaper.C"

void PrintPaperPlots() {

  std::string outdir_base = "~/plots/paper/";
  std::ofstream numbers_file("/home/edmonds/plots/paper/silicon-numbers.tex", std::ofstream::out);
  if (!numbers_file.is_open()) {
    std::cout << "ERROR with numbers_file" << std::endl;
    return;
  }
  std::string SiL3_main_tag = "geq2TgtPulse_newPP20us_1";
  std::string SiL3_norm_tag = "geq0TgtPulse_newPP20us_1";

  numbers_file << "\\newcommand\\SiXRayE{400.177(5)}" << std::endl;
  numbers_file << "\\newcommand\\SiXRayI{$80.3(8)$}" << std::endl;
  numbers_file << "\\newcommand\\SiLifetime{$756(1)$}" << std::endl;
  numbers_file << "\\newcommand\\SiLifetimeNoErr{756 ns}" << std::endl;
  //  numbers_file << "\\newcommand\\SiCaptureFraction{$0.658$}" << std::endl;
  numbers_file << "\\newcommand\\SiCaptureFraction{$65.8(13)$}" << std::endl;
  numbers_file << std::endl;
  
  std::string outdir = outdir_base;
  Si16a_FinalPlot_DataVsMC_BeamEnergy_ThinAll(outdir);
  Si16b_FinalPlot_CountTMEs(numbers_file);
  Si16b_FinalPlot_XRaySpectrum_wFit("", numbers_file); // don't want the plots
  //  Si16b_FinalPlot_SiR3VetoEfficiency("", numbers_file);
  numbers_file << "\\newcommand\\SibVetoEff{\\num[round-precision=2, round-mode=figures]{0.74 \\pm 0.07}}" << std::endl; // use combined number from elog:477
  Si16b_FinalPlot_TimeCutEfficiency("", numbers_file);
  //  Si16b_FinalPlot_TwoLayer_EvdE_NoVeto_forPaper(outdir); // no longer needed for paper
  //  Si16b_FinalPlot_TwoLayer_EvdE_forPaper(outdir); // no longer needed for paper
  //  Si16b_FinalPlot_ThreeLayer_EvdE_forPaper(outdir); // no longer needed for paper
  //  Si16b_FinalPlot_TwoLayer_EvdE_PSel_forPaper(outdir); // no longer needed for paper
  Si16b_FinalPlot_UnfoldedAndFoldedSpectra("", numbers_file);
  Si16b_FinalPlot_NormalisedSpectrum_forPaper(outdir, numbers_file);
  Si16b_FinalPlot_SpectrumFit("", numbers_file);
  //  Si16b_FinalPlot_NormalisedSpectrum_wSiL3_forPaper(outdir);
  SiL3_FinalPlot_CountTMEs(SiL3_main_tag, numbers_file);
  SiL3_FinalPlot_XRaySpectrum_wFit(SiL3_norm_tag, "", numbers_file); // don't want the plots
  SiL3_FinalPlot_RawSpectrum(SiL3_main_tag, "", numbers_file);
  SiL3_FinalPlot_RawSpectrum_RecoilComparison(SiL3_main_tag, "", numbers_file);
  //  SiL3_FinalPlot_NormalisedSpectrum("", numbers_file);
  // Si16b_FinalPlot_TwoLayer_EvdE_Rotated(outdir);
  // Si16b_FinalPlot_TwoLayer_EvdE_TCutG_2sigma_forJobTalk(outdir);
  // Si16b_FinalPlot_TwoLayer_Lifetime_forJobTalk(outdir);
  // Si16b_FinalPlot_ResponseMatrix_ELoss(outdir);
  // Si16b_FinalPlot_UnfoldedAndFoldedSpectra_forJobTalk(outdir);
  // Si16b_FinalPlot_NormalisedSpectrum_wSiL3_RecoilComparison_forJobTalk(outdir);
  // SiL3_FinalPlot_NormalisedSpectrum_wSW_forJobTalk(outdir);
  Si16b_FinalPlot_SW(outdir);
  SiL3_FinalPlot_ResponseMatrix(outdir);
  Si16b_FinalPlot_NormalisedSpectrum_wSiL3NewCorr(outdir, numbers_file);

  RawSpectra_forPaper(outdir);
  //  UnfoldedSpectra_SiLVsSiR_Al50_forPaper(outdir);
  Al50_FinalPlot_UnfoldedSpectra_DifferentMuStopPos_SiLSiR_forPaper(outdir);
  //  Al50_FinalPlot_UnfoldedSpectra_Al100VsAl50_forPaper(outdir);
  FinalPlots_Al100VsAl50_forPaper(outdir, numbers_file);
  FinalPlots_forPaper(outdir, numbers_file);
  FinalPlots_wExpoFits_forPaper("", numbers_file); // no plots, just numbers
  numbers_file.close();
}
