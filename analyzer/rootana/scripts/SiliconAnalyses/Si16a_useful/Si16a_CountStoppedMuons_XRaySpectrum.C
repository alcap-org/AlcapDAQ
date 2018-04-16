#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_XRaySpectrum.C"

void Si16a_CountStoppedMuons_XRaySpectrum(std::string infilename, std::string outfilename) {

  CountStoppedMuons_XRaySpectrumArgs args;
  args.infilename = infilename;
  args.channel = "GeLoGain";
  args.inhistname = "hXRaySpectrum_" + args.channel;
  args.outfilename = outfilename;
  args.outtreename = "xray_spectrum_indirect";
  args.outwsname = "ws";
  args.material = "Si";
  args.transition = "2p-1s";
  args.rebin_factor = 2;
  args.ge_eff_run = 9302;
  args.min_time = -200;
  args.max_time = 200;
  
  CountStoppedMuons_XRaySpectrum(args);
}
