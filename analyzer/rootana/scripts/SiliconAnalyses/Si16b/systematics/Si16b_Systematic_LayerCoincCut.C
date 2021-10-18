#include "../../Utils/Systematic_MakeGlobalPlot.C"
#include "../../Utils/Systematic_MakePlot.C"

void Si16b_Systematic_LayerCoincCut(std::string infilename, std::string outfilename,
				    std::string pid, std::string particle_name,
				    std::string sigma, std::string layer_coinc,
				    std::string timecut, std::string bin_width) {

  // Systematic_MakeGlobalPlotArgs args;
  // args.central_val_filename = infilename;
  // args.central_val_histname = "FinalNormalisation_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "/hNormalisedSpectrum";
  // args.overall_systematic = 0.2;//2.8;
  // args.outfilename = outfilename;
  // args.outdirname = "LayerCoincCut_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_SystPlot";
  // Systematic_MakeGlobalPlot(args);
  
  Systematic_MakePlotArgs args;
  args.central_val_filename = infilename;
  args.central_val_histname = "FinalNormalisation_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "/hNormalisedSpectrum";
  args.rebin_factor = 1;
  args.central_rebin_factor = args.rebin_factor;
  args.syst_val_filename = infilename;
  args.syst_val_histname = "FinalNormalisation_" + particle_name + "_" + pid + "_" + sigma + "_layerCoinc200ns_" + timecut + "_" + bin_width + "/hNormalisedSpectrum";
  args.outfilename = outfilename;
  args.outdirname = "FinalNormalisation_" + particle_name + "_" + pid + "_" + sigma + "_" + layer_coinc + "_" + timecut + "_" + bin_width + "_LayerCoinc200ns_SystPlot";
  Systematic_MakePlot(args);

}
