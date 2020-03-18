#include "../unfold/Si16b_Unfold_ResponseMatrix.C"
#include "../unfold/Si16b_Unfold_FinalNormalisation.C"
#include "../../Utils/Systematic_MakePlot.C"

void Si16b_Systematic_UnfoldingBinWidths(std::string infilename, std::string outfilename,
					 std::string countfilename, std::string counttreename,
					 std::string pid, std::string particle_name) {

  // Number of bin widths
  const int n_bin_widths = 2;
  int bin_widths[n_bin_widths] = {500, 1000};

  for (int i_bin_width = 0; i_bin_width < n_bin_widths; ++i_bin_width) {
    int bin_width = bin_widths[i_bin_width];
    int rebin_factor = bin_width/10;
	
    std::string indirname = "TimeCut_" + particle_name + "_" + pid;
    std::string inhistname = indirname + "/hCorrectedSpectrum";

    std::string outdirname = "ResponseMatrix_" + particle_name + "_" + pid + "_BinW" + std::to_string(bin_width);

    std::string mcresponsename = "SiR_two_layer_response";
    if (particle_name == "proton") {
      mcresponsename = "SiR_three_layer_response";
    }

    std::string mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5_" + particle_name + "_" + std::to_string(bin_width) + "keVBins.root";
    if (particle_name == "alpha") {
      mcfilename = "~/data/mc/Si16b/respMatrix_12M_Geom-P5_" + particle_name + "_" + std::to_string(bin_width) + "keVBins.root";
    }

    Si16b_Unfold_ResponseMatrix(infilename, outfilename, inhistname, mcfilename, mcresponsename, rebin_factor, outdirname, "bayes");

    inhistname = outdirname + "/hCorrectedSpectrum";
    outdirname = "FinalNormalisation_" + particle_name + "_" + pid + "_BinW" + std::to_string(bin_width);
    Si16b_Unfold_FinalNormalisation(outfilename, inhistname, countfilename, counttreename, outfilename, outdirname);

    Systematic_MakePlotArgs args;
    args.central_val_filename = infilename;
    args.central_val_histname = "FinalNormalisation_" + particle_name + "_" + pid + "/hNormalisedSpectrum";
    args.rebin_factor = 1000 / bin_width; // need to rebin the 500 keV to be 1 MeV to see any change in rates
    args.syst_val_filename = outfilename;
    args.syst_val_histname = outdirname + "/hNormalisedSpectrum";
    args.outfilename = outfilename;
    args.outdirname = outdirname + "_SystPlot";
    Systematic_MakePlot(args);
  }
}
