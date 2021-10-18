#include "../unfold/Si16b_Unfold_ResponseMatrix.C"
#include "../unfold/Si16b_Unfold_FinalNormalisation.C"
#include "../../Utils/Systematic_MakePlot.C"

void Si16b_Systematic_MuonStopPositions(std::string infilename, std::string outfilename,
					std::string countfilename, std::string counttreename,
					std::string pid, std::string particle_name,
					std::string sigma, std::string layer_coinc,
					std::string timecut, std::string bin_width, double bin_width_val) {

  // Number of positions
  const int n_positions = 3;
  std::string positions[n_positions] = {"", "_midLowE", "_midHighE"};//{"", "_higherE", "_lowerE"};
  //  int rebin_factor = 100;

  for (int i_position = 0; i_position < n_positions; ++i_position) {
    std::string position = positions[i_position];
	
    std::string indirname = "TimeCut_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut;
    std::string inhistname = indirname + "/hCorrectedSpectrum";

    std::string outdirname = "ResponseMatrix_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_MuStopPos" + position;

    std::string mcresponsename = "SiR_two_layer_response";
    if (particle_name == "proton") {
      mcresponsename = "SiR_three_layer_response";
    }

    std::string mcfilename = "/data/R15bMC/respMatrix_100M_Si16b_" + particle_name + "_100keVBins" + position + ".root";

    Si16b_Unfold_ResponseMatrix(infilename, outfilename, inhistname, mcfilename, mcresponsename, bin_width_val, outdirname);

    inhistname = outdirname + "/hCorrectedSpectrum";
    outdirname = "FinalNormalisation_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_MuStopPos" + position;
    Si16b_Unfold_FinalNormalisation(outfilename, inhistname, countfilename, counttreename, outfilename, outdirname);

    Systematic_MakePlotArgs args;
    args.central_val_filename = infilename;
    args.central_val_histname = "FinalNormalisation_" + particle_name + "_" + pid +  + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "/hNormalisedSpectrum";
    args.rebin_factor = 1;
    args.central_rebin_factor = args.rebin_factor;
    args.syst_val_filename = outfilename;
    args.syst_val_histname = outdirname + "/hNormalisedSpectrum";
    args.outfilename = outfilename;
    args.outdirname = outdirname + "_SystPlot";
    Systematic_MakePlot(args);
  }
}
