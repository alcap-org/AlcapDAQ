#include "scripts/SiliconAnalyses/SiL3/unfold/SiL3_Unfold_FlatBackground.C"
#include "scripts/SiliconAnalyses/SiL3/unfold/SiL3_Unfold_DecayElectronCorrection.C"
#include "scripts/SiliconAnalyses/SiL3/unfold/SiL3_Unfold_ResponseMatrix.C"
#include "scripts/SiliconAnalyses/SiL3/unfold/SiL3_Unfold_TimeCut.C"
#include "scripts/SiliconAnalyses/SiL3/unfold/SiL3_Unfold_GeometricAcceptance.C"
#include "scripts/SiliconAnalyses/SiL3/unfold/SiL3_Unfold_FinalNormalisation.C"

void SiL3_AllUnfolds() {

  //  std::string infilename = "~/data/results/SiL3/raw_spectra.root";
  //  std::string outfilename = "~/data/results/SiL3/unfold.root";
  //  std::string infilename = "~/data/results/SiL3/raw_spectra_wMuScCut_3000-3500ADC.root";
  //  std::string outfilename = "~/data/results/SiL3/unfold_wMuScCut_3000-3500ADC.root";
  std::string infilename = "~/data/results/SiL3/raw_spectra_geq2TgtPulse.root";
  std::string outfilename = "~/data/results/SiL3/unfold_geq2TgtPulse.root";
  //  std::string infilename = "~/data/results/SiL3/raw_spectra_v13.root";
  //  std::string outfilename = "~/data/results/SiL3/unfold_v13.root";

  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  const int n_time_slices = 6;
  double min_time_slices[n_time_slices] = {2000, 2500, 3000, 3500, 2000, 6000};
  double max_time_slices[n_time_slices] = {2500, 3000, 3500, 4000, 4000, 20000};

  std::stringstream time_slice_str;
  for (int i_time_slice = 0; i_time_slice < n_time_slices; ++i_time_slice) {
    double i_min_time_slice = min_time_slices[i_time_slice];
    double i_max_time_slice = max_time_slices[i_time_slice];

    time_slice_str.str("");
    time_slice_str << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;

    // Subtract flat background first
    std::string indirname = "SiL3_ActiveTarget_" + time_slice_str.str();
    std::string inhistname = indirname + "/hRawSpectrum";
    std::string cutfilename = infilename;
    std::string cuttreename = indirname + "/cuttree";
    std::string corrfilename = infilename;
    std::string corrdirname = "SiL3_ActiveTarget_FlatBkg";
    //    std::string corrdirname = "SiL3_ActiveTarget_" + time_slice_str.str() + "_FlatBkg";
    std::string corrhistname = corrdirname + "/hRawSpectrum";
    std::string corrtreename = corrdirname + "/cuttree";
    std::string outdirname = "FlatBkg_" + time_slice_str.str();
    SiL3_Unfold_FlatBackground(infilename, inhistname, cutfilename, cuttreename, corrfilename, corrhistname, corrtreename, outfilename, outdirname);

    // Now scale to account for a false time cut
    double decay_lifetime = 680;
    std::string incuttreename = indirname + "/cuttree";
    inhistname = outdirname + "/hCorrectedSpectrum";
    outdirname = "FalseTimeCut_" + time_slice_str.str();
    SiL3_Unfold_TimeCut(outfilename, outfilename, inhistname, infilename, incuttreename, outdirname, decay_lifetime);
    //    SiL3_Unfold_TimeCut(infilename, outfilename, inhistname, infilename, incuttreename, outdirname);

    // Now scale to account for a false time cut
    decay_lifetime = 756;
    outdirname = "TimeCut_" + time_slice_str.str();
    SiL3_Unfold_TimeCut(outfilename, outfilename, inhistname, infilename, incuttreename, outdirname, decay_lifetime);

    // Continue with correct time cut
    inhistname = outdirname + "/hCorrectedSpectrum";
    outdirname = "GeomAcceptance_" + time_slice_str.str();
    SiL3_Unfold_GeometricAcceptance(outfilename, outfilename, inhistname, outdirname, 1);

    
    // Now take care of decay electron background
    inhistname = outdirname + "/hCorrectedSpectrum";
    //    corrfilename = "~/data/mc/SiL3/decayCorr_1M_Geom-P1_electrons-45-55MeV.root";
    corrfilename = "~/data/mc/SiL3/decayCorr_10M_Geom-P1_muplus.root";
    //    corrfilename = "~/data/mc/SiL3/decayCorr_100k_Geom-P1_muplus_w1300umSiL3.root";
    //    corrfilename = "~/data/mc/SiL3/decayCorr_100k_Geom-P1_muplus_w1475umSiL3.root";
    //    corrfilename = "~/data/mc/SiL3/decayCorr_100k_Geom-P1_muplus_w2000umSiL3.root";
    //    corrfilename = "~/data/mc/SiL3/decayCorr_100k_Geom-P1_muplus_w1475umSiL3_wKFactor0-9.root";
    //    corrfilename = "~/data/mc/SiL3/decayCorr_test.root";
    corrhistname = "hEDep_muplus";
    std::string countfilename = "~/data/results/SiL3/normalisation_geq1TgtPulse.root";
    std::string counttreename = "XRaySpectrum_GeLoGain_noTimeCut/counttree";
    outdirname = "DecayElectron_" + time_slice_str.str();
    SiL3_Unfold_DecayElectronCorrection(outfilename, inhistname, corrfilename, corrhistname, countfilename, counttreename, outfilename, outdirname);

    inhistname = outdirname + "/hCorrectedSpectrum";
    outdirname = "FinalNormalisation_" + time_slice_str.str();
    SiL3_Unfold_FinalNormalisation(outfilename, inhistname, countfilename, counttreename, outfilename, outdirname);
    
    /*
    // Finally unfold the proton escape correction
    inhistname = "TimeCut_" + time_slice_str.str() + "/hCorrectedSpectrum";
    outdirname = "ProtonEscapeCorrection_" + time_slice_str.str();
    SiL3_Unfold_ResponseMatrix(outfilename, outfilename, inhistname, outdirname);
    */
  }
}
