#include "systematics/Si16b_Systematic_LayerCoincCut.C"
#include "systematics/Si16b_Systematic_UnfoldingIterations.C"
#include "systematics/Si16b_Systematic_UnfoldingBinWidths.C"
#include "systematics/Si16b_Systematic_MuonStopPositions.C"
#include "systematics/Si16b_Systematic_SiRPosition.C"
#include "systematics/Si16b_Systematic_PIDCut.C"
#include "../Utils/Systematic_FinalPlot.C"
#include "../Utils/Systematic_SumPlot.C"

void Si16b_AllSystematics() {

  gROOT->ProcessLine(".L ~/libRooUnfold.so");

  std::string infilename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_3sigma.root";
  std::string outfilename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_3sigma.root";
  std::string countfilename = "~/data/results/Si16b/normalisation_newPP_geq1TgtPulse.root";
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

  for (int i_PID = 0; i_PID < n_PIDs; ++i_PID) {
    std::string i_pid = PIDs[i_PID];
    //    double i_pid_efficiency = pid_efficiencies[i_PID];

    //    Si16b_Systematic_LayerCoincCut(infilename, outfilename, i_pid, particle_names[0]);
    
    for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
      std::string i_particle_name = particle_names[i_particle];

      Si16b_Systematic_MuonStopPositions(infilename, outfilename, countfilename, counttreename, i_pid, i_particle_name);

      Si16b_Systematic_SiRPosition(infilename, outfilename, countfilename, counttreename, i_pid, i_particle_name);

      Si16b_Systematic_UnfoldingIterations(infilename, outfilename, countfilename, counttreename, i_pid, i_particle_name);

      Si16b_Systematic_UnfoldingBinWidths(infilename, outfilename, countfilename, counttreename, i_pid, i_particle_name);
      
      Si16b_Systematic_PIDCut(infilename, outfilename, i_pid, i_particle_name);
      

      // Compile all the systematics we want into one plot
      Systematic_FinalPlotArgs args;
      args.central_val_filename = infilename;
      args.central_val_histname = "FinalNormalisation_" + i_particle_name + "_" + i_pid + "/hNormalisedSpectrum";
      args.syst_val_filename = outfilename;

      // args.syst_val_histnames.push_back("LayerCoincCut_" + i_pid + "_SystPlot");
      // args.symmetric.push_back(false);
      args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_NIter100_SystPlot");
      args.symmetric.push_back(false);
      args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_PID2sigma_SystPlot");
      args.symmetric.push_back(false);
      //      args.syst_val_histnames.push_back("PID_" + i_particle_name + "_" + i_pid + "_1sigma_SystPlot");
      //      args.symmetric.push_back(false);
      args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_MuStopPos_higherE_SystPlot");
      args.symmetric.push_back(false);
      args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_MuStopPos_lowerE_SystPlot");
      args.symmetric.push_back(false);
      args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_SiRPos_closerSiR_SystPlot");
      args.symmetric.push_back(false);
      args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_SiRPos_furtherSiR_SystPlot");
      args.symmetric.push_back(false);
      args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_BinW500_SystPlot");
      args.symmetric.push_back(false);

      args.outfilename = outfilename;
      args.outdirname = "FinalSystPlot_" + i_particle_name + "_" + i_pid;
      Systematic_FinalPlot(args);

      // Make individual systematics plots
      Systematic_FinalPlotArgs single_args;
      single_args.central_val_filename = infilename;
      single_args.central_val_histname = "FinalNormalisation_" + i_particle_name + "_" + i_pid + "/hNormalisedSpectrum";
      single_args.syst_val_filename = outfilename;
      single_args.outfilename = outfilename;
      
      // single_args.syst_val_histnames.push_back("LayerCoincCut_" + i_pid + "_SystPlot");
      // single_args.symmetric.push_back(false);
      // single_args.outdirname = "FinalSystPlot_LayerCoincCut_" + i_particle_name + "_" + i_pid;
      // Systematic_FinalPlot(single_args);

      single_args.syst_val_histnames.clear();
      single_args.symmetric.clear();
      single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_NIter100_SystPlot");
      single_args.symmetric.push_back(false);
      single_args.outdirname = "FinalSystPlot_NIter_" + i_particle_name + "_" + i_pid;
      Systematic_FinalPlot(single_args);

      single_args.syst_val_histnames.clear();
      single_args.symmetric.clear();
      single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_PID2sigma_SystPlot");
      single_args.symmetric.push_back(false);
      single_args.outdirname = "FinalSystPlot_PID_" + i_particle_name + "_" + i_pid;
      Systematic_FinalPlot(single_args);

      single_args.syst_val_histnames.clear();
      single_args.symmetric.clear();
      single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_MuStopPos_higherE_SystPlot");
      single_args.symmetric.push_back(false);
      single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_MuStopPos_lowerE_SystPlot");
      single_args.symmetric.push_back(false);
      single_args.outdirname = "FinalSystPlot_MuStopPos_" + i_particle_name + "_" + i_pid;
      Systematic_FinalPlot(single_args);

      single_args.syst_val_histnames.clear();
      single_args.symmetric.clear();
      single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_SiRPos_closerSiR_SystPlot");
      single_args.symmetric.push_back(false);
      single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_SiRPos_furtherSiR_SystPlot");
      single_args.symmetric.push_back(false);
      single_args.outdirname = "FinalSystPlot_SiRPos_" + i_particle_name + "_" + i_pid;
      Systematic_FinalPlot(single_args);

      single_args.syst_val_histnames.clear();
      single_args.symmetric.clear();
      single_args.syst_val_histnames.push_back("FinalNormalisation_" + i_particle_name + "_" + i_pid + "_BinW500_SystPlot");
      single_args.symmetric.push_back(false);
      single_args.outdirname = "FinalSystPlot_BinW_" + i_particle_name + "_" + i_pid;
      Systematic_FinalPlot(single_args);
    }
    
    Systematic_SumPlotArgs sum_args;
    sum_args.gre_filename = outfilename;
    for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
      std::string i_particle_name = particle_names[i_particle];
      sum_args.gre_names.push_back("FinalSystPlot_" + i_particle_name + "_" + i_pid);
    }    
    sum_args.outfilename = outfilename;
    sum_args.outdirname = "FinalSystPlot_Sum_" + i_pid;
    Systematic_SumPlot(sum_args);
  }
}
