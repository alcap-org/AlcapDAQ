#include "../../Utils/XRaySpectrum.C"

void SiL3_XRaySpectrum(std::string infilename, std::string outfilename) {

  const int n_ge_channels = 2;
  std::string ge_channels[n_ge_channels] = {"GeLoGain", "GeHiGain"};

  for (int i_ge_channel = 0; i_ge_channel < n_ge_channels; ++i_ge_channel) {

    std::string this_ge_channel = ge_channels[i_ge_channel];
    
    XRaySpectrumArgs args;
    args.infilename = infilename;
    args.treename = "geTree_" + this_ge_channel;
    args.outfilename = outfilename;    
    args.outdirname = this_ge_channel;
    args.min_energy = 0;
    args.max_energy = 2000;
    args.energy_width = 0.5;
    // if (this_ge_channel == "GeHiGain") {
    //   args.energy_width = 0.184892;
    // }
    // else {
    //   args.energy_width = 0.396515;
    // }
    args.min_time = -20000;
    args.max_time = 20000;
    args.time_width = 10;
    XRaySpectrum(args);
  }
}
