#include "scripts/SiliconAnalyses/Utils/XRaySpectrum.C"

void Si16b_XRaySpectrum(std::string infilename, std::string outfilename) {

  const int n_ge_channels = 2;
  std::string ge_channels[n_ge_channels] = {"GeLoGain", "GeHiGain"};

  for (int i_ge_channel = 0; i_ge_channel < n_ge_channels; ++i_ge_channel) {

    std::string this_ge_channel = ge_channels[i_ge_channel];
    
    XRaySpectrumArgs args;
    args.infilename = infilename;
    args.treename = "getree_" + this_ge_channel;
    args.outfilename = outfilename;
    
    args.outhistname = "hEnergyTime_" + this_ge_channel;
    args.min_energy = 0;
    args.max_energy = 2000;
    args.energy_width = 0.5;
    args.min_time = -30000;
    args.max_time = 30000;
    args.time_width = 10;
    XRaySpectrum(args);
  }
}
