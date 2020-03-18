#include <fstream>

#include "Si16b/final/Si16b_FinalPlot_TwoLayer_EvdE_Rotated.C"
#include "Si16b/final/Si16b_FinalPlot_TwoLayer_EvdE_TCutG_2sigma_forJobTalk.C"
#include "Si16b/final/Si16b_FinalPlot_TwoLayer_Lifetime_forJobTalk.C"
#include "Si16b/final/Si16b_FinalPlot_ResponseMatrix_ELoss.C"
#include "Si16b/final/Si16b_FinalPlot_UnfoldedAndFoldedSpectra_forJobTalk.C"
#include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum_forJobTalk.C"
#include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum_wSiL3_forJobTalk.C"
#include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum_wSiL3_RecoilComparison_forJobTalk.C"
#include "SiL3/final/SiL3_FinalPlot_NormalisedSpectrum_wSW_forJobTalk.C"

void PrintJobTalkPlots() {

  std::string outdir_base = "~/plots/job_talk/";
  // std::ofstream numbers_file("/home/edmonds/plots/analysis_note/silicon/figs/silicon-numbers.tex", std::ofstream::out);
  // if (!numbers_file.is_open()) {
  //   std::cout << "ERROR with numbers_file" << std::endl;
  //   return;
  // }
  
  std::string outdir = outdir_base;
  Si16b_FinalPlot_TwoLayer_EvdE_Rotated(outdir);
  Si16b_FinalPlot_TwoLayer_EvdE_TCutG_2sigma_forJobTalk(outdir);
  Si16b_FinalPlot_TwoLayer_Lifetime_forJobTalk(outdir);
  Si16b_FinalPlot_ResponseMatrix_ELoss(outdir);
  Si16b_FinalPlot_UnfoldedAndFoldedSpectra_forJobTalk(outdir);
  Si16b_FinalPlot_NormalisedSpectrum_forJobTalk(outdir);
  Si16b_FinalPlot_NormalisedSpectrum_wSiL3_forJobTalk(outdir);
  Si16b_FinalPlot_NormalisedSpectrum_wSiL3_RecoilComparison_forJobTalk(outdir);
  SiL3_FinalPlot_NormalisedSpectrum_wSW_forJobTalk(outdir);
  //  numbers_file.close();
}
