#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_XRaySpectrum.C"

void SiL3_CountStoppedMuons_XRaySpectrum(std::string infilename, std::string outfilename) {

  CountStoppedMuons_XRaySpectrumArgs args;
  args.infilename = infilename;
  args.channel = "GeLoGain";
  args.inhistname = "hXRaySpectrum_" + args.channel;
  args.outfilename = outfilename;
  args.outtreename = "indirect_count";
  args.outwsname = "ws";
  args.material = "Si";
  args.transition = "2p-1s";
  args.rebin_factor = 2;
  args.ge_eff_run = 9302;
  //  args.ge_eff_run = 10319;
  args.min_time = -20000;
  args.max_time = 20000;
  
  CountStoppedMuons_XRaySpectrum(args);
}
