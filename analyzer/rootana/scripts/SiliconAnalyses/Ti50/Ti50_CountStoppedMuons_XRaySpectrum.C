#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_XRaySpectrum.C"

void Ti50_CountStoppedMuons_XRaySpectrum(std::string infilename, std::string outfilename) {

  CountStoppedMuons_XRaySpectrumArgs args;
  args.infilename = infilename;
  args.channel = "GeLoGain";
  args.inhistname = "hXRaySpectrum_" + args.channel;
  args.outfilename = outfilename;
  args.outtreename = "xray_spectrum_indirect_timecut";
  args.outwsname = "ws_timecut";
  args.material = "Ti";
  args.transition = "2p-1s";
  args.rebin_factor = 1;
  args.ge_eff_run = 9302;
  args.min_time = -200;
  args.max_time = 200;
  args.fit_window_min = -15;
  args.fit_window_max = 15;
  CountStoppedMuons_XRaySpectrum(args);

  args.outtreename = "xray_spectrum_indirect_notimecut";
  args.outwsname = "ws_notimecut";
  args.min_time = -20000;
  args.max_time = 20000;  
  CountStoppedMuons_XRaySpectrum(args);

}
