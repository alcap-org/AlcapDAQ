#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_XRaySpectrum.C"

void Si16b_CountStoppedMuons_XRaySpectrum(std::string infilename, std::string outfilename) {

  const int n_ge_channels = 2;
  std::string ge_channels[n_ge_channels] = {"GeLoGain", "GeHiGain"};

  for (int i_ge_channel = 0; i_ge_channel < n_ge_channels; ++i_ge_channel) {

    std::string this_ge_channel = ge_channels[i_ge_channel];

    CountStoppedMuons_XRaySpectrumArgs args;
    args.infilename = infilename;
    args.channel = this_ge_channel;
    args.inhistname = args.channel + "/hEnergyTime";
    args.outfilename = outfilename;

    args.material = "Si";
    args.transition = "2p-1s";
    args.rebin_factor = 1;
    args.fit_window_min = -10;
    args.fit_window_max = 10;

    args.outdirname = "XRaySpectrum_" + args.channel + "_wTimeCut";
    args.min_time = -200;
    args.max_time = 200;
    CountStoppedMuons_XRaySpectrum(args);
    
    args.outdirname = "XRaySpectrum_" + args.channel + "_noTimeCut";
    args.min_time = -20000;
    args.max_time = 20000;  
    CountStoppedMuons_XRaySpectrum(args);

    args.transition = "3p-1s";

    args.outdirname = "XRaySpectrum_" + args.channel + "_wTimeCut_3p1s";
    args.min_time = -200;
    args.max_time = 200;
    CountStoppedMuons_XRaySpectrum(args);
    
    args.outdirname = "XRaySpectrum_" + args.channel + "_noTimeCut_3p1s";
    args.min_time = -20000;
    args.max_time = 20000;  
    CountStoppedMuons_XRaySpectrum(args);
  }
}
