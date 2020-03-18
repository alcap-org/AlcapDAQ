#include "../unfold/Si16b_Unfold_ResponseMatrix.C"
#include "../unfold/Si16b_Unfold_FinalNormalisation.C"
#include "../../Utils/Systematic_MakePlot.C"

void Si16b_Systematic_UnfoldingIterations(std::string infilename, std::string outfilename,
					  std::string countfilename, std::string counttreename,
					  std::string pid, std::string particle_name) {

  // Number of iterations
  const int n_iterations = 11;
  int iterations[n_iterations] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 100};
  int rebin_factor = 100;

  for (int i_iteration = 0; i_iteration < n_iterations; ++i_iteration) {
    int iteration = iterations[i_iteration];
	
    std::string indirname = "TimeCut_" + particle_name + "_" + pid;
    std::string inhistname = indirname + "/hCorrectedSpectrum";

    std::string outdirname = "ResponseMatrix_" + particle_name + "_" + pid + "_NIter" + std::to_string(iteration);

    std::string mcresponsename = "SiR_two_layer_response";
    if (particle_name == "proton") {
      mcresponsename = "SiR_three_layer_response";
    }

    std::string mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5_" + particle_name + "_1000keVBins.root";
    if (particle_name == "alpha") {
      mcfilename = "~/data/mc/Si16b/respMatrix_12M_Geom-P5_" + particle_name + "_1000keVBins.root";
    }

    Si16b_Unfold_ResponseMatrix(infilename, outfilename, inhistname, mcfilename, mcresponsename, rebin_factor, outdirname, "bayes", iteration);

    inhistname = outdirname + "/hCorrectedSpectrum";
    outdirname = "FinalNormalisation_" + particle_name + "_" + pid + "_NIter" + std::to_string(iteration);
    Si16b_Unfold_FinalNormalisation(outfilename, inhistname, countfilename, counttreename, outfilename, outdirname);

    Systematic_MakePlotArgs args;
    args.central_val_filename = infilename;
    args.central_val_histname = "FinalNormalisation_" + particle_name + "_" + pid + "/hNormalisedSpectrum";
    args.rebin_factor = 1;
    args.syst_val_filename = outfilename;
    args.syst_val_histname = outdirname + "/hNormalisedSpectrum";
    args.outfilename = outfilename;
    args.outdirname = outdirname + "_SystPlot";
    Systematic_MakePlot(args);
  }
}
