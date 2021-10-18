//#include "../unfold/SiL3_Unfold_ResponseMatrix.C"
#include "../unfold/SiL3_Unfold_FinalNormalisation.C"
#include "../../Utils/Systematic_MakePlot.C"

void SiL3_Systematic_TimeSlice(std::string infilename, std::string outfilename,
			       std::string countfilename, std::string counttreename,
			       std::string time_slice_str, std::string recoil) {

  // const int n_time_slices = 5;
  // double min_time_slices[n_time_slices] = {2000, 2000, 2500, 3000, 3500};
  // double max_time_slices[n_time_slices] = {4000, 2500, 3000, 3500, 4000};
  //  const int n_time_slices = 3;
  //  double min_time_slices[n_time_slices] = {2000, 2500, 3000};
  //  double max_time_slices[n_time_slices] = {4000, 4000, 4000};
  const int n_time_slices = 2;
  double min_time_slices[n_time_slices] = {3000, 3500};
  double max_time_slices[n_time_slices] = {3500, 4000};

  std::stringstream syst_time_slice;
  for (int i_time_slice = 0; i_time_slice < n_time_slices; ++i_time_slice) {
    double i_min_time_slice = min_time_slices[i_time_slice];
    double i_max_time_slice = max_time_slices[i_time_slice];
    
    syst_time_slice.str("");
    syst_time_slice << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;

    Systematic_MakePlotArgs args;
    args.central_val_filename = infilename;
    args.syst_val_filename = infilename;
    args.outfilename = outfilename;

    std::string outdirname = "FinalNormalisation_" + syst_time_slice.str() + "_" + time_slice_str + "Syst_" + recoil;
    //    std::string outdirname = "DecayElectron_" + syst_time_slice.str() + "_" + time_slice_str + "Syst_" + recoil;
    
    // 100 keV Bins
    args.central_val_histname = "FinalNormalisation_" + time_slice_str + "_" + recoil + "_100keVBins/hNormalisedSpectrum";
    args.syst_val_histname = "FinalNormalisation_" + syst_time_slice.str() + "_" + recoil + "_100keVBins/hNormalisedSpectrum";
    args.rebin_factor = 1;
    args.central_rebin_factor = args.rebin_factor;
    //    args.central_val_histname = "DecayElectron_" + time_slice_str + "_" + recoil + "/hCorrectedSpectrum";
    //    args.syst_val_histname = "DecayElectron_" + syst_time_slice.str() + "_" + recoil + "/hCorrectedSpectrum";
    //    args.rebin_factor = 2; // rebinning systematic hist
    args.outdirname = outdirname + "_100keVBins_SystPlot";
    Systematic_MakePlot(args);

    // // 200 keV Bins
    args.central_val_histname = "FinalNormalisation_" + time_slice_str + "_" + recoil + "_200keVBins/hNormalisedSpectrum";
    args.syst_val_histname = "FinalNormalisation_" + syst_time_slice.str() + "_" + recoil + "_200keVBins/hNormalisedSpectrum";
    args.rebin_factor = 1; // rebinning systematic hist
    args.central_rebin_factor = args.rebin_factor;
    args.outdirname = outdirname + "_200keVBins_SystPlot";
    Systematic_MakePlot(args);

    // // 500 keV Bins
    args.central_val_histname = "FinalNormalisation_" + time_slice_str + "_" + recoil + "_500keVBins/hNormalisedSpectrum";
    args.syst_val_histname = "FinalNormalisation_" + syst_time_slice.str() + "_" + recoil + "_500keVBins/hNormalisedSpectrum";
    args.rebin_factor = 1; // rebinning systematic hist
    args.central_rebin_factor = args.rebin_factor;
    args.outdirname = outdirname + "_500keVBins_SystPlot";
    Systematic_MakePlot(args);
  }
}
