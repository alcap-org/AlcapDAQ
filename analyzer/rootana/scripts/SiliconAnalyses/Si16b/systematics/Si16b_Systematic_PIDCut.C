//#include "../../Utils/Systematic_MakePlot.C"

void Si16b_Systematic_PIDCut(std::string infilename, std::string outfilename,
			     std::string pid, std::string particle_name) {

  // Number of pids
  const int n_sigmas = 3;
  int sigmas[n_sigmas] = {3, 2, 1};

  for (int i_sigma = 0; i_sigma < n_sigmas; ++i_sigma) {
    int sigma = sigmas[i_sigma];

    std::string systfilename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse";
    //    if (sigma != 3) {
    systfilename += "_" + std::to_string(sigma) + "sigma";
    //    }
    systfilename += ".root";

    Systematic_MakePlotArgs args;
    args.central_val_filename = infilename;
    args.central_val_histname = "FinalNormalisation_" + particle_name + "_" + pid + "/hNormalisedSpectrum";
    args.rebin_factor = 1;
    args.syst_val_filename = systfilename;
    args.syst_val_histname = args.central_val_histname; // same histogram, different file
    args.outfilename = outfilename;
    args.outdirname = "FinalNormalisation_" + particle_name + "_" + pid + "_PID" + std::to_string(sigma) + "sigma_SystPlot";
    Systematic_MakePlot(args);
  }
}
