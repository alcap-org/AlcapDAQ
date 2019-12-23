#include "Si16b/final/Si16b_FinalPlot_Budyashov.C"
#include "Si16b/final/Si16b_FinalPlot_Target_RawSpectrum.C"
#include "Si16b/final/Si16b_FinalPlot_XRaySpectrum_wFit.C"
#include "Si16b/final/Si16b_FinalPlot_XRaySpectrum_TimeCuts.C"
#include "Si16b/final/Si16b_FinalPlot_TwoLayer_EvdE_TCutG.C"
#include "Si16b/final/Si16b_FinalPlot_ThreeLayer_EvdE_TCutG.C"
#include "Si16b/final/Si16b_FinalPlot_TwoLayer_TDiff.C"
#include "Si16b/final/Si16b_FinalPlot_TwoLayer_Lifetime.C"
#include "Si16b/final/Si16b_FinalPlot_TimeCutEfficiency.C"
#include "Si16b/final/Si16b_FinalPlot_SiR3VetoEfficiency.C"
#include "Si16b/final/Si16b_FinalPlot_RawSpectra_SiR3Veto.C"
#include "Si16b/final/Si16b_FinalPlot_ResponseMatrix.C"
#include "Si16b/final/Si16b_FinalPlot_UnfoldedAndFoldedSpectra.C"
#include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum.C"
#include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum_wSW.C"
#include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum_wBudyashov.C"

#include "SiL3/final/SiL3_FinalPlot_Target_RawSpectrum.C"
#include "SiL3/final/SiL3_FinalPlot_XRaySpectrum_wFit.C"
#include "SiL3/final/SiL3_FinalPlot_TargetSpectrum_EnergyTime.C"
#include "SiL3/final/SiL3_FinalPlot_RawSpectrum.C"
#include "SiL3/final/SiL3_FinalPlot_RawSpectrum_RecoilComparison.C"
#include "SiL3/final/SiL3_FinalPlot_Lifetime.C"
#include "SiL3/final/SiL3_FinalPlot_LifetimeComparison.C"
#include "SiL3/final/SiL3_FinalPlot_FlatBkgCorrection.C"
#include "SiL3/final/SiL3_FinalPlot_DecayElectronCorrection.C"
#include "SiL3/final/SiL3_FinalPlot_ResponseMatrix.C"
#include "SiL3/final/SiL3_FinalPlot_EscapeCorrection.C"
#include "SiL3/final/SiL3_FinalPlot_RawSpectrum_RecoilComparisonSystematic.C"
#include "SiL3/final/SiL3_FinalPlot_NormalisedSpectrum_wSW.C"

#include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum_wSiL3.C"
#include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum_wAll.C"

void PrintAnalysisNotePlots() {

  std::string outdir_base = "~/plots/analysis_note/";

  std::string outdir = outdir_base + "silicon/figs/";
  Si16b_FinalPlot_Budyashov(outdir);
  Si16b_FinalPlot_Target_RawSpectrum(outdir);
  Si16b_FinalPlot_XRaySpectrum_wFit(outdir);
  Si16b_FinalPlot_XRaySpectrum_TimeCuts(outdir);
  Si16b_FinalPlot_TwoLayer_EvdE_TCutG(outdir);
  Si16b_FinalPlot_ThreeLayer_EvdE_TCutG(outdir);
  Si16b_FinalPlot_TwoLayer_TDiff(outdir);
  Si16b_FinalPlot_TwoLayer_Lifetime(outdir);
  Si16b_FinalPlot_TimeCutEfficiency(outdir);
  Si16b_FinalPlot_SiR3VetoEfficiency(outdir);
  Si16b_FinalPlot_RawSpectra_SiR3Veto(outdir);
  Si16b_FinalPlot_ResponseMatrix(outdir);
  Si16b_FinalPlot_UnfoldedAndFoldedSpectra(outdir);
  Si16b_FinalPlot_NormalisedSpectrum(outdir);
  Si16b_FinalPlot_NormalisedSpectrum_wSW(outdir);
  Si16b_FinalPlot_NormalisedSpectrum_wBudyashov(outdir);

  SiL3_FinalPlot_Target_RawSpectrum(outdir);
  SiL3_FinalPlot_XRaySpectrum_wFit(outdir);
  SiL3_FinalPlot_TargetSpectrum_EnergyTime(outdir);
  SiL3_FinalPlot_RawSpectrum(outdir);
  SiL3_FinalPlot_RawSpectrum_RecoilComparison(outdir);
  SiL3_FinalPlot_Lifetime(outdir);
  SiL3_FinalPlot_LifetimeComparison(outdir);
  SiL3_FinalPlot_FlatBkgCorrection(outdir);
  SiL3_FinalPlot_DecayElectronCorrection(outdir);
  SiL3_FinalPlot_ResponseMatrix(outdir);
  SiL3_FinalPlot_EscapeCorrection(outdir);
  SiL3_FinalPlot_RawSpectrum_RecoilComparisonSystematic(outdir);
  SiL3_FinalPlot_NormalisedSpectrum_wSW(outdir);

  Si16b_FinalPlot_NormalisedSpectrum_wSiL3(outdir);
  Si16b_FinalPlot_NormalisedSpectrum_wAll(outdir);
}
