#include "scripts/SiliconAnalyses/Utils/PunchthroughAndStopFit.C"
#include "scripts/SiliconAnalyses/Si16a/fits/FitParams.h"

void Si16a_PunchthroughAndStopFit(std::string infilename, std::string outfilename, bool add_punchthrough) {

  PunchthroughAndStopFitArgs args;
  args.infilename = infilename;
  args.outfilename = outfilename;

  args.add_punchthrough = add_punchthrough;
  args.add_stop = SiL1FitParams::add_stop;
  
  args.rebin_factor = SiL1FitParams::rebin_factor;
  
  args.edep_min = SiL1FitParams::edep_min_wPunchthrough;
  args.edep_max = SiL1FitParams::edep_max_wPunchthrough;

  args.landau_mean_min = SiL1FitParams::landau_mean_min;
  args.landau_mean_max = SiL1FitParams::landau_mean_max;
  args.landau_sigma_min = SiL1FitParams::landau_sigma_min;
  args.landau_sigma_max = SiL1FitParams::landau_sigma_max;
  args.resolution_sigma_min = SiL1FitParams::resolution_sigma_min;
  args.resolution_sigma_max = SiL1FitParams::resolution_sigma_max;
  if (add_punchthrough) {
    args.stop_edep_min = SiL1FitParams::stop_edep_min_wPunchthrough;
  }
  else {
    args.edep_min = SiL1FitParams::edep_min_woutPunchthrough;
    args.edep_max = SiL1FitParams::edep_max_woutPunchthrough;
    args.stop_edep_min = SiL1FitParams::stop_edep_min_woutPunchthrough;
    args.add_punchthrough = true;
  }
  args.stop_edep_max = SiL1FitParams::stop_edep_max;
  args.stop_sigma_min = SiL1FitParams::stop_sigma_min;
  args.stop_sigma_max = SiL1FitParams::stop_sigma_max;
  args.stop_alpha_min = SiL1FitParams::stop_alpha_min;
  args.stop_alpha_max = SiL1FitParams::stop_alpha_max;

  args.n_punch_min = SiL1FitParams::n_punch_min;
  args.n_punch_max = SiL1FitParams::n_punch_max;
  args.n_stop_min = SiL1FitParams::n_stop_min;
  args.n_stop_max = SiL1FitParams::n_stop_max;

  args.inhistname = "RawSpectrum_fromEnergyTime_Thin_wNoThick_TimeSlice-200_200/hRawSpectrum";
  args.outdirname = "PunchthroughAndStopFit_Thin_wNoThick";
  PunchthroughAndStopFit(args);

  args.inhistname = "RawSpectrum_fromEnergyTime_Thin_All_TimeSlice-200_200/hRawSpectrum";
  args.outdirname = "PunchthroughAndStopFit_Thin_All";
  PunchthroughAndStopFit(args);


  // Now do SiT
  args.add_stop = SiTFitParams::add_stop;
  args.rebin_factor = SiTFitParams::rebin_factor;
  
  args.edep_min = SiTFitParams::edep_min_wPunchthrough;
  args.edep_max = SiTFitParams::edep_max_wPunchthrough;

  args.landau_mean_min = SiTFitParams::landau_mean_min;
  args.landau_mean_max = SiTFitParams::landau_mean_max;
  args.landau_sigma_min = SiTFitParams::landau_sigma_min;
  args.landau_sigma_max = SiTFitParams::landau_sigma_max;
  args.resolution_sigma_min = SiTFitParams::resolution_sigma_min;
  args.resolution_sigma_max = SiTFitParams::resolution_sigma_max;
  args.stop_edep_min = SiTFitParams::stop_edep_min;
  args.stop_edep_max = SiTFitParams::stop_edep_max;
  args.stop_sigma_min = SiTFitParams::stop_sigma_min;
  args.stop_sigma_max = SiTFitParams::stop_sigma_max;
  args.stop_alpha_min = SiTFitParams::stop_alpha_min;
  args.stop_alpha_max = SiTFitParams::stop_alpha_max;

  args.n_punch_min = SiTFitParams::n_punch_min;
  args.n_punch_max = SiTFitParams::n_punch_max;
  args.n_stop_min = SiTFitParams::n_stop_min;
  args.n_stop_max = SiTFitParams::n_stop_max;
  args.inhistname = "RawSpectrum_fromEnergyTime_SiT_All_TimeSlice-200_200/hRawSpectrum";
  args.outdirname = "PunchthroughAndStopFit_SiT_All";
  PunchthroughAndStopFit(args);
}
