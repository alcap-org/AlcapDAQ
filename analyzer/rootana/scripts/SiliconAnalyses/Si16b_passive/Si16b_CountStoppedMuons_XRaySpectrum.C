#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_XRaySpectrum.C"

void Si16b_CountStoppedMuons_XRaySpectrum() {

  CountStoppedMuons_XRaySpectrumArgs args;
  args.infilename = "~/data/results/Si16b_passive/plots.root";
  args.channel = "GeLoGain";
  args.inhistname = "hEnergyTime_" + args.channel;
  args.outfilename = "~/data/results/Si16b_passive/normalisation.root";
  args.material = "Si";
  args.transition = "2p-1s";
  args.rebin_factor = 4;
  
  CountStoppedMuons_XRaySpectrum(args);
}
