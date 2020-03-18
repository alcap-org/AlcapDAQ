#include <fstream>

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
#include "Si16b/final/Si16b_FinalPlot_ResponseMatrix_ELoss.C"
#include "Si16b/final/Si16b_FinalPlot_GeometricAcceptances.C"
#include "Si16b/final/Si16b_FinalPlot_UnfoldedAndFoldedSpectra.C"
#include "Si16b/final/Si16b_FinalPlot_TwoLayer_EvdE_TCutG_2sigma.C"
#include "Si16b/final/Si16b_FinalPlot_TwoLayer_EvdE_TCutG_1sigma.C"
#include "Si16b/final/Si16b_FinalPlot_ThreeLayer_EvdE_TCutG_2sigma.C"
#include "Si16b/final/Si16b_FinalPlot_ThreeLayer_EvdE_TCutG_1sigma.C"
#include "Si16b/final/Si16b_FinalPlot_Systematic_PIDCut.C"
#include "Si16b/final/Si16b_FinalPlot_Systematic_UnfoldingIterations.C"
#include "Si16b/final/Si16b_FinalPlot_Systematic_UnfoldingBinWidth.C"
#include "Si16b/final/Si16b_FinalPlot_DataVsMC_ThinEnergy.C"
#include "Si16b/final/Si16b_FinalPlot_Systematic_MuonStopPositions.C"
#include "Si16b/final/Si16b_FinalPlot_Systematic_SiRPosition.C"
#include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum.C"
#include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum_wSW.C"
#include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum_wBudyashov.C"
#include "Si16b/final/Si16b_FinalPlot_Systematic_IndividualComponents.C"

#include "SiL3/final/SiL3_FinalPlot_Target_RawSpectrum.C"
#include "SiL3/final/SiL3_FinalPlot_XRaySpectrum_wFit.C"
#include "SiL3/final/SiL3_FinalPlot_XRaySpectra_TimeCut.C"
#include "SiL3/final/SiL3_FinalPlot_TargetSpectrum_EnergyTime.C"
#include "SiL3/final/SiL3_FinalPlot_Lifetime.C"
#include "SiL3/final/SiL3_FinalPlot_LifetimeComparison.C"
#include "SiL3/final/SiL3_FinalPlot_RawSpectrum.C"
#include "SiL3/final/SiL3_FinalPlot_RawSpectrum_RecoilComparison.C"
#include "SiL3/final/SiL3_FinalPlot_FlatBkgCorrection.C"
#include "SiL3/final/SiL3_FinalPlot_DecayElectronCorrection.C"
#include "SiL3/final/SiL3_FinalPlot_ResponseMatrix.C"
#include "SiL3/final/SiL3_FinalPlot_EscapeCorrection.C"
#include "SiL3/final/SiL3_FinalPlot_RawSpectrum_RecoilComparisonSystematic.C"
#include "SiL3/final/SiL3_FinalPlot_ResponseMatrix_Systematics.C"
#include "SiL3/final/SiL3_FinalPlot_Systematic_TimeSlice.C"
#include "SiL3/final/SiL3_FinalPlot_Systematic_ParticleComposition.C"
#include "SiL3/final/SiL3_FinalPlot_Systematic_UnfoldingRegParameter.C"
#include "SiL3/final/SiL3_FinalPlot_DataVsMC_BeamEnergy.C"
#include "SiL3/final/SiL3_FinalPlot_DecayElectronCorrection_MuStopPosSystematicComparison.C"
#include "SiL3/final/SiL3_FinalPlot_ResponseMatrix_Systematics_MuStopPos.C"
#include "SiL3/final/SiL3_FinalPlot_Systematic_MuonStopPositions.C"
#include "SiL3/final/SiL3_FinalPlot_NormalisedSpectrum.C"
#include "SiL3/final/SiL3_FinalPlot_Systematic_IndividualComponents.C"
#include "SiL3/final/SiL3_FinalPlot_NormalisedSpectrum_wSW.C"

#include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum_wSiL3.C"
#include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum_wSiL3_RecoilComparison.C"
#include "Si16b/final/Si16b_FinalPlot_NormalisedSpectrum_wAll.C"

#include "Si16b/final/Si16b_FinalPlot_SpectrumFit.C"

#include "Si16a/final/Si16a_FinalPlot_TargetSpectrum_ThinEnergy.C"
#include "Si16a/final/Si16a_FinalPlot_TargetSpectrum_ThickEnergy.C"
#include "../MuonBeamSimulation/DataVsMC_BeamPositions.C"
#include "Si16a/final/Si16a_FinalPlot_DataVsMC_BeamEnergy_ThinAll.C"

void PrintAnalysisNotePlots() {

  std::string outdir_base = "~/plots/analysis_note/";
  std::ofstream numbers_file("/home/edmonds/plots/analysis_note/silicon/figs/silicon-numbers.tex", std::ofstream::out);
  if (!numbers_file.is_open()) {
    std::cout << "ERROR with numbers_file" << std::endl;
    return;
  }
  
  std::string outdir = outdir_base + "silicon/figs/";
  Si16b_FinalPlot_Budyashov(outdir);
  Si16b_FinalPlot_Target_RawSpectrum(outdir);
  Si16b_FinalPlot_XRaySpectrum_wFit(outdir, numbers_file);
  Si16b_FinalPlot_XRaySpectrum_TimeCuts(outdir, numbers_file);
  Si16b_FinalPlot_TwoLayer_EvdE_TCutG(outdir, numbers_file);
  Si16b_FinalPlot_ThreeLayer_EvdE_TCutG(outdir);
  Si16b_FinalPlot_TwoLayer_TDiff(outdir, numbers_file);
  Si16b_FinalPlot_TwoLayer_Lifetime(outdir);
  Si16b_FinalPlot_TimeCutEfficiency(outdir, numbers_file);
  Si16b_FinalPlot_SiR3VetoEfficiency(outdir, numbers_file);
  Si16b_FinalPlot_RawSpectra_SiR3Veto(outdir);
  Si16b_FinalPlot_ResponseMatrix(outdir);
  Si16b_FinalPlot_ResponseMatrix_ELoss(outdir);
  Si16b_FinalPlot_GeometricAcceptances(outdir, numbers_file);
  Si16b_FinalPlot_UnfoldedAndFoldedSpectra(outdir);
  Si16b_FinalPlot_TwoLayer_EvdE_TCutG_2sigma(outdir);
  Si16b_FinalPlot_TwoLayer_EvdE_TCutG_1sigma(outdir);
  Si16b_FinalPlot_ThreeLayer_EvdE_TCutG_2sigma(outdir);
  Si16b_FinalPlot_ThreeLayer_EvdE_TCutG_1sigma(outdir);
  Si16b_FinalPlot_Systematic_PIDCut(outdir);
  Si16b_FinalPlot_Systematic_UnfoldingIterations(outdir);
  Si16b_FinalPlot_Systematic_MuonStopPositions(outdir);
  Si16b_FinalPlot_DataVsMC_ThinEnergy(outdir);
  Si16b_FinalPlot_Systematic_UnfoldingBinWidth(outdir);
  Si16b_FinalPlot_Systematic_SiRPosition(outdir);
  Si16b_FinalPlot_NormalisedSpectrum(outdir, numbers_file);
  Si16b_FinalPlot_NormalisedSpectrum_wSW(outdir);
  Si16b_FinalPlot_NormalisedSpectrum_wBudyashov(outdir);
  Si16b_FinalPlot_Systematic_IndividualComponents(outdir, numbers_file);

  SiL3_FinalPlot_Target_RawSpectrum(outdir);
  SiL3_FinalPlot_XRaySpectrum_wFit(outdir, numbers_file);
  SiL3_FinalPlot_XRaySpectra_TimeCut(outdir, numbers_file);
  SiL3_FinalPlot_TargetSpectrum_EnergyTime(outdir, numbers_file);
  SiL3_FinalPlot_Lifetime(outdir, numbers_file);
  SiL3_FinalPlot_LifetimeComparison(outdir);
  SiL3_FinalPlot_RawSpectrum(outdir);
  SiL3_FinalPlot_RawSpectrum_RecoilComparison(outdir, numbers_file);
  SiL3_FinalPlot_FlatBkgCorrection(outdir, numbers_file);
  SiL3_FinalPlot_DecayElectronCorrection(outdir);
  SiL3_FinalPlot_ResponseMatrix(outdir);
  SiL3_FinalPlot_EscapeCorrection(outdir);
  SiL3_FinalPlot_RawSpectrum_RecoilComparisonSystematic(outdir, numbers_file);
  SiL3_FinalPlot_ResponseMatrix_Systematics(outdir);
  SiL3_FinalPlot_Systematic_TimeSlice(outdir);
  SiL3_FinalPlot_Systematic_ParticleComposition(outdir);
  SiL3_FinalPlot_Systematic_UnfoldingRegParameter(outdir);
  SiL3_FinalPlot_DataVsMC_BeamEnergy(outdir);
  SiL3_FinalPlot_DecayElectronCorrection_MuStopPosSystematicComparison(outdir);
  SiL3_FinalPlot_ResponseMatrix_Systematics_MuStopPos(outdir);
  SiL3_FinalPlot_Systematic_MuonStopPositions(outdir);
  SiL3_FinalPlot_NormalisedSpectrum(outdir, numbers_file);
  SiL3_FinalPlot_Systematic_IndividualComponents(outdir, numbers_file);
  SiL3_FinalPlot_NormalisedSpectrum_wSW(outdir);

  Si16b_FinalPlot_NormalisedSpectrum_wSiL3(outdir);
  Si16b_FinalPlot_NormalisedSpectrum_wSiL3_RecoilComparison(outdir);
  Si16b_FinalPlot_NormalisedSpectrum_wAll(outdir, numbers_file);

  Si16b_FinalPlot_SpectrumFit(outdir, numbers_file);


  outdir = outdir_base + "simulation/figs/";
  Si16a_FinalPlot_TargetSpectrum_ThinEnergy(outdir);
  Si16a_FinalPlot_TargetSpectrum_ThickEnergy(outdir);
  DataVsMC_BeamPositions(outdir);
  Si16a_FinalPlot_DataVsMC_BeamEnergy_ThinAll(outdir);
  
  numbers_file.close();
}
