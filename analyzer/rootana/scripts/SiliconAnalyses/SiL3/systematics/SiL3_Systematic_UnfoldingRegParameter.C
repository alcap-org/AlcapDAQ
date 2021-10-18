#include "../unfold/SiL3_Unfold_ResponseMatrix.C"
#include "../unfold/SiL3_Unfold_FinalNormalisation.C"
#include "../../Utils/Systematic_MakePlot.C"

void SiL3_Systematic_UnfoldingRegParameter(std::string infilename, std::string outfilename,
					  std::string countfilename, std::string counttreename,
					   std::string time_slice_str, std::string recoil) {

  // Number of reg_parameters
  const int n_reg_parameters = 3;
  int reg_parameters[n_reg_parameters] = {250, 200, 300};

  for (int i_reg_parameter = 0; i_reg_parameter < n_reg_parameters; ++i_reg_parameter) {
    int reg_parameter = reg_parameters[i_reg_parameter];
	
    std::string inhistname = "DecayElectron_" + time_slice_str  + "_" + recoil + "/hCorrectedSpectrum";
    std::string outdirname = "ResponseMatrix_" + time_slice_str + "_" + recoil + "_KReg" + std::to_string(reg_parameter);

    std::string setting = ""; // central setting
    SiL3_Unfold_ResponseMatrix(infilename, outfilename, inhistname, outdirname, "combined", setting, reg_parameter);

    inhistname = outdirname + "/hCorrectedSpectrum";
    outdirname = "FinalNormalisation_" + time_slice_str + "_" + recoil + "_KReg" + std::to_string(reg_parameter);
    SiL3_Unfold_FinalNormalisation(outfilename, inhistname, countfilename, counttreename, outfilename, outdirname);

    Systematic_MakePlotArgs args;
    args.central_val_filename = infilename;
    args.syst_val_filename = outfilename;
    args.outfilename = outfilename;

    // 100 keV Bins
    args.central_val_histname = "FinalNormalisation_" + time_slice_str + "_" + recoil + "_100keVBins/hNormalisedSpectrum";
    args.syst_val_histname = outdirname + "_100keVBins/hNormalisedSpectrum";
    args.rebin_factor = 1; // rebinning systematic
    args.outdirname = outdirname + "_100keVBins_SystPlot";
    Systematic_MakePlot(args);

    // 200 keV Bins
    args.central_val_histname = "FinalNormalisation_" + time_slice_str + "_" + recoil + "_200keVBins/hNormalisedSpectrum";
    args.syst_val_histname = outdirname + "_200keVBins/hNormalisedSpectrum";
    args.rebin_factor = 1; // rebinning systematic
    args.outdirname = outdirname + "_200keVBins_SystPlot";
    Systematic_MakePlot(args);

    // 500 keV Bins
    args.central_val_histname = "FinalNormalisation_" + time_slice_str + "_" + recoil + "_500keVBins/hNormalisedSpectrum";
    args.syst_val_histname = outdirname + "_500keVBins/hNormalisedSpectrum";
    args.rebin_factor = 1; // rebinning systematic
    args.outdirname = outdirname + "_500keVBins_SystPlot";
    Systematic_MakePlot(args);
  }
}
