#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_XRaySpectrum.C"

void MT_CountStoppedMuons_XRaySpectrum(std::string infilename, std::string outfilename) {

  const int n_ge_channels = 2;
  std::string ge_channels[n_ge_channels] = {"GeLoGain", "GeHiGain"};

  const int n_materials = 2;
  std::string materials[n_materials] = {"Al", "Si"};
  
  for (int i_ge_channel = 0; i_ge_channel < n_ge_channels; ++i_ge_channel) {

    std::string this_ge_channel = ge_channels[i_ge_channel];

    CountStoppedMuons_XRaySpectrumArgs args;
    args.infilename = infilename;
    args.channel = this_ge_channel;
    args.inhistname = args.channel + "/hEnergyTime";
    args.outfilename = outfilename;

    args.transition = "2p-1s";
    args.rebin_factor = 1;
    args.fit_window_min = -15;
    args.fit_window_max = 15;

    for (int i_material = 0; i_material < n_materials; ++i_material) {
      std::string material = materials[i_material];
      args.material = material;

      args.outdirname = "XRaySpectrum_" + args.channel + "_" + args.material + "_wTimeCut";
      args.min_time = -200;
      args.max_time = 200;
      CountStoppedMuons_XRaySpectrum(args);
  
      args.outdirname = "XRaySpectrum_" + args.channel + "_" + args.material + "_noTimeCut";
      args.min_time = -20000;
      args.max_time = 20000;  
      CountStoppedMuons_XRaySpectrum(args);
    }
  }
}
