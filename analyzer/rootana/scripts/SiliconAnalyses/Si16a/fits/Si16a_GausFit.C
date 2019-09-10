#include "scripts/SiliconAnalyses/Utils/GausFit.C"

void Si16a_GausFit(std::string infilename, std::string outfilename, std::string run_str, std::string sf_str, bool mc = false) {

  GausFitArgs args;
  args.infilename = infilename;
  args.outfilename = outfilename;
  
  const int n_settings = 1;
  //  std::string histnames[n_settings] = {"Thin_wNoThick_TimeSlice-200_200/hRawSpectrum", "muonbeam/hRawSpectrum"};
  //  std::string histnames[n_settings] = {"muonbeam_thickStop/hRawSpectrum", "muonbeam_thinStop/hRawSpectrum"};
  std::string histnames[n_settings] = {"muonbeam_thickStop/hRawSpectrum"};
  std::string mchistnames[n_settings] = {"hEvdE_SiL_px"};
  std::string outdirnames[n_settings] = {"GausFit_ThickStop"};

  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    if (mc) {
      args.inhistname = mchistnames[i_setting];
    }
    else {
      args.inhistname = histnames[i_setting];
    }
    args.outdirname = outdirnames[i_setting];

    args.outhisttitle = "Si16a Dataset, Run " + run_str + ", SF = " + sf_str;
    if (mc) {
      args.outhisttitle += " (MC)";
    }

    GausFit(args);
  }
}
