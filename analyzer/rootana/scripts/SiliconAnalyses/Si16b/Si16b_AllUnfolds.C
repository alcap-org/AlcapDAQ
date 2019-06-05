#include "scripts/SiliconAnalyses/Si16b/unfold/Si16b_Unfold_ResponseMatrix.C"
#include "scripts/SiliconAnalyses/Si16b/unfold/Si16b_Unfold_TimeCut.C"
#include "scripts/SiliconAnalyses/Si16b/unfold/Si16b_Unfold_FlatBackground.C"
#include "scripts/SiliconAnalyses/Si16b/unfold/Si16b_Unfold_DecayElectronCorrection.C"
#include "scripts/SiliconAnalyses/Si16b/unfold/Si16b_Unfold_GeometricAcceptance.C"

void Si16b_AllUnfolds() {

  //  gROOT->ProcessLine(".L scripts/Unfolding/bayesian-unfolding/libRooUnfold.so");

  std::string infilename = "~/data/results/Si16b/raw_spectra.root";
  std::string outfilename = "~/data/results/Si16b/unfold.root";


  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  // For protons, deuterons etc
  const int n_particles = 5;
  std::string particle_names[n_particles] = {"proton", "deuteron", "triton", "alpha", "proton_combined"};
  TH1D* hSum = 0;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string i_particle_name = particle_names[i_particle];

    std::string inhistname = i_particle_name + "/hRawSpectrum";
    std::string incuttreename = i_particle_name + "/cuttree";
    std::string outdirname = "TimeCut_" + i_particle_name;
    double decay_lifetime = 756;
    Si16b_Unfold_TimeCut(infilename, outfilename, inhistname, infilename, incuttreename, outdirname, decay_lifetime);

    std::string datahistname = outdirname + "/hCorrectedSpectrum";
    std::string mcfilename = "~/data/mc/Si16b/R15b_Si16b_response-matrix_500keV-bins_" + i_particle_name + "s_new_wBugFix.root";
    std::string mcresponsename = "SiR_response";
    if (i_particle == 0) {
      mcresponsename = "SiR_two_layer_response";
    }
    int rebin_factor = 50;
    outdirname = "ResponseMatrix_" + i_particle_name;
    Si16b_Unfold_ResponseMatrix(outfilename, outfilename, datahistname, mcfilename, mcresponsename, rebin_factor, outdirname);

    // Sum the histograms for a total
    if (i_particle < 4) {
      std::string final_histname = "ResponseMatrix_" + i_particle_name + "/hUnfoldedSpectrum";
      TFile* outfile2 = new TFile(outfilename.c_str(), "UPDATE");
      TH1D* final_hist = (TH1D*) outfile2->Get(final_histname.c_str());
      if (!final_hist) {
	std::cout << "Error: Problem getting histogram " << final_histname << std::endl;
	return;
      }
      if (!hSum) {
	hSum = (TH1D*) final_hist->Clone("hUnfoldedSpectrum");
	hSum->SetDirectory(0);
      }
      else {
	hSum->Add(final_hist);
      }
      outfile2->Close();
    }
  }

  TFile* outfile2 = new TFile(outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile2->mkdir("AllParticles");
  outdir->cd();
  hSum->Write();
  outfile2->Write();
  outfile2->Close();


  const int n_time_slices = 3;
  double min_time_slices[n_time_slices] = {1000, 2000, 1000};
  double max_time_slices[n_time_slices] = {2000, 5000, 5000};
  //  const int n_time_slices = 5;
  //  double min_time_slices[n_time_slices] = {1000, 2000, 3000, 4000, 1000};
  //  double max_time_slices[n_time_slices] = {2000, 3000, 4000, 5000, 5000};
  //  const int n_time_slices = 10;
  //  double min_time_slices[n_time_slices] = {500,  1500, 2500, 3500, 4500, 5500, 6500, 7500, 8500, 500};
  //  double max_time_slices[n_time_slices] = {1500, 2500, 3500, 4500, 5500, 6500, 7500, 8500, 9500, 15000};

  const int n_detectors = 2;
  std::string detectors[n_detectors] = {"SiL3", "SiR2"};
  double geom_acceptances[n_detectors] = {0.01002, 0.01074};
  
  std::stringstream time_slice_str;
  for (int i_time_slice = 0; i_time_slice < n_time_slices; ++i_time_slice) {
    double i_min_time_slice = min_time_slices[i_time_slice];
    double i_max_time_slice = max_time_slices[i_time_slice];

    time_slice_str.str("");
    time_slice_str << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;

    for (int i_detector = 0; i_detector < n_detectors; ++i_detector) {
      std::string detector = detectors[i_detector];
      double geom_acceptance = geom_acceptances[i_detector];

      std::cout << "AllUnfolds: " << detector << " " << time_slice_str.str() << std::endl;
      
      // Subtract flat background first
      std::string indirname = detector + "_" + time_slice_str.str();
      std::string inhistname = indirname + "/hRawSpectrum";
      std::string cutfilename = infilename;
      std::string cuttreename = indirname + "/cuttree";
      std::string corrfilename = infilename;
      //      std::string corrdirname = detector + "_FlatBkg";
      std::string corrdirname = detector + "_" + time_slice_str.str() + "_FlatBkg";
      std::string corrhistname = corrdirname + "/hRawSpectrum";
      std::string corrtreename = corrdirname + "/cuttree";
      std::string outdirname = detector + "_FlatBkg_" + time_slice_str.str();
      Si16b_Unfold_FlatBackground(infilename, inhistname, cutfilename, cuttreename, corrfilename, corrhistname, corrtreename, outfilename, outdirname);

      // Now scale to account for the time cut
      std::string incuttreename = indirname + "/cuttree";
      inhistname = outdirname + "/hCorrectedSpectrum";
      double decay_lifetime = 775;
      outdirname = detector + "_FalseTimeCut_" + time_slice_str.str();
      Si16b_Unfold_TimeCut(outfilename, outfilename, inhistname, infilename, incuttreename, outdirname, decay_lifetime);

      decay_lifetime = 756;
      outdirname = detector + "_TimeCut_" + time_slice_str.str();
      Si16b_Unfold_TimeCut(outfilename, outfilename, inhistname, infilename, incuttreename, outdirname, decay_lifetime);
      
      inhistname = outdirname + "/hCorrectedSpectrum";
      outdirname = detector + "_GeomAcceptance_" + time_slice_str.str();
      Si16b_Unfold_GeometricAcceptance(outfilename, outfilename, inhistname, outdirname, geom_acceptance);

      // decay electron correction
      inhistname = outdirname + "/hCorrectedSpectrum";
      corrfilename = "~/data/mc/Si16b/decayCorr_test2.root";
      corrhistname = "hEDep_muplus";
      std::string countfilename = "~/data/results/Si16b/normalisation.root";
      std::string counttreename = "XRaySpectrum_GeLoGain_noTimeCut/counttree";
      outdirname = detector + "_DecayElectron_" + time_slice_str.str();
      Si16b_Unfold_DecayElectronCorrection(outfilename, inhistname, corrfilename, corrhistname, countfilename, counttreename, outfilename, outdirname);
    }
    /*
    std::string datahistname = "TimeCut_SiL3/hCorrectedSpectrum";
    std::string mcfilename = "~/data/mc/Si16b/R15b_Si16b_response-matrix_500keV-bins_proton_combineds_new_wBugFix.root";
    std::string mcresponsename = "SiL_two_layer_response";
    int rebin_factor = 12;
    outdirname = "ResponseMatrix_SiL3";
    Si16b_Unfold_ResponseMatrix(outfilename, outfilename, datahistname, mcfilename, mcresponsename, rebin_factor, outdirname);
    */

    /*
    // For SiR2 analysis
    indirname = "SiR2_Michels_TimeSlice1000_5000";
    inhistname = indirname + "/hRawSpectrum";
    cutfilename = infilename;
    cuttreename = indirname + "/cuttree";
    corrfilename = infilename;
    corrhistname = "SiR2_Michels_FlatBkg/hRawSpectrum";
    corrtreename = "SiR2_Michels_FlatBkg/cuttree";
    outdirname = "FlatBkg_SiR2_Michels";
    Si16b_Unfold_FlatBackground(infilename, inhistname, cutfilename, cuttreename, corrfilename, corrhistname, corrtreename, outfilename, outdirname);
    
    inhistname = outdirname + "/hCorrectedSpectrum";
    outdirname = "TimeCut_SiR2_Michels";
    Si16b_Unfold_TimeCut(outfilename, outfilename, inhistname, infilename, cuttreename, outdirname);

  
    indirname = "SiR2_" + time_slice_str.str();
    inhistname = indirname + "/hRawSpectrum";
    cutfilename = infilename;
    cuttreename = indirname + "/cuttree";
    corrfilename = infilename;
    corrhistname = "SiR2_FlatBkg/hRawSpectrum";
    corrtreename = "SiR2_FlatBkg/cuttree";
    outdirname = "FlatBkg_SiR2";
    Si16b_Unfold_FlatBackground(infilename, inhistname, cutfilename, cuttreename, corrfilename, corrhistname, corrtreename, outfilename, outdirname);
    
    inhistname = outdirname + "/hCorrectedSpectrum";
    corrfilename = outfilename;
    corrhistname = "FlatBkg_SiR2_Michels/hCorrectedSpectrum";
    outdirname = "DecayCorrection_SiR2";
    Si16b_Unfold_DecayElectronCorrection(outfilename, inhistname, corrfilename, corrhistname, outfilename, outdirname);
    
    inhistname = outdirname + "/hCorrectedSpectrum";
    outdirname = "TimeCut_SiR2";
    Si16b_Unfold_TimeCut(outfilename, outfilename, inhistname, infilename, cuttreename, outdirname);
    
    inhistname = outdirname + "/hCorrectedSpectrum";
    outdirname = "GeomAcceptance_SiR2";
    Si16b_Unfold_GeometricAcceptance(outfilename, outfilename, inhistname, outdirname, 0.01074);
    
    datahistname = "TimeCut_SiR2/hCorrectedSpectrum";
    //  mcfilename = "~/data/mc/Si16b/R15b_Si16b_response-matrix_500keV-bins_protons_new_wBugFix.root";
    mcfilename = "~/data/mc/Si16b/R15b_Si16b_response-matrix_500keV-bins_protons-SWSpec.root";
    mcresponsename = "SiR_middle_layer_response";
    rebin_factor = 2;
    outdirname = "ResponseMatrix_SiR2";
    Si16b_Unfold_ResponseMatrix(outfilename, outfilename, datahistname, mcfilename, mcresponsename, rebin_factor, outdirname);
    */
  }
}
