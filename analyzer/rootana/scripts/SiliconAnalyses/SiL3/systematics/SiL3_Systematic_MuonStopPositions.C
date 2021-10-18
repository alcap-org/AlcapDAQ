#include "../unfold/SiL3_Unfold_DecayElectronCorrection.C"
//#include "../unfold/SiL3_Unfold_ResponseMatrix.C"
#include "../unfold/SiL3_Unfold_FinalNormalisation.C"
#include "../../Utils/Systematic_MakePlot.C"

void SiL3_Systematic_MuonStopPositions(std::string infilename, std::string outfilename,
				       std::string countfilename, std::string counttreename,
				       std::string time_slice_str, std::string recoil_name) {

  // Number of settings
  const int n_settings = 3;
  std::string settings[n_settings] = {"", "higherE", "lowerE"};

  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = settings[i_setting];

    std::string outdirname = "TimeCut_" + time_slice_str + "_" + recoil_name;
    std::string inhistname = outdirname + "/hCorrectedSpectrum";
    //    std::string corrfilename = "~/data/mc/SiL3/decayCorr_1M_Geom-P1" + setting + "_muplus.root";
    std::string corrfilename = "~/data/mc/SiL3/decayCorr_1M_Geom-P1";
    if (setting != "") {
      corrfilename += "-" + setting;
    }
    corrfilename += "_muplus_KFactor0-85_MeV.root";
    std::string corrhistname = "hEDep_muplus";
    outdirname = "DecayElectron_" + time_slice_str + "_" + recoil_name + "_MuStopPos" + setting;
    SiL3_Unfold_DecayElectronCorrection(infilename, inhistname, corrfilename, corrhistname, countfilename, counttreename, outfilename, outdirname);

    
    //    inhistname = outdirname + "/hCorrectedSpectrum";
    //    outdirname = "ResponseMatrix_" + time_slice_str + "_" + recoil_name + "_MuStopPos" + setting;
    //    std::string response = "combined";
    //    SiL3_Unfold_ResponseMatrix(outfilename, outfilename, inhistname, outdirname, response, setting);

    inhistname = outdirname + "/hCorrectedSpectrum";
    outdirname = "FinalNormalisation_" + time_slice_str + "_" + recoil_name + "_MuStopPos" + setting;
    SiL3_Unfold_FinalNormalisation(outfilename, inhistname, countfilename, counttreename, outfilename, outdirname);

    Systematic_MakePlotArgs args;
    args.central_val_filename = infilename;
    args.syst_val_filename = outfilename;
    args.outfilename = outfilename;

    // 100 keV bins
    args.central_val_histname = "FinalNormalisation_" + time_slice_str + "_" + recoil_name + "_100keVBins/hNormalisedSpectrum";
    args.rebin_factor = 1; // rebinnnig systematics histogram
    args.syst_val_histname = outdirname + "_100keVBins/hNormalisedSpectrum";
    //    args.central_val_histname = "DecayElectron_" + time_slice_str + "_" + recoil_name + "/hCorrectedSpectrum";
    //    args.syst_val_histname = outdirname + "/hCorrectedSpectrum";
    // args.rebin_factor = 2;
    args.outdirname = outdirname + "_100keVBins_SystPlot";
    Systematic_MakePlot(args);

    // 200 keV bins
    args.central_val_histname = "FinalNormalisation_" + time_slice_str + "_" + recoil_name + "_200keVBins/hNormalisedSpectrum";
    args.rebin_factor = 1; // rebinnnig systematics histogram
    args.syst_val_histname = outdirname + "_200keVBins/hNormalisedSpectrum";
    args.outdirname = outdirname + "_200keVBins_SystPlot";
    Systematic_MakePlot(args);

    // 500 keV bins
    args.central_val_histname = "FinalNormalisation_" + time_slice_str + "_" + recoil_name + "_500keVBins/hNormalisedSpectrum";
    args.rebin_factor = 1; // rebinnnig systematics histogram
    args.syst_val_histname = outdirname + "_500keVBins/hNormalisedSpectrum";
    args.outdirname = outdirname + "_500keVBins_SystPlot";
    Systematic_MakePlot(args);
  }
}
