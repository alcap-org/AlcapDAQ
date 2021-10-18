#include "../unfold/Si16b_Unfold_ResponseMatrix.C"
#include "../unfold/Si16b_Unfold_FinalNormalisation.C"
#include "../../Utils/Systematic_MakePlot.C"
#include "../../Utils/Systematic_MakeGlobalPlot.C"

void Si16b_Systematic_SiRPosition(std::string infilename, std::string outfilename,
				  std::string countfilename, std::string counttreename,
				  std::string pid, std::string particle_name,
				  std::string sigma, std::string layer_coinc,
				  std::string timecut, std::string bin_width) {

  Systematic_MakeGlobalPlotArgs args;
  args.central_val_filename = infilename;
  args.central_val_histname = "FinalNormalisation_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "/hNormalisedSpectrum";
  args.overall_systematic = 1.5;//2.8;
  args.outfilename = outfilename;
  args.outdirname = "SiRPosition_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_SystPlot";
  Systematic_MakeGlobalPlot(args);

  // // Number of positions
  // const int n_positions = 3;
  // std::string positions[n_positions] = {"", "_closerSiR", "_furtherSiR"};
  // int rebin_factor = 100;

  // for (int i_position = 0; i_position < n_positions; ++i_position) {
  //   std::string position = positions[i_position];
	
  //   std::string indirname = "TimeCut_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut;
  //   std::string inhistname = indirname + "/hCorrectedSpectrum";

  //   std::string outdirname = "ResponseMatrix_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_SiRPos" + position;

  //   std::string mcresponsename = "SiR_two_layer_response";
  //   if (particle_name == "proton") {
  //     mcresponsename = "SiR_three_layer_response";
  //   }

  //   std::string mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5_" + particle_name + "_1MeVBins" + position + ".root";
  //   if (particle_name == "alpha") {
  //     mcfilename = "~/data/mc/Si16b/respMatrix_12M_Geom-P5_" + particle_name + "_1MeVBins" + position + ".root";
  //   }

  //   Si16b_Unfold_ResponseMatrix(infilename, outfilename, inhistname, mcfilename, mcresponsename, rebin_factor, outdirname, "bayes");

  //   inhistname = outdirname + "/hCorrectedSpectrum";
  //   outdirname = "FinalNormalisation_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_SiRPos" + position;
  //   Si16b_Unfold_FinalNormalisation(outfilename, inhistname, countfilename, counttreename, outfilename, outdirname);

  //   Systematic_MakePlotArgs args;
  //   args.central_val_filename = infilename;
  //   args.central_val_histname = "FinalNormalisation_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "/hNormalisedSpectrum";
  //   args.rebin_factor = 1;
  //   args.syst_val_filename = outfilename;
  //   args.syst_val_histname = outdirname + "/hNormalisedSpectrum";
  //   args.outfilename = outfilename;
  //   args.outdirname = outdirname + "_SystPlot";
  //   Systematic_MakePlot(args);
  // }
}
