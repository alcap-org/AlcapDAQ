#include "../../Utils/CountStoppedMuons_XRaySpectrum.C"

void Ti50_CountStoppedMuons_XRaySpectrum(std::string infilename, std::string outfilename) {

  const int n_ge_channels = 2;
  std::string ge_channels[n_ge_channels] = {"GeLoGain", "GeHiGain"};

  const int n_time_cuts = 3;
  double min_times[n_time_cuts] = {-20000, -5000, -200};
  double max_times[n_time_cuts] = {20000, 5000, 200};
  std::string time_cuts[n_time_cuts] = {"noTimeCut", "5000nsTimeCut", "200nsTimeCut"};

  const int n_transitions = 1;
  std::string transitions[n_transitions] = {"2p-1s"};//, "3p-1s"};//, "Capture843keV", "Capture1014keV"};
  std::string labels[n_transitions] = {"2p1s"};//, "3p1s"};//, "Capture843keV", "Capture1014keV"};

  CountStoppedMuons_XRaySpectrumArgs args;
  args.infilename = infilename;
  args.material = "Ti";
  args.rebin_factor = 2;
  args.fit_window_min = -10;
  args.fit_window_max = 20;
  args.is2DHist  = true;
  args.outfilename = outfilename;

  for (int i_ge_channel = 0; i_ge_channel < n_ge_channels; ++i_ge_channel) {

    std::string this_ge_channel = ge_channels[i_ge_channel];
    args.channel = this_ge_channel;
    args.inhistname = args.channel + "/hEnergyTime";

    for (int i_time_cut = 0; i_time_cut < n_time_cuts; ++i_time_cut) {
      args.min_time = min_times[i_time_cut];
      args.max_time = max_times[i_time_cut];
      std::string this_time_cut = time_cuts[i_time_cut];

      for (int i_transition = 0; i_transition < n_transitions; ++i_transition) {
	args.transition = transitions[i_transition];
	std::string this_transition = labels[i_transition];
	
	args.outdirname = "XRaySpectrum_" + args.channel + "_" + this_transition + "_" + this_time_cut;
	CountStoppedMuons_XRaySpectrum(args);
      }
    }
  }

  // Do a fit to Mark's spectrum
  CountStoppedMuons_XRaySpectrumArgs mark_args;
  mark_args.infilename = "~/data/results/Ti50/Ti_XRaySpectra_fromMark.root";
  mark_args.material = "Ti";
  mark_args.transition = "2p-1s";
  mark_args.rebin_factor = 1;
  mark_args.fit_window_min = -15;//-10;
  mark_args.fit_window_max = 25;//20;
  mark_args.outfilename = outfilename;
  mark_args.channel = "GeHiGain";
  mark_args.inhistname = "h" + args.channel;
  mark_args.min_time = -5000;//min_times[i_time_cut];
  mark_args.max_time = 5000;//max_times[i_time_cut];
  mark_args.outdirname = "Mark";
  mark_args.is2DHist = false;
  CountStoppedMuons_XRaySpectrum(mark_args);
}
