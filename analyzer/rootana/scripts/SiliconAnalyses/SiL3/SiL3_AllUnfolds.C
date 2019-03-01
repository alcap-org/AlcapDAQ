#include "scripts/SiliconAnalyses/SiL3/unfold/SiL3_Unfold_FlatBackground.C"
#include "scripts/SiliconAnalyses/SiL3/unfold/SiL3_Unfold_DecayElectronCorrection.C"
#include "scripts/SiliconAnalyses/SiL3/unfold/SiL3_Unfold_ResponseMatrix.C"
#include "scripts/SiliconAnalyses/SiL3/unfold/SiL3_Unfold_TimeCut.C"
#include "scripts/SiliconAnalyses/SiL3/unfold/SiL3_Unfold_GeometricAcceptance.C"

void SiL3_AllUnfolds() {

  std::string infilename = "~/data/results/SiL3/raw_spectra.root";
  std::string outfilename = "~/data/results/SiL3/unfold.root";
  //  std::string infilename = "~/data/results/SiL3/raw_spectra_wMuScCut_3000-3500ADC.root";
  //  std::string outfilename = "~/data/results/SiL3/unfold_wMuScCut_3000-3500ADC.root";
  //  std::string infilename = "~/data/results/SiL3/raw_spectra_special.root";
  //  std::string outfilename = "~/data/results/SiL3/unfold_special.root";

  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  double min_time_slice = 2000;
  double max_time_slice = 4000;
  double time_slice_step = 500;

  std::stringstream time_slice_str;
  for (double i_min_time_slice = min_time_slice; i_min_time_slice < max_time_slice; i_min_time_slice += time_slice_step) {
    double i_max_time_slice = i_min_time_slice + time_slice_step;

    time_slice_str.str("");
    time_slice_str << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;

    // Subtract flat background first
    std::string indirname = "RawSpectrum_fromEnergyTime_" + time_slice_str.str();
    std::string inhistname = indirname + "/hRawSpectrum";
    std::string intreename = indirname + "/cuttree";
    std::string corrdirname = "RawSpectrum_fromEnergyTime_FlatBkg";
    std::string corrhistname = corrdirname + "/hRawSpectrum";
    std::string corrtreename = corrdirname + "/cuttree";
    std::string outdirname = "FlatBkg_" + time_slice_str.str();
    SiL3_Unfold_FlatBackground(infilename, inhistname, intreename, infilename, corrhistname, corrtreename, outfilename, outdirname);

    // Now take care of decay electron background
    //    inhistname = outdirname + "/hCorrectedSpectrum";
    //    outdirname = "DecayElectronCorrection_" + time_slice_str.str();
    //    SiL3_Unfold_DecayElectronCorrection(outfilename, inhistname, outfilename, outdirname);

    // Now scale to account for the time cut
    std::string incuttreename = indirname + "/cuttree";
    inhistname = outdirname + "/hCorrectedSpectrum";
    outdirname = "TimeCut_" + time_slice_str.str();
    SiL3_Unfold_TimeCut(outfilename, outfilename, inhistname, infilename, incuttreename, outdirname);

    // Finally unfold the proton escape correction
    inhistname = outdirname + "/hCorrectedSpectrum";
    outdirname = "ProtonEscapeCorrection_" + time_slice_str.str();
    SiL3_Unfold_ResponseMatrix(outfilename, outfilename, inhistname, outdirname);
  }

  time_slice_str.str("");
  time_slice_str << "TimeSlice" << min_time_slice << "_" << max_time_slice;
  
  std::string indirname = "RawSpectrum_fromEnergyTime_" + time_slice_str.str();
  std::string inhistname = indirname + "/hRawSpectrum";
  std::string incuttreename = indirname + "/cuttree";
  std::string outdirname = "TimeCut_" + time_slice_str.str();
  SiL3_Unfold_TimeCut(infilename, outfilename, inhistname, infilename, incuttreename, outdirname);

  inhistname = outdirname + "/hCorrectedSpectrum";
  outdirname = "GeomAcceptance_" + time_slice_str.str();
  SiL3_Unfold_GeometricAcceptance(outfilename, outfilename, inhistname, outdirname, 1);

  inhistname = "TimeCut_" + time_slice_str.str() + "/hCorrectedSpectrum";
  outdirname = "ProtonEscapeCorrection_" + time_slice_str.str();
  SiL3_Unfold_ResponseMatrix(outfilename, outfilename, inhistname, outdirname);
}
