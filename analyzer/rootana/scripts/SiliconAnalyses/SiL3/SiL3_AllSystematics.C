#include "systematics/SiL3_Systematic_ParticleComposition.C"
#include "systematics/SiL3_Systematic_UnfoldingRegParameter.C"
#include "systematics/SiL3_Systematic_MuonStopPositions.C"
#include "systematics/SiL3_Systematic_TimeSlice.C"

#include "../Utils/Systematic_FinalPlot.C"

void SiL3_AllSystematics() {

  gROOT->ProcessLine(".L ~/libRooUnfold.so");

  std::string infilename = "~/data/results/SiL3/unfold_geq2TgtPulse_newPP20us.root";
  std::string outfilename = "~/data/results/SiL3/systematics_geq2TgtPulse_newPP20us.root";
  std::string countfilename = "~/data/results/SiL3/normalisation_geq0TgtPulse_newPP20us.root";
  std::string counttreename = "XRaySpectrum_GeHiGain_2p1s_5000nsTimeCut/counttree";
      
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  const int n_bin_widths = 3;
  std::string bin_widths[n_bin_widths] = {"100keVBins", "200keVBins", "500keVBins"};
  
  const int n_recoils = 2;
  std::string recoils[n_recoils] = {"noRecoil", "allRecoil"};//"nuRecoil"};
  
  // const int n_time_slices = 5;
  // double min_time_slices[n_time_slices] = {2000, 2500, 3000, 3500, 2000};
  // double max_time_slices[n_time_slices] = {2500, 3000, 3500, 4000, 4000};
  const int n_time_slices = 1;
  double min_time_slices[n_time_slices] = {2000};
  double max_time_slices[n_time_slices] = {4000};

  std::stringstream time_slice_str;

  for (int i_recoil = 0; i_recoil < n_recoils; ++i_recoil) {
    std::string recoil = recoils[i_recoil];
    
    for (int i_time_slice = 0; i_time_slice < n_time_slices; ++i_time_slice) {
      double i_min_time_slice = min_time_slices[i_time_slice];
      double i_max_time_slice = max_time_slices[i_time_slice];
      
      time_slice_str.str("");
      time_slice_str << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;

      SiL3_Systematic_UnfoldingRegParameter(infilename, outfilename, countfilename, counttreename, time_slice_str.str(), recoil);
      SiL3_Systematic_MuonStopPositions(infilename, outfilename, countfilename, counttreename, time_slice_str.str(), recoil);
      SiL3_Systematic_ParticleComposition(infilename, outfilename, countfilename, counttreename, time_slice_str.str(), recoil);
      SiL3_Systematic_TimeSlice(infilename, outfilename, countfilename, counttreename, time_slice_str.str(), recoil);
      
      for (int i_bin_width = 0; i_bin_width < n_bin_widths; ++i_bin_width) {
	std::string bin_width = bin_widths[i_bin_width];
					   
	// Compile all the systematics we want into one plot
	Systematic_FinalPlotArgs args;
	args.central_val_filename = infilename;
	args.central_val_histname = "FinalNormalisation_" + time_slice_str.str() + "_" + recoil + "_" + bin_width + "/hNormalisedSpectrum";
	args.syst_val_filename = outfilename;
	
	args.syst_val_histnames.push_back("FinalNormalisation_" + time_slice_str.str() + "_" + recoil + "_KReg200_" + bin_width + "_SystPlot");
	args.symmetric.push_back(false);
	args.syst_val_histnames.push_back("FinalNormalisation_" + time_slice_str.str() + "_" + recoil + "_KReg300_" + bin_width + "_SystPlot");
	args.symmetric.push_back(false);
					   
	args.syst_val_histnames.push_back("FinalNormalisation_" + time_slice_str.str() + "_lowAllRecoil_" + recoil + "Syst_" + bin_width + "_SystPlot");
	args.symmetric.push_back(false);
	args.syst_val_histnames.push_back("FinalNormalisation_" + time_slice_str.str() + "_highAllRecoil_" + recoil +  "Syst_" + bin_width + "_SystPlot");
	args.symmetric.push_back(false);
	// could add noRecoil here... 
	
	args.syst_val_histnames.push_back("FinalNormalisation_" + time_slice_str.str() + "_" + recoil + "_MuStopPosHigherE_" + bin_width + "_SystPlot");
	args.symmetric.push_back(false);
	args.syst_val_histnames.push_back("FinalNormalisation_" + time_slice_str.str() + "_" + recoil + "_MuStopPosLowerE_" + bin_width + "_SystPlot");
	args.symmetric.push_back(false);

	args.syst_val_histnames.push_back("FinalNormalisation_TimeSlice2000_2500_" + time_slice_str.str() + "Syst_" + recoil + "_" + bin_width + "_SystPlot");
	args.symmetric.push_back(false);
	args.syst_val_histnames.push_back("FinalNormalisation_TimeSlice2500_3000_" + time_slice_str.str() + "Syst_" + recoil + "_" + bin_width + "_SystPlot");
	args.symmetric.push_back(false);
	args.syst_val_histnames.push_back("FinalNormalisation_TimeSlice3000_3500_" + time_slice_str.str() + "Syst_" + recoil + "_" + bin_width + "_SystPlot");
	args.symmetric.push_back(false);
	args.syst_val_histnames.push_back("FinalNormalisation_TimeSlice3500_4000_" + time_slice_str.str() + "Syst_" + recoil + "_" + bin_width + "_SystPlot");
	args.symmetric.push_back(false);
	
	args.outfilename = outfilename;
	args.outdirname = "FinalSystPlot_" + time_slice_str.str() + "_" + recoil + "_" + bin_width;
	Systematic_FinalPlot(args);


	// Make individual systematics plots
	Systematic_FinalPlotArgs single_args;
	single_args.central_val_filename = infilename;
	single_args.central_val_histname = "FinalNormalisation_" + time_slice_str.str() + "_" + recoil + "_" + bin_width + "/hNormalisedSpectrum";
	single_args.syst_val_filename = outfilename;
	single_args.outfilename = outfilename;

	single_args.syst_val_histnames.clear();
	single_args.symmetric.clear();
	single_args.syst_val_histnames.push_back("FinalNormalisation_" + time_slice_str.str() + "_" + recoil + "_KReg200_" + bin_width + "_SystPlot");
	single_args.symmetric.push_back(false);
	single_args.syst_val_histnames.push_back("FinalNormalisation_" + time_slice_str.str() + "_" + recoil + "_KReg300_" + bin_width + "_SystPlot");
	single_args.symmetric.push_back(false);
	single_args.outdirname = "FinalSystPlot_KReg_" + time_slice_str.str() + "_" + recoil + "_" + bin_width;
	Systematic_FinalPlot(single_args);

	single_args.syst_val_histnames.clear();
	single_args.symmetric.clear();
	single_args.syst_val_histnames.push_back("FinalNormalisation_" + time_slice_str.str() + "_lowAllRecoil_" + recoil + "Syst_" + bin_width + "_SystPlot");
	single_args.symmetric.push_back(false);
	single_args.syst_val_histnames.push_back("FinalNormalisation_" + time_slice_str.str() + "_highAllRecoil_" + recoil +  "Syst_" + bin_width + "_SystPlot");
	single_args.symmetric.push_back(false);
	single_args.outdirname = "FinalSystPlot_ParticleComposition_" + time_slice_str.str() + "_" + recoil + "_" + bin_width;
	Systematic_FinalPlot(single_args);

	single_args.syst_val_histnames.clear();
	single_args.symmetric.clear();
	single_args.syst_val_histnames.push_back("FinalNormalisation_" + time_slice_str.str() + "_" + recoil + "_MuStopPosHigherE_" + bin_width + "_SystPlot");
	single_args.symmetric.push_back(false);
	single_args.syst_val_histnames.push_back("FinalNormalisation_" + time_slice_str.str() + "_" + recoil + "_MuStopPosLowerE_" + bin_width + "_SystPlot");
	single_args.symmetric.push_back(false);
	single_args.outdirname = "FinalSystPlot_MuStopPos_" + time_slice_str.str() + "_" + recoil + "_" + bin_width;
	Systematic_FinalPlot(single_args);

	single_args.syst_val_histnames.clear();
	single_args.symmetric.clear();
	single_args.syst_val_histnames.push_back("FinalNormalisation_TimeSlice2000_2500_" + time_slice_str.str() + "Syst_" + recoil + "_" + bin_width + "_SystPlot");
	single_args.symmetric.push_back(false);
	single_args.syst_val_histnames.push_back("FinalNormalisation_TimeSlice2500_3000_" + time_slice_str.str() + "Syst_" + recoil + "_" + bin_width + "_SystPlot");
	single_args.symmetric.push_back(false);
	single_args.syst_val_histnames.push_back("FinalNormalisation_TimeSlice3000_3500_" + time_slice_str.str() + "Syst_" + recoil + "_" + bin_width + "_SystPlot");
	single_args.symmetric.push_back(false);
	single_args.syst_val_histnames.push_back("FinalNormalisation_TimeSlice3500_4000_" + time_slice_str.str() + "Syst_" + recoil + "_" + bin_width + "_SystPlot");
	single_args.symmetric.push_back(false);
	single_args.outdirname = "FinalSystPlot_TimeSlice_" + time_slice_str.str() + "_" + recoil + "_" + bin_width;
	Systematic_FinalPlot(single_args);
      }
    }
  }  
}
