#include "unfold/SiL3_Unfold_FlatBackground.C"
#include "unfold/SiL3_Unfold_DecayElectronCorrection.C"
#include "unfold/SiL3_Unfold_ResponseMatrix.C"
#include "unfold/SiL3_Unfold_TimeCut.C"
#include "unfold/SiL3_Unfold_GeometricAcceptance.C"
#include "unfold/SiL3_Unfold_FinalNormalisation.C"

void SiL3_AllUnfolds() {

  //  std::string infilename = "~/data/results/SiL3/raw_spectra.root";
  //  std::string outfilename = "~/data/results/SiL3/unfold.root";
  //  std::string infilename = "~/data/results/SiL3/raw_spectra_wMuScCut_3000-3500ADC.root";
  //  std::string outfilename = "~/data/results/SiL3/unfold_wMuScCut_3000-3500ADC.root";
  std::string infilename = "~/data/results/SiL3/raw_spectra_geq2TgtPulse_newPP20us.root";
  std::string outfilename = "~/data/results/SiL3/unfold_geq2TgtPulse_newPP20us.root";
  //  std::string infilename = "~/data/results/SiL3/raw_spectra_v13.root";
  //  std::string outfilename = "~/data/results/SiL3/unfold_v13.root";

  std::string countfilename = "~/data/results/SiL3/normalisation_geq0TgtPulse_newPP20us.root";
  std::string counttreename = "XRaySpectrum_GeHiGain_2p1s_5000nsTimeCut/counttree";
  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  const int n_time_slices = 5;
  double min_time_slices[n_time_slices] = {2000, 2500, 3000, 3500, 2000};
  double max_time_slices[n_time_slices] = {2500, 3000, 3500, 4000, 4000};

  const int n_recoils = 9;
  std::string recoil_names[n_recoils] = {"noRecoil", "nuRecoil", "pRecoil", "dRecoil", "tRecoil", "aRecoil", "allRecoil", "lowAllRecoil", "highAllRecoil"};

  for (int i_recoil = 0; i_recoil < n_recoils; ++i_recoil) {
    std::string recoil_name = recoil_names[i_recoil];
    
    std::stringstream time_slice_str;
    for (int i_time_slice = 0; i_time_slice < n_time_slices; ++i_time_slice) {
      double i_min_time_slice = min_time_slices[i_time_slice];
      double i_max_time_slice = max_time_slices[i_time_slice];

      time_slice_str.str("");
      time_slice_str << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;

      // Subtract flat background first
      std::string indirname = "SiL3_ActiveTarget_" + time_slice_str.str() + "_" + recoil_name;
      std::string inhistname = indirname + "/hRawSpectrum";
      std::string cutfilename = infilename;
      std::string cuttreename = indirname + "/cuttree";
      std::string corrfilename = infilename;
      //    std::string corrdirname = "SiL3_ActiveTarget_FlatBkg";
      std::string corrdirname = "SiL3_ActiveTarget_" + time_slice_str.str() + "_FlatBkg_noRecoil";
      std::string corrhistname = corrdirname + "/hRawSpectrum";
      std::string corrtreename = corrdirname + "/cuttree";
      std::string outdirname = "FlatBkg_" + time_slice_str.str() + "_" + recoil_name;
      SiL3_Unfold_FlatBackground(infilename, inhistname, cutfilename, cuttreename, corrfilename, corrhistname, corrtreename, outfilename, outdirname);

      // Now scale to account for a false time cut
      /*    double decay_lifetime = 680;
	    std::string incuttreename = indirname + "/cuttree";
	    inhistname = outdirname + "/hCorrectedSpectrum";
	    outdirname = "FalseTimeCut_" + time_slice_str.str();
	    SiL3_Unfold_TimeCut(outfilename, outfilename, inhistname, infilename, incuttreename, outdirname, decay_lifetime);
	    //    SiL3_Unfold_TimeCut(infilename, outfilename, inhistname, infilename, incuttreename, outdirname);
	    */
    
      // Now scale to account for the time cut
      double decay_lifetime = 756;
      std::string incuttreename = indirname + "/cuttree";
      inhistname = outdirname + "/hCorrectedSpectrum";
      outdirname = "TimeCut_" + time_slice_str.str() + "_" + recoil_name;
      SiL3_Unfold_TimeCut(outfilename, outfilename, inhistname, infilename, incuttreename, outdirname, decay_lifetime);
      
      /*
      // Continue with correct time cut
      inhistname = outdirname + "/hCorrectedSpectrum";
      outdirname = "GeomAcceptance_" + time_slice_str.str();
      SiL3_Unfold_GeometricAcceptance(outfilename, outfilename, inhistname, outdirname, 1);
      */
    
      // Now take care of decay electron background
      inhistname = outdirname + "/hCorrectedSpectrum";
      //    corrfilename = "~/data/mc/SiL3/decayCorr_1M_Geom-P1_electrons-45-55MeV.root";
      //    corrfilename = "~/data/mc/SiL3/decayCorr_100k_Geom-P1_muplus_w1300umSiL3.root";
      //    corrfilename = "~/data/mc/SiL3/decayCorr_100k_Geom-P1_muplus_w1475umSiL3.root";
      //    corrfilename = "~/data/mc/SiL3/decayCorr_100k_Geom-P1_muplus_w2000umSiL3.root";
      //    corrfilename = "~/data/mc/SiL3/decayCorr_100k_Geom-P1_muplus_w1475umSiL3_wKFactor0-9.root";
      //    corrfilename = "~/data/mc/SiL3/decayCorr_test.root";
      //        corrfilename = "~/data/mc/SiL3/decayCorr_10M_Geom-P1_muplus.root";
      corrfilename = "~/data/mc/SiL3/decayCorr_1M_Geom-P1_muplus.root";
      corrhistname = "hEDep_muplus";
      outdirname = "DecayElectron_" + time_slice_str.str() + "_" + recoil_name;
      SiL3_Unfold_DecayElectronCorrection(outfilename, inhistname, corrfilename, corrhistname, countfilename, counttreename, outfilename, outdirname);
      
      
      // Finally unfold the proton escape correction    
      inhistname = outdirname + "/hCorrectedSpectrum";
      outdirname = "ProtonEscape_" + time_slice_str.str() + "_" + recoil_name;
      SiL3_Unfold_ResponseMatrix(outfilename, outfilename, inhistname, outdirname, "proton");

      inhistname = "DecayElectron_" + time_slice_str.str() + "_" + recoil_name + "/hCorrectedSpectrum";
      outdirname = "DeuteronEscape_" + time_slice_str.str() + "_" + recoil_name;
      SiL3_Unfold_ResponseMatrix(outfilename, outfilename, inhistname, outdirname, "deuteron");
      
      inhistname = "DecayElectron_" + time_slice_str.str() + "_" + recoil_name + "/hCorrectedSpectrum";
      outdirname = "TritonEscape_" + time_slice_str.str() + "_" + recoil_name;
      SiL3_Unfold_ResponseMatrix(outfilename, outfilename, inhistname, outdirname, "triton");
      
      inhistname = "DecayElectron_" + time_slice_str.str() + "_" + recoil_name + "/hCorrectedSpectrum";
      outdirname = "AlphaEscape_" + time_slice_str.str() + "_" + recoil_name;
      SiL3_Unfold_ResponseMatrix(outfilename, outfilename, inhistname, outdirname, "alpha");      
      
      inhistname = "DecayElectron_" + time_slice_str.str()  + "_" + recoil_name + "/hCorrectedSpectrum";
      outdirname = "CombinedEscape_" + time_slice_str.str() + "_" + recoil_name;
      SiL3_Unfold_ResponseMatrix(outfilename, outfilename, inhistname, outdirname, "combined");
      
      inhistname = outdirname + "/hCorrectedSpectrum";
      outdirname = "FinalNormalisation_" + time_slice_str.str() + "_" + recoil_name;
      SiL3_Unfold_FinalNormalisation(outfilename, inhistname, countfilename, counttreename, outfilename, outdirname);
      
    }
  }
}
