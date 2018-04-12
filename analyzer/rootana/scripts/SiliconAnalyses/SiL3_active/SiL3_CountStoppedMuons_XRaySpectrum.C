#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_XRaySpectrum.C"

void SiL3_CountStoppedMuons_XRaySpectrum(std::string infilename, std::string outfilename) {

  CountStoppedMuons_XRaySpectrumArgs args;
  args.infilename = infilename;
  args.channel = "GeLoGain";
  args.inhistname = "hEnergyTime_" + args.channel;
  args.outfilename = outfilename;
  args.material = "Si";
  args.transition = "2p-1s";
  args.rebin_factor = 4;
  
  CountStoppedMuons_XRaySpectrum(args);
}
