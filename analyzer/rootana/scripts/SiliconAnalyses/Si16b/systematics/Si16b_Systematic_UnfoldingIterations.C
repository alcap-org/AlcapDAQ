#include "../unfold/Si16b_Unfold_ResponseMatrix.C"
#include "../unfold/Si16b_Unfold_FinalNormalisation.C"
#include "../../Utils/Systematic_MakePlot.C"

void Si16b_Systematic_UnfoldingIterations(std::string infilename, std::string outfilename,
					  std::string countfilename, std::string counttreename,
					  std::string pid, std::string particle_name,
					  std::string sigma, std::string layer_coinc,
					  std::string timecut, std::string bin_width, double bin_width_val) {

  // Number of iterations
  const int n_iterations = 2;
  int iterations[n_iterations] = {4, 20};
  int rebin_factor = 1;

  for (int i_iteration = 0; i_iteration < n_iterations; ++i_iteration) {
    int iteration = iterations[i_iteration];
	
    std::string indirname = "TimeCut_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut;
    std::string inhistname = indirname + "/hCorrectedSpectrum";

    std::string outdirname = "ResponseMatrix_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_ " + bin_width + "_NIter" + std::to_string(iteration);

    std::string mcresponsename = "SiR_two_layer_response";
    if (particle_name == "proton") {
      mcresponsename = "SiR_three_layer_response";
    }

    std::string mcfilename = "/data/R15bMC/respMatrix_100M_Si16b_" + particle_name + "_100keVBins.root";
    Si16b_Unfold_ResponseMatrix(infilename, outfilename, inhistname, mcfilename, mcresponsename, bin_width_val, outdirname, 10000, "bayes", iteration);

    inhistname = outdirname + "/hCorrectedSpectrum";
    outdirname = "FinalNormalisation_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut  + "_" + bin_width + "_NIter" + std::to_string(iteration);
    Si16b_Unfold_FinalNormalisation(outfilename, inhistname, countfilename, counttreename, outfilename, outdirname);

    Systematic_MakePlotArgs args;
    args.central_val_filename = infilename;
    args.central_val_histname = "FinalNormalisation_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "/hNormalisedSpectrum";
    args.rebin_factor = 1;
    args.central_rebin_factor = args.rebin_factor;
    args.syst_val_filename = outfilename;
    args.syst_val_histname = outdirname + "/hNormalisedSpectrum";
    args.outfilename = outfilename;
    args.outdirname = outdirname + "_SystPlot";
    Systematic_MakePlot(args);
  }
}
