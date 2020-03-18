#include "../unfold/SiL3_Unfold_ResponseMatrix.C"
#include "../unfold/SiL3_Unfold_FinalNormalisation.C"
#include "../../Utils/Systematic_MakePlot.C"

void SiL3_Systematic_ParticleComposition(std::string infilename, std::string outfilename,
					 std::string countfilename, std::string counttreename,
					 std::string time_slice_str, std::string inrecoil) {

  // Number of recoils
  const int n_recoils = 4;
  std::string recoils[n_recoils] = {"allRecoil", "lowAllRecoil", "highAllRecoil", "noRecoil"};
  std::string responses[n_recoils] = {"combined", "combinedLow", "combinedHigh", "combined"};
  
  for (int i_recoil = 0; i_recoil < n_recoils; ++i_recoil) {
    std::string recoil = recoils[i_recoil];
    std::string response = responses[i_recoil];

    std::string inhistname = "DecayElectron_" + time_slice_str  + "_" + recoil + "/hCorrectedSpectrum";
    std::string outdirname = "CombinedEscape_" + time_slice_str + "_" + recoil + "_" + inrecoil + "Syst";
    SiL3_Unfold_ResponseMatrix(infilename, outfilename, inhistname, outdirname, response);
      
    inhistname = outdirname + "/hCorrectedSpectrum";
    outdirname = "FinalNormalisation_" + time_slice_str + "_" + recoil + "_" + inrecoil + "Syst";
    SiL3_Unfold_FinalNormalisation(outfilename, inhistname, countfilename, counttreename, outfilename, outdirname);

    Systematic_MakePlotArgs args;
    args.central_val_filename = infilename;
    args.syst_val_filename = outfilename;
    args.outfilename = outfilename;

    // 100 keV Bins
    args.central_val_histname = "FinalNormalisation_" + time_slice_str + "_" + recoil + "_100keVBins/hNormalisedSpectrum";
    args.syst_val_histname = outdirname + "_100keVBins/hNormalisedSpectrum";
    args.rebin_factor = 1; // rebinning systematic hist
    args.outdirname = outdirname + "_100keVBins_SystPlot";
    Systematic_MakePlot(args);

    // 200 keV Bins
    args.central_val_histname = "FinalNormalisation_" + time_slice_str + "_" + recoil + "_200keVBins/hNormalisedSpectrum";
    args.syst_val_histname = outdirname + "_200keVBins/hNormalisedSpectrum";
    args.rebin_factor = 1; // rebinning systematic hist
    args.outdirname = outdirname + "_200keVBins_SystPlot";
    Systematic_MakePlot(args);

    // 500 keV Bins
    args.central_val_histname = "FinalNormalisation_" + time_slice_str + "_" + recoil + "_500keVBins/hNormalisedSpectrum";
    args.syst_val_histname = outdirname + "_500keVBins/hNormalisedSpectrum";
    args.rebin_factor = 1; // rebinning systematic hist
    args.outdirname = outdirname + "_500keVBins_SystPlot";
    Systematic_MakePlot(args);
  }
}
