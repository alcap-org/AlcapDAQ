#include "../../Utils/Systematic_MakeGlobalPlot.C"

void Si16b_Systematic_LayerCoincCut(std::string infilename, std::string outfilename,
				    std::string pid, std::string particle_name) {

  Systematic_MakeGlobalPlotArgs args;
  args.central_val_filename = infilename;
  args.central_val_histname = "FinalNormalisation_" + particle_name + "_" + pid + "/hNormalisedSpectrum";
  args.overall_systematic = 2.8;
  args.outfilename = outfilename;
  args.outdirname = "LayerCoincCut_" + pid + "_SystPlot";
  Systematic_MakeGlobalPlot(args);
}
