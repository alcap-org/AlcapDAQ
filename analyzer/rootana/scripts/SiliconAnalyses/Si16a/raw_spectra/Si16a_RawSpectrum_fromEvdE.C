#include "scripts/SiliconAnalyses/Utils/RawSpectrum_fromEvdE.C"

void Si16a_RawSpectrum_fromEvdE(std::string infilename, std::string outfilename) {

  RawSpectrum_fromEvdEArgs args;
  args.datafilename = infilename;
  args.outfilename = outfilename;

  const int n_settings = 3;
  std::string indirnames[n_settings] = {"all_Target_timecut-200_200ns_layerCoinc", "all_Target_timecut-200_200ns_thickLateCoinc", "all_Target_timecut-200_200ns_layerCoinc"};
  std::string inhistnames[n_settings] = {"hEvdE_TwoLayer_12", "hEvdE_TwoLayer_12", "hSingleDetAxes_TwoLayer_12"};
  std::string outdirnames[n_settings] = {"muonbeam_thickStop", "muonbeam_thinStop", "muonbeam_thickStop_E2only"};
  bool projection_xs[n_settings] = {true, false, true};
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    args.datahistnames.clear();
    args.scale_ratios.clear();

    args.outdirname = outdirnames[i_setting];
    args.projection_x = projection_xs[i_setting];
    std::string indirname = indirnames[i_setting];
    args.datacuttreename = indirname + "/cuttree";
    std::string histname = indirname + "/" + inhistnames[i_setting];
    args.datahistnames.push_back(histname);
    args.scale_ratios.push_back(1);
    RawSpectrum_fromEvdE(args);
  }
}
