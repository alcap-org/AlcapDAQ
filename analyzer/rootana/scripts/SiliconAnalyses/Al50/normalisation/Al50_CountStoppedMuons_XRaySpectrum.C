#include "../../Utils/CountStoppedMuons_XRaySpectrum.C"

void Al50_CountStoppedMuons_XRaySpectrum(std::string infilename, std::string outfilename) {

  // Do a fit to John's spectrum
  CountStoppedMuons_XRaySpectrumArgs john_args;
  john_args.infilename = infilename;
  john_args.material = "Al";
  john_args.transition = "2p-1s";
  john_args.rebin_factor = 1;
  john_args.fit_window_min = -8;
  john_args.fit_window_max = 8;
  john_args.outfilename = outfilename;
  john_args.channel = "GeHiGain";
  john_args.inhistname = "hgehi_e_all";
  john_args.min_time = -5000;//min_times[i_time_cut];
  john_args.max_time = 5000;//max_times[i_time_cut];
  john_args.outdirname = "John";
  john_args.is2DHist = false;
  CountStoppedMuons_XRaySpectrum(john_args);
}
