#include "unfold/Si16b_Unfold_ResponseMatrix.C"
#include "unfold/Si16b_Unfold_TimeCut.C"
#include "unfold/Si16b_Unfold_FlatBackground.C"
#include "unfold/Si16b_Unfold_DecayElectronCorrection.C"
#include "unfold/Si16b_Unfold_GeometricAcceptance.C"
#include "unfold/Si16b_Unfold_FinalNormalisation.C"
#include "unfold/Si16b_Unfold_PIDCutEfficiency.C"

void Si16b_AllUnfolds() {

  gROOT->ProcessLine(".L ~/libRooUnfold.so");

  std::string infilename = "~/data/results/Si16b/raw_spectra_newPP_geq1TgtPulse_2.root";
  std::string outfilename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_2.root";
  std::string countfilename = "~/data/results/Si16b/normalisation_newPP_geq1TgtPulse_1.root";
  std::string counttreename = "XRaySpectrum_GeHiGain_2p1s_5000nsTimeCut/counttree";
      
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  // Have two different PID methods
  const int n_PIDs = 1;
  std::string PIDs[n_PIDs] = {"TCutG"};//, "PSel"};

  const int n_sigmas = 3;
  std::string sigmas[n_sigmas] = {"3", "2", "1"};
  double sigma_efficiencies[n_sigmas] = {0.997, 0.95, 0.68};
  //  double sigma_efficiency = 0.997;//3sigma
  //  double sigma_efficiency = 0.95;//2sigma
  //  double sigma_efficiency = 0.68;//1sigma
  //  double pid_efficiencies[n_PIDs] = {0.9779, 0.986};

  const int n_layer_coincs = 2;
  std::string layer_coincs[n_layer_coincs] = {"200ns", "500ns"};
  
  const int n_timecuts = 2;
  std::string timecuts[n_timecuts] = {"400ns", "0ns"};
  double min_times[n_timecuts] = {400, 0};

  const int n_bin_widths = 3;
  std::string bin_width_tags[n_bin_widths] = {"1MeV", "500keV", "100keV"};
  double bin_width_vals[n_bin_widths] = {1, 0.5, 0.1};
  
  // For protons, deuterons etc
  const int n_particles = 4;
  std::string particle_names[n_particles] = {"proton", "deuteron", "triton", "alpha"};//, "proton_combined"};
  double highE_cuts[n_particles] = {10000,10000,10000,10000};//{25, 25, 25, 25};

  for (int i_PID = 0; i_PID < n_PIDs; ++i_PID) {
    std::string i_pid = PIDs[i_PID];
    //    double i_pid_efficiency = pid_efficiencies[i_PID];
    
    for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
      std::string i_particle_name = particle_names[i_particle];
      double highE_cut = highE_cuts[i_particle];
      
      for (int i_sigma = 0; i_sigma < n_sigmas; ++i_sigma) {
	std::string sigma = sigmas[i_sigma];
	double sigma_efficiency = sigma_efficiencies[i_sigma];

	for (int i_layer_coinc = 0; i_layer_coinc < n_layer_coincs; ++i_layer_coinc) {
	  std::string layer_coinc = layer_coincs[i_layer_coinc];

	  for (int i_timecut = 0; i_timecut < n_timecuts; ++i_timecut) {
	    std::string timecut = timecuts[i_timecut];

	    //      std::string indirname = i_particle_name + "_" + i_pid;
	    std::string indirname = i_particle_name + "_" + i_pid + "_" + sigma + "sig_layerCoinc" + layer_coinc + "_tGT" + timecut;
	    std::cout << indirname << std::endl;
	    std::string inhistname = indirname + "/hRawSpectrum";

	    std::string outdirname = "PIDCut_" + i_particle_name + "_" + i_pid + "_" + sigma + "sig_layerCoinc" + layer_coinc + "_tGT" + timecut;
	    // std::string corrfilename = "~/data/mc/Si16b/pid-effs.root";
	    // if (i_pid == "PSel") { // only do this for PSel
	    //   int rebin_factor = 100;//50;
	    //   Si16b_Unfold_PIDCutEfficiency(infilename, outfilename, inhistname, outdirname, corrfilename, i_particle_name, rebin_factor);
	    // }
	
	    std::string incuttreename = indirname + "/cuttree";
	    double decay_lifetime = 756;
	    std::string branch = "thick_time_cut";
	    double extra_efficiency = 0.941;
	    std::string datafilename = "";
	    if (i_pid == "TCutG") { // do PID cut correction here
	      extra_efficiency *= sigma_efficiency; // PID cut
	      datafilename = infilename;
	    }
	    else if (i_pid == "PSel") {
	      datafilename = outfilename;
	      inhistname = outdirname + "/hCorrectedSpectrum";
	    }
	    outdirname = "TimeCut_" + i_particle_name + "_" + i_pid + "_" + sigma + "sig_layerCoinc" + layer_coinc + "_tGT" + timecut;
	    Si16b_Unfold_TimeCut(datafilename, outfilename, inhistname, infilename, incuttreename, outdirname, decay_lifetime, branch, extra_efficiency);
	    //      Si16b_Unfold_TimeCut(infilename, outfilename, inhistname, infilename, incuttreename, outdirname, decay_lifetime, branch, extra_efficiency);
      
	    std::string datahistname = outdirname + "/hCorrectedSpectrum";
	    //    std::string mcfilename = "~/data/mc/Si16b/R15b_Si16b_response-matrix_500keV-bins_" + i_particle_name + "s_new_wBugFix.root";
	    //    std::string mcfilename = "~/data/mc/Si16b/respMatrix_1M_Geom-P5_" + i_particle_name + ".root";
      
	    //    std::string mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5-highestE_" + i_particle_name + ".root";
	    //    std::string mcfilename = "~/data/mc/Si16b/respMatrix_1M_Geom-P5_" + i_particle_name + "-sw.root";
	    std::string mcresponsename = "SiR_two_layer_response";
	    if (i_particle_name == "proton") {
	      mcresponsename = "SiR_three_layer_response";
	    }
	    	      //	    std::string mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5_" + i_particle_name + "_1MeVBins.root";
	    //	    std::string mcfilename = "~/data/mc/Si16b/respMatrix_100M_Geom-P5_" + i_particle_name + "_100keVBins.root";//_notAllEntries.root";
	    std::string mcfilename = "/data/R15bMC/respMatrix_100M_Si16b_" + i_particle_name + "_100keVBins.root";//_notAllEntries.root";
	    // if (i_particle_name == "proton") {
	    // //   //	      mcfilename = "/data/R15bMC/respMatrix_100M_Al50_" + i_particle_name + "_100keVBins_10umSiR3DL.root";
	    // //   mcfilename = "~/g4sim/alcap/respMatrix_100M_Si16b_" + i_particle_name + "_100keVBins_incDLEDep.root";
	    //   mcfilename = "~/g4sim/alcap/respMatrix_100M_Al50_" + i_particle_name + "_100keVBins_expo.root";
	    // }
	      //	    rebin_factor = 10;
	    //	    if (i_particle_name == "alpha") {
	    //	      mcfilename = "~/data/mc/Si16b/respMatrix_12M_Geom-P5_" + i_particle_name + "_1MeVBins.root";
	      //      	rebin_factor = 100;
	    //	    }
	    //	    if (i_particle_name == "triton" || i_particle_name == "deuteron") {
	      //	      mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5_" + i_particle_name + "_1MeVBins.root";
	    //	      mcfilename = "~/data/mc/Si16b/respMatrix_100M_Geom-P5_" + i_particle_name + "_100keVBins.root";
	    //	    }
	    //	    if (i_pid == "TCutG") {
	    //	      rebin_factor *= 100; // because didn't rebin for PID effs
	    //	    }

	    for (int i_bin_width = 0; i_bin_width < n_bin_widths; ++i_bin_width) {
	      std::string bin_width_tag = bin_width_tags[i_bin_width];
	      double bin_width_val = bin_width_vals[i_bin_width];

	      // int rebin_factor = 1;
	      // if (i_particle_name != "proton") {
	      // 	rebin_factor = 1;//2;
	      // }
      
	      outdirname = "ResponseMatrix_" + i_particle_name + "_" + i_pid + "_" + sigma + "sig_layerCoinc" + layer_coinc + "_tGT" + timecut + "_BinW" + bin_width_tag;
	      Si16b_Unfold_ResponseMatrix(outfilename, outfilename, datahistname, mcfilename, mcresponsename, bin_width_val, outdirname, highE_cut);

	      inhistname = outdirname + "/hCorrectedSpectrum";
	      outdirname = "FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "sig_layerCoinc" + layer_coinc + "_tGT" + timecut + "_BinW" + bin_width_tag;
	      Si16b_Unfold_FinalNormalisation(outfilename, inhistname, countfilename, counttreename, outfilename, outdirname);

	    /*
	    //      mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5_" + i_particle_name + "_retune.root";
	    mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5_" + i_particle_name + "_retune_500keVBins.root";
	    rebin_factor = 1;
	    outdirname = "ResponseMatrix_" + i_particle_name + "_" + i_pid + "_retune";
	    Si16b_Unfold_ResponseMatrix(outfilename, outfilename, datahistname, mcfilename, mcresponsename, rebin_factor, outdirname);
	    
	    inhistname = outdirname + "/hCorrectedSpectrum";
	    outdirname = "FinalNormalisation_" + i_particle_name + "_" + i_pid + "_retune";
	    Si16b_Unfold_FinalNormalisation(outfilename, inhistname, countfilename, counttreename, outfilename, outdirname);
	    */
	    }
	  }
	}
      }
    }
  }
}
  /*
  // Active target analysis
  std::string indirname = "ActiveTarget";
  std::string inhistname = indirname + "/hRawSpectrum";
  std::string cutfilename = infilename;
  std::string cuttreename = indirname + "/cuttree";
  std::string corrfilename = infilename;
  //    std::string corrdirname = "SiL3_ActiveTarget_FlatBkg";
  std::string corrdirname = "ActiveTarget_FlatBkg";
  std::string corrhistname = corrdirname + "/hRawSpectrum";
  std::string corrtreename = corrdirname + "/cuttree";
  std::string outdirname = "FlatBkg_Active";
  Si16b_Unfold_FlatBackground(infilename, inhistname, cutfilename, cuttreename, corrfilename, corrhistname, corrtreename, outfilename, outdirname);

  inhistname = outdirname + "/hCorrectedSpectrum";
  std::string incuttreename = indirname + "/cuttree";
  outdirname = "TimeCut_Active";
  double decay_lifetime = 756;
  std::string branch = "time";
  double extra_efficiency = 1.0;
  Si16b_Unfold_TimeCut(outfilename, outfilename, inhistname, infilename, incuttreename, outdirname, decay_lifetime, branch, extra_efficiency);

  countfilename = "~/data/results/Si16b/normalisation_newPP_geq1TgtPulse.root";
  
  inhistname = outdirname + "/hCorrectedSpectrum";
  corrfilename = "~/data/mc/Si16b/decayCorr_100k_Geom-P5.root";
  corrhistname = "hEDep_muplus";
  outdirname = "DecayElectron_Active";
  Si16b_Unfold_DecayElectronCorrection(outfilename, inhistname, corrfilename, corrhistname, countfilename, counttreename, outfilename, outdirname);


  // escape correction
  inhistname = outdirname + "/hCorrectedSpectrum";
  outdirname = "CombinedEscape_Active";
  std::string mcfilename = "~/data/mc/Si16b/respMatrix_100k_Geom-P5_combined_activeTgt.root";
  std::string mcresponsename = "SiL_middle_layer_response";
  int rebin_factor = 4;
  Si16b_Unfold_ResponseMatrix(outfilename, outfilename, inhistname, mcfilename, mcresponsename, rebin_factor, outdirname, "bayes");

  
  inhistname = outdirname + "/hCorrectedSpectrum";
  outdirname = "FinalNormalisation_Active";
  Si16b_Unfold_FinalNormalisation(outfilename, inhistname, countfilename, counttreename, outfilename, outdirname);
  */
  /*
      //      mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5_" + i_particle_name + ".root";
      //      std::string mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5_" + i_particle_name + "_500keVBins.root";
      //      std::string mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5_" + i_particle_name + "_500keVBins_newGaps.root";
      //      std::string mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5_" + i_particle_name + "_1000keVBins_cutObsE.root";
      //      std::string mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5_" + i_particle_name + "_1000keVBins_truth.root";
      //            std::string mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5_" + i_particle_name + "_1000keVBins_higherE.root";
      //      std::string mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5_" + i_particle_name + "_2000keVBins.root";
      //	std::string mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5_" + i_particle_name + "_1000keVBins_withRecoil.root";
      */
        /*
      std::string mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5-higherE_" + i_particle_name + ".root";
      outdirname = "ResponseMatrix_" + i_particle_name + "_" + i_pid + "_higherE";
      Si16b_Unfold_ResponseMatrix(outfilename, outfilename, datahistname, mcfilename, mcresponsename, rebin_factor, outdirname);

      mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5-highestE_" + i_particle_name + ".root";
      outdirname = "ResponseMatrix_" + i_particle_name + "_" + i_pid + "_highestE";
      Si16b_Unfold_ResponseMatrix(outfilename, outfilename, datahistname, mcfilename, mcresponsename, rebin_factor, outdirname);

      mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5-lowerE_" + i_particle_name + ".root";
      outdirname = "ResponseMatrix_" + i_particle_name + "_" + i_pid + "_lowerE";
      Si16b_Unfold_ResponseMatrix(outfilename, outfilename, datahistname, mcfilename, mcresponsename, rebin_factor, outdirname);

      mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5-lowestE_" + i_particle_name + ".root";
      outdirname = "ResponseMatrix_" + i_particle_name + "_" + i_pid + "_lowestE";
      Si16b_Unfold_ResponseMatrix(outfilename, outfilename, datahistname, mcfilename, mcresponsename, rebin_factor, outdirname);

      mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5-1mmCloserSiR_" + i_particle_name + ".root";
      outdirname = "ResponseMatrix_" + i_particle_name + "_" + i_pid + "_1mmCloserSiR";
      Si16b_Unfold_ResponseMatrix(outfilename, outfilename, datahistname, mcfilename, mcresponsename, rebin_factor, outdirname);

      mcfilename = "~/data/mc/Si16b/respMatrix_1M_Geom-P5-detswap_" + i_particle_name + ".root";
      outdirname = "ResponseMatrix_" + i_particle_name + "_" + i_pid + "_detswap";
      Si16b_Unfold_ResponseMatrix(outfilename, outfilename, datahistname, mcfilename, mcresponsename, rebin_factor, outdirname);

      mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5-collXOffset0mm_" + i_particle_name + ".root";
      outdirname = "ResponseMatrix_" + i_particle_name + "_" + i_pid + "_collXOffset0mm";
      Si16b_Unfold_ResponseMatrix(outfilename, outfilename, datahistname, mcfilename, mcresponsename, rebin_factor, outdirname);
      */

  /*  
  // SiL3 Inclusive
  //////////////////
  // Subtract flat background first
  std::string indirname = "SiL3";
  std::string inhistname = indirname + "/hRawSpectrum";
  std::string cutfilename = infilename;
  std::string cuttreename = indirname + "/cuttree";
  std::string corrfilename = infilename;
  std::string corrdirname = indirname + "_FlatBkg";
  std::string corrhistname = corrdirname + "/hRawSpectrum";
  std::string corrtreename = corrdirname + "/cuttree";
  std::string outdirname = "FlatBkg_SiL3";
  Si16b_Unfold_FlatBackground(infilename, inhistname, cutfilename, cuttreename, corrfilename, corrhistname, corrtreename, outfilename, outdirname);

  // Now scale to account for the time cut
  std::string incuttreename = indirname + "/cuttree";
  inhistname = outdirname + "/hCorrectedSpectrum";
  double decay_lifetime = 756;
  outdirname = "TimeCut_SiL3";
  Si16b_Unfold_TimeCut(outfilename, outfilename, inhistname, infilename, incuttreename, outdirname, decay_lifetime, "time");

  // decay electron correction
  double geom_acceptance = 0.0100878;
  */
  //  /*  inhistname = outdirname + "/hCorrectedSpectrum";
  //  corrfilename = "~/data/mc/Si16b/decayCorr_500k_Geom-P5.root";
  //  corrhistname = "hEDep_muplus";
  //  outdirname = "DecayElectron_SiL3";
  //  Si16b_Unfold_DecayElectronCorrection(outfilename, inhistname, corrfilename, corrhistname, countfilename, counttreename, outfilename, outdirname);
  //  */
  /*
  inhistname = outdirname + "/hCorrectedSpectrum";
  outdirname = "GeomAcceptance_SiL3";
  Si16b_Unfold_GeometricAcceptance(outfilename, outfilename, inhistname, outdirname, geom_acceptance);
  */
  
  //  std::string mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5_proton_retune.root";
  //  std::string mcresponsename = "SiL_middle_layer_response";
  //  int rebin_factor = 2;
  //  std::string datahistname = outdirname + "/hCorrectedSpectrum";
  //  outdirname = "ResponseMatrix_SiL3_retune";
  //  Si16b_Unfold_ResponseMatrix(outfilename, outfilename, datahistname, mcfilename, mcresponsename, rebin_factor, outdirname, "bayes");
  
  
  //  inhistname = outdirname + "/hCorrectedSpectrum";
  //  outdirname = "FinalNormalisation_SiL3_retune";
  //  Si16b_Unfold_FinalNormalisation(outfilename, inhistname, countfilename, counttreename, outfilename, outdirname);

  /*
  // SiR12 Inclusive
  //////////////////
  // Subtract flat background first
  indirname = "all_TCutG_noTimeCut";
  inhistname = indirname + "/hRawSpectrum";
  cutfilename = infilename;
  cuttreename = indirname + "/cuttree";
  corrfilename = infilename;
  outdirname = "TimeCut_SiR12";
  Si16b_Unfold_TimeCut(infilename, outfilename, inhistname, infilename, cuttreename, outdirname, decay_lifetime);

  geom_acceptance = 0.013265;
  inhistname = outdirname + "/hCorrectedSpectrum";
  outdirname = "GeomAcceptance_SiR12";
  Si16b_Unfold_GeometricAcceptance(outfilename, outfilename, inhistname, outdirname, geom_acceptance);
  */
  //  mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5_proton_retune.root";
  //  mcresponsename = "SiR_two_layer_response";
  //  rebin_factor = 10;
  //  datahistname = outdirname + "/hCorrectedSpectrum";
  //  outdirname = "ResponseMatrix_SiR12_retune";
  //  Si16b_Unfold_ResponseMatrix(outfilename, outfilename, datahistname, mcfilename, mcresponsename, rebin_factor, outdirname, "bayes");

  //  inhistname = outdirname + "/hCorrectedSpectrum";
  //  outdirname = "FinalNormalisation_SiR12_retune";
  //  Si16b_Unfold_FinalNormalisation(outfilename, inhistname, countfilename, counttreename, outfilename, outdirname);

  //  TFile* outfile2 = new TFile(outfilename.c_str(), "UPDATE");
  //  TDirectory* outdir = outfile2->mkdir("AllParticles");
  //  outdir->cd();
  //  hSum->Write();
  //  outfile2->Write();
  //  outfile2->Close();


  /*
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
  */
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
  //  }
  //}
