#include "systematics/Si16b_Systematic_LayerCoincCut.C"
#include "systematics/Si16b_Systematic_UnfoldingIterations.C"
#include "systematics/Si16b_Systematic_UnfoldingBinWidths.C"
#include "systematics/Si16b_Systematic_MuonStopPositions.C"
#include "systematics/Si16b_Systematic_Miscalibration.C"
#include "systematics/Si16b_Systematic_SiRPosition.C"
//#include "systematics/Si16b_Systematic_PIDCut.C"
#include "../Utils/Systematic_FinalPlot.C"
#include "../Utils/Systematic_SumPlot.C"

void Si16b_AllSystematics() {

  gROOT->ProcessLine(".L ~/libRooUnfold.so");

  std::string infilename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_2.root";
  std::string outfilename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_2.root";
  std::string countfilename = "~/data/results/Si16b/normalisation_newPP_geq1TgtPulse_1.root";
  std::string counttreename = "XRaySpectrum_GeHiGain_2p1s_5000nsTimeCut/counttree";
      
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  // Have two different PID methods
  const int n_PIDs = 1;
  std::string PIDs[n_PIDs] = {"TCutG"};//, "PSel"};
  //  double pid_efficiencies[n_PIDs] = {0.9779, 0.986};

  // For protons, deuterons etc
  const int n_particles = 4;
  std::string particle_names[n_particles] = {"proton", "deuteron", "triton", "alpha"};//, "proton_combined"};

  std::string sigma = "2sig";
  std::string layer_coinc = "layerCoinc500ns";
  std::string timecut = "tGT0ns";
  std::string bin_width = "BinW500keV";
  double bin_width_val = 0.5;
  
  for (int i_PID = 0; i_PID < n_PIDs; ++i_PID) {
    std::string i_pid = PIDs[i_PID];
    //    double i_pid_efficiency = pid_efficiencies[i_PID];
    
    for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
      std::string i_particle_name = particle_names[i_particle];

      //      Si16b_Systematic_LayerCoincCut(infilename, outfilename, i_pid, i_particle_name, sigma, layer_coinc, timecut, bin_width);

      Si16b_Systematic_MuonStopPositions(infilename, outfilename, countfilename, counttreename, i_pid, i_particle_name, sigma, layer_coinc, timecut, bin_width, bin_width_val);
      Si16b_Systematic_Miscalibration(infilename, outfilename, countfilename, counttreename, i_pid, i_particle_name, sigma, layer_coinc, timecut, bin_width, bin_width_val);

      Si16b_Systematic_UnfoldingIterations(infilename, outfilename, countfilename, counttreename, i_pid, i_particle_name, sigma, layer_coinc, timecut, bin_width, bin_width_val);

      //      Si16b_Systematic_SiRPosition(infilename, outfilename, countfilename, counttreename, i_pid, i_particle_name, sigma, layer_coinc, timecut, bin_width);

      Si16b_Systematic_UnfoldingBinWidths(infilename, outfilename, countfilename, counttreename, i_pid, i_particle_name, sigma, layer_coinc, timecut, bin_width);
      
      //      Si16b_Systematic_PIDCut(infilename, outfilename, i_pid, i_particle_name, sigma, layer_coinc, timecut);
      

      // Compile all the systematics we want into one plot
      Systematic_FinalPlotArgs args;
      args.central_val_filename = infilename;
      args.central_val_histname = "FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "/hNormalisedSpectrum";
      args.syst_val_filename = outfilename;

      //      args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" +timecut + "_" + bin_width + "_LayerCoinc200ns_SystPlot");
      //      args.symmetric.push_back(true);

      args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_MuStopPos_midHighE_SystPlot");
      args.symmetric.push_back(false);
      args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_MuStopPos_midLowE_SystPlot");
      args.symmetric.push_back(false);

      args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_MisCalibDown_SystPlot");
      args.symmetric.push_back(false);
      args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_MisCalibUp_SystPlot");
      args.symmetric.push_back(false);

      args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_NIter20_SystPlot");
      args.symmetric.push_back(false);

      //      args.syst_val_histnames.push_back("SiRPosition_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_SystPlot");
      //      args.symmetric.push_back(false);

      //      args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_PID1sigma_SystPlot");
      //      args.symmetric.push_back(false);
      //      //      args.syst_val_histnames.push_back("PID_" + i_particle_name + "_" + i_pid + "_1sigma_SystPlot");
      //      //      args.symmetric.push_back(false);
      //      args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_SiRPos_closerSiR_SystPlot");
      //      args.symmetric.push_back(false);
      //      args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_SiRPos_furtherSiR_SystPlot");
      args.symmetric.push_back(false);
      args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_BinW100keV_SystPlot");
      args.symmetric.push_back(false);

      args.outfilename = outfilename;
      args.outdirname = "FinalSystPlot_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width;
      Systematic_FinalPlot(args);

      // Make individual systematics plots
      Systematic_FinalPlotArgs single_args;
      single_args.central_val_filename = infilename;
      single_args.central_val_histname = "FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "/hNormalisedSpectrum";
      single_args.syst_val_filename = outfilename;
      single_args.outfilename = outfilename;
      
      // single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_LayerCoinc200ns_SystPlot");
      // single_args.symmetric.push_back(false);
      // single_args.outdirname = "FinalSystPlot_LayerCoincCut_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width;
      // Systematic_FinalPlot(single_args);

      single_args.syst_val_histnames.clear();
      single_args.symmetric.clear();
      single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_MuStopPos_midHighE_SystPlot");
      single_args.symmetric.push_back(false);
      single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_MuStopPos_midLowE_SystPlot");
      single_args.symmetric.push_back(false);
      single_args.outdirname = "FinalSystPlot_MuStopPos_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width;
      Systematic_FinalPlot(single_args);


      single_args.syst_val_histnames.clear();
      single_args.symmetric.clear();
      single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_MuStopPos_midHighE_SystPlot");
      single_args.symmetric.push_back(false);
      single_args.outdirname = "FinalSystPlot_MuStopPosHigh_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width;
      Systematic_FinalPlot(single_args);

      single_args.syst_val_histnames.clear();
      single_args.symmetric.clear();
      single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_MuStopPos_midLowE_SystPlot");
      single_args.symmetric.push_back(false);
      single_args.outdirname = "FinalSystPlot_MuStopPosLow_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width;
      Systematic_FinalPlot(single_args);


      single_args.syst_val_histnames.clear();
      single_args.symmetric.clear();
      single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_MisCalibDown_SystPlot");
      single_args.symmetric.push_back(false);
      single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_MisCalibUp_SystPlot");
      single_args.symmetric.push_back(false);
      single_args.outdirname = "FinalSystPlot_MisCalib_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width;
      Systematic_FinalPlot(single_args);

      single_args.syst_val_histnames.clear();
      single_args.symmetric.clear();
      single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_MisCalibDown_SystPlot");
      single_args.symmetric.push_back(false);
      single_args.outdirname = "FinalSystPlot_MisCalibDown_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width;
      Systematic_FinalPlot(single_args);
      
      single_args.syst_val_histnames.clear();
      single_args.symmetric.clear();
      single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_MisCalibUp_SystPlot");
      single_args.symmetric.push_back(false);
      single_args.outdirname = "FinalSystPlot_MisCalibUp_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width;
      Systematic_FinalPlot(single_args);

      single_args.syst_val_histnames.clear();
      single_args.symmetric.clear();
      single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_NIter20_SystPlot");
      single_args.symmetric.push_back(false);
      single_args.outdirname = "FinalSystPlot_NIter_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width;
      Systematic_FinalPlot(single_args);

      // single_args.syst_val_histnames.clear();
      // single_args.symmetric.clear();
      // single_args.syst_val_histnames.push_back("SiRPosition_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_SystPlot");
      // single_args.symmetric.push_back(false);
      // single_args.outdirname = "FinalSystPlot_SiRPos_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width;
      // Systematic_FinalPlot(single_args);
      
      // single_args.syst_val_histnames.clear();
      // single_args.symmetric.clear();
      // single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_PID1sigma_SystPlot");
      // single_args.symmetric.push_back(false);
      // single_args.outdirname = "FinalSystPlot_PID_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut;
      // Systematic_FinalPlot(single_args);

      // single_args.syst_val_histnames.clear();
      // single_args.symmetric.clear();
      // single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_SiRPos_closerSiR_SystPlot");
      // single_args.symmetric.push_back(false);
      // single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_SiRPos_furtherSiR_SystPlot");
      // single_args.symmetric.push_back(false);
      // single_args.outdirname = "FinalSystPlot_SiRPos_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut;
      // Systematic_FinalPlot(single_args);

      single_args.syst_val_histnames.clear();
      single_args.symmetric.clear();
      single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_BinW100keV_SystPlot");
      single_args.symmetric.push_back(false);
      single_args.outdirname = "FinalSystPlot_BinW_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width;
      Systematic_FinalPlot(single_args);
    }
    
    Systematic_SumPlotArgs sum_args;
    sum_args.gre_filename = outfilename;
    for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
      std::string i_particle_name = particle_names[i_particle];
      sum_args.gre_names.push_back("FinalSystPlot_" + i_particle_name + "_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width);
    }    
    sum_args.outfilename = outfilename;
    sum_args.outdirname = "FinalSystPlot_Sum_" + i_pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width;
    Systematic_SumPlot(sum_args);
  }
}
