#include "../../Utils/CountStoppedMuons_XRaySpectrum.C"

void SiL3_CountStoppedMuons_XRaySpectrum(std::string infilename, std::string outfilename) {

  const int n_ge_channels = 2;
  std::string ge_channels[n_ge_channels] = {"GeLoGain", "GeHiGain"};

  const int n_time_cuts = 4;
  double min_times[n_time_cuts] = {-20000, -10000, -5000, -200};
  double max_times[n_time_cuts] = {20000, 10000, 5000, 200};
  std::string time_cuts[n_time_cuts] = {"noTimeCut", "10000nsTimeCut", "5000nsTimeCut", "200nsTimeCut"};

  const int n_transitions = 4;
  std::string transitions[n_transitions] = {"2p-1s", "3p-1s", "Capture843keV", "Capture1014keV"};
  std::string labels[n_transitions] = {"2p1s", "3p1s", "Capture843keV", "Capture1014keV"};

  CountStoppedMuons_XRaySpectrumArgs args;
  args.infilename = infilename;
  args.material = "Si";
  args.rebin_factor = 1;
  args.fit_window_min = -10;
  args.fit_window_max = 10;
  args.outfilename = outfilename;
  args.is2DHist = true;
    
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

	if (args.transition == "Capture843keV") {
	  args.rebin_factor = 2;
	}
	else {
	  args.rebin_factor = 1;
	}

	args.outdirname = "XRaySpectrum_" + args.channel + "_" + this_transition + "_" + this_time_cut;
	CountStoppedMuons_XRaySpectrum(args);
      }
    }
  }
}
