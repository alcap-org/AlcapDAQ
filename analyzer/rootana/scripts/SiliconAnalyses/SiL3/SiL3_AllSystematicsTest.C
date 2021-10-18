#include "systematics/SiL3_Systematic_ParticleComposition.C"
//#include "systematics/SiL3_Systematic_UnfoldingRegParameter.C"
#include "systematics/SiL3_Systematic_MuonStopPositions.C"
#include "systematics/SiL3_Systematic_TimeSlice.C"

#include "../Utils/Systematic_FinalPlot.C"

void SiL3_AllSystematics() {

  gROOT->ProcessLine(".L ~/libRooUnfold.so");

  std::string infilename = "~/data/results/SiL3/unfold_geq2TgtPulse_newPP20us_1_test_KFactor0-85.root";
  std::string outfilename = "~/data/results/SiL3/systematics_geq2TgtPulse_newPP20us_1_KFactor0-85_test.root";
  std::string countfilename = "~/data/results/SiL3/normalisation_geq0TgtPulse_newPP20us_1.root";
  std::string counttreename = "XRaySpectrum_GeHiGain_2p1s_5000nsTimeCut/counttree";
      
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  const int n_bin_widths = 1;//3;
  std::string bin_widths[n_bin_widths] = {"500keVBins"};//{"100keVBins", "200keVBins", "500keVBins"};
  
  const int n_recoils = 1;//3;
  std::string recoils[n_recoils] = {"allRecoil"};//{"noRecoil", "allRecoil", "nuRecoil"};
  
  // const int n_ana_time_slices = 5;
  // double min_ana_time_slices[n_ana_time_slices] = {2000, 2500, 3000, 3500, 2000};
  // double max_ana_time_slices[n_ana_time_slices] = {2500, 3000, 3500, 4000, 4000};
  const int n_ana_time_slices = 1;
  double min_ana_time_slices[n_ana_time_slices] = {3000};
  double max_ana_time_slices[n_ana_time_slices] = {4000};

  //  const int n_syst_time_slices = 3;
  //  double min_syst_time_slices[n_syst_time_slices] = {2000, 2500, 3000};
  //  double max_syst_time_slices[n_syst_time_slices] = {4000, 4000, 4000};
  const int n_syst_time_slices = 2;
  double min_syst_time_slices[n_syst_time_slices] = {3000, 3500};
  double max_syst_time_slices[n_syst_time_slices] = {3500, 4000};

  std::stringstream ana_time_slice_str;
  std::stringstream syst_time_slice_str;

  for (int i_recoil = 0; i_recoil < n_recoils; ++i_recoil) {
    std::string recoil = recoils[i_recoil];
    
    for (int i_ana_time_slice = 0; i_ana_time_slice < n_ana_time_slices; ++i_ana_time_slice) {
      double i_min_ana_time_slice = min_ana_time_slices[i_ana_time_slice];
      double i_max_ana_time_slice = max_ana_time_slices[i_ana_time_slice];
      
      ana_time_slice_str.str("");
      ana_time_slice_str << "TimeSlice" << i_min_ana_time_slice << "_" << i_max_ana_time_slice;

      //      SiL3_Systematic_UnfoldingRegParameter(infilename, outfilename, countfilename, counttreename, ana_time_slice_str.str(), recoil);
      SiL3_Systematic_MuonStopPositions(infilename, outfilename, countfilename, counttreename, ana_time_slice_str.str(), recoil);
      SiL3_Systematic_ParticleComposition(infilename, outfilename, countfilename, counttreename, ana_time_slice_str.str(), recoil);
      SiL3_Systematic_TimeSlice(infilename, outfilename, countfilename, counttreename, ana_time_slice_str.str(), recoil);
      
      for (int i_bin_width = 0; i_bin_width < n_bin_widths; ++i_bin_width) {
	std::string bin_width = bin_widths[i_bin_width];
					   
	// Compile all the systematics we want into one plot
	Systematic_FinalPlotArgs args;
	args.central_val_filename = infilename;
	args.central_val_histname = "FinalNormalisation_" + ana_time_slice_str.str() + "_" + recoil + "_" + bin_width + "/hNormalisedSpectrum";
	//	args.central_val_histname = "DecayElectron_" + ana_time_slice_str.str() + "_" + recoil + "/hCorrectedSpectrum";
	args.syst_val_filename = outfilename;
	
	// args.syst_val_histnames.push_back("FinalNormalisation_" + ana_time_slice_str.str() + "_" + recoil + "_KReg200_" + bin_width + "_SystPlot");
	// args.symmetric.push_back(true);
	// if (i_min_ana_time_slice < 2750) {
	//   args.syst_val_histnames.push_back("FinalNormalisation_" + ana_time_slice_str.str() + "_" + recoil + "_KReg300_" + bin_width + "_SystPlot");
	//   args.symmetric.push_back(true);
	// }
					   
	args.syst_val_histnames.push_back("FinalNormalisation_" + ana_time_slice_str.str() + "_lowAllRecoil_" + recoil + "Syst_" + bin_width + "_SystPlot");
	args.symmetric.push_back(true);
	args.syst_val_histnames.push_back("FinalNormalisation_" + ana_time_slice_str.str() + "_highAllRecoil_" + recoil +  "Syst_" + bin_width + "_SystPlot");
	args.symmetric.push_back(true);
	args.syst_val_histnames.push_back("FinalNormalisation_" + ana_time_slice_str.str() + "_noRecoil_" + recoil +  "Syst_" + bin_width + "_SystPlot");
	args.symmetric.push_back(true);
	// could add noRecoil here... 
	
	// args.syst_val_histnames.push_back("FinalNormalisation_" + ana_time_slice_str.str() + "_" + recoil + "_MuStopPoshigherE_" + bin_width + "_SystPlot");
	// args.symmetric.push_back(true);
	// args.syst_val_histnames.push_back("FinalNormalisation_" + ana_time_slice_str.str() + "_" + recoil + "_MuStopPoslowerE_" + bin_width + "_SystPlot");
	// args.symmetric.push_back(true);

	// for (int i_syst_time_slice = 0; i_syst_time_slice < n_syst_time_slices; ++i_syst_time_slice) {
	//   double i_min_syst_time_slice = min_syst_time_slices[i_syst_time_slice];
	//   double i_max_syst_time_slice = max_syst_time_slices[i_syst_time_slice];
      
	//   syst_time_slice_str.str("");
	//   syst_time_slice_str << "TimeSlice" << i_min_syst_time_slice << "_" << i_max_syst_time_slice;

	//   //	  if (i_min_ana_time_slice < i_min_syst_time_slice) {
	//   args.syst_val_histnames.push_back("FinalNormalisation_" + syst_time_slice_str.str() + "_" + ana_time_slice_str.str() + "Syst_" + recoil + "_" + bin_width + "_SystPlot");
	//   args.symmetric.push_back(true);
	//     //	  }
	// }
	
	args.outfilename = outfilename;
	args.outdirname = "FinalSystPlot_" + ana_time_slice_str.str() + "_" + recoil + "_" + bin_width;
	Systematic_FinalPlot(args);


	// Make individual systematics plots
	Systematic_FinalPlotArgs single_args;
	single_args.central_val_filename = infilename;
	single_args.central_val_histname = "FinalNormalisation_" + ana_time_slice_str.str() + "_" + recoil + "_" + bin_width + "/hNormalisedSpectrum";
	//	single_args.central_val_histname = "DecayElectron_" + ana_time_slice_str.str() + "_" + recoil + "/hCorrectedSpectrum";
	single_args.syst_val_filename = outfilename;
	single_args.outfilename = outfilename;

	// single_args.syst_val_histnames.clear();
	// single_args.symmetric.clear();
	// single_args.syst_val_histnames.push_back("FinalNormalisation_" + ana_time_slice_str.str() + "_" + recoil + "_KReg200_" + bin_width + "_SystPlot");
	// single_args.symmetric.push_back(true);
	// if (i_min_ana_time_slice < 2750) {
	//   single_args.syst_val_histnames.push_back("FinalNormalisation_" + ana_time_slice_str.str() + "_" + recoil + "_KReg300_" + bin_width + "_SystPlot");
	//   single_args.symmetric.push_back(true);
	// }
	// single_args.outdirname = "FinalSystPlot_KReg_" + ana_time_slice_str.str() + "_" + recoil + "_" + bin_width;
	// Systematic_FinalPlot(single_args);

	single_args.syst_val_histnames.clear();
	single_args.symmetric.clear();
	single_args.syst_val_histnames.push_back("FinalNormalisation_" + ana_time_slice_str.str() + "_lowAllRecoil_" + recoil + "Syst_" + bin_width + "_SystPlot");
	single_args.symmetric.push_back(true);
	single_args.syst_val_histnames.push_back("FinalNormalisation_" + ana_time_slice_str.str() + "_highAllRecoil_" + recoil +  "Syst_" + bin_width + "_SystPlot");
	single_args.symmetric.push_back(true);
	single_args.syst_val_histnames.push_back("FinalNormalisation_" + ana_time_slice_str.str() + "_noRecoil_" + recoil +  "Syst_" + bin_width + "_SystPlot");
	single_args.symmetric.push_back(true);
	single_args.outdirname = "FinalSystPlot_ParticleComposition_" + ana_time_slice_str.str() + "_" + recoil + "_" + bin_width;
	Systematic_FinalPlot(single_args);

	single_args.syst_val_histnames.clear();
	single_args.symmetric.clear();
	single_args.syst_val_histnames.push_back("FinalNormalisation_" + ana_time_slice_str.str() + "_" + recoil + "_MuStopPoshigherE_" + bin_width + "_SystPlot");
	single_args.symmetric.push_back(true);
	single_args.syst_val_histnames.push_back("FinalNormalisation_" + ana_time_slice_str.str() + "_" + recoil + "_MuStopPoslowerE_" + bin_width + "_SystPlot");
	single_args.symmetric.push_back(true);
	single_args.outdirname = "FinalSystPlot_MuStopPos_" + ana_time_slice_str.str() + "_" + recoil + "_" + bin_width;
	//	Systematic_FinalPlot(single_args);

	single_args.syst_val_histnames.clear();
	single_args.symmetric.clear();
	for (int i_syst_time_slice = 0; i_syst_time_slice < n_syst_time_slices; ++i_syst_time_slice) {
	  double i_min_syst_time_slice = min_syst_time_slices[i_syst_time_slice];
	  double i_max_syst_time_slice = max_syst_time_slices[i_syst_time_slice];
      
	  syst_time_slice_str.str("");
	  syst_time_slice_str << "TimeSlice" << i_min_syst_time_slice << "_" << i_max_syst_time_slice;

	  //	  if (i_min_ana_time_slice < i_min_syst_time_slice) {
	    single_args.syst_val_histnames.push_back("FinalNormalisation_" + syst_time_slice_str.str() + "_" + ana_time_slice_str.str() + "Syst_" + recoil + "_" + bin_width + "_SystPlot");
	    single_args.symmetric.push_back(true);
	    //	  }
	}
	single_args.outdirname = "FinalSystPlot_TimeSlice_" + ana_time_slice_str.str() + "_" + recoil + "_" + bin_width;
	//	Systematic_FinalPlot(single_args);
      }
    }
  }  
}
