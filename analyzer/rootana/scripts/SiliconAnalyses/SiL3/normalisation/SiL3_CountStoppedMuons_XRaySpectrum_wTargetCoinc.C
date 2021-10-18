#include "../../Utils/CountStoppedMuons_XRaySpectrum.C"
#include "../../Utils/CountStoppedMuons_TargetSpectrum.C"

void SiL3_CountStoppedMuons_XRaySpectrum_wTargetCoinc(std::string infilename, std::string outfilename) {

  const int n_ge_channels = 2;
  std::string ge_channels[n_ge_channels] = {"GeLoGain", "GeHiGain"};  

  const int n_slices = 4;
  double min_muon_energies[n_slices] = {0, 3000, 3000, 4000};
  double max_muon_energies[n_slices] = {30000, 6000, 4000, 5000};

  const int n_transitions = 2;
  std::string transitions[n_transitions] = {"2p", "3p"};

  std::stringstream muon_slice;

  for (int i_slice = 0; i_slice < n_slices; ++i_slice) {
    double i_muon_slice_start = min_muon_energies[i_slice];
    double i_muon_slice_end = max_muon_energies[i_slice];

    muon_slice.str("");
    muon_slice << "MuonSlice" << (int)i_muon_slice_start << "_" << (int)i_muon_slice_end;

    for (int i_ge_channel = 0; i_ge_channel < n_ge_channels; ++i_ge_channel) {
      
      std::string this_ge_channel = ge_channels[i_ge_channel];

      for (int i_transition = 0; i_transition < n_transitions; ++i_transition) {
	std::string this_transition = transitions[i_transition] + "-1s";
	
	CountStoppedMuons_XRaySpectrumArgs xray_args;
	xray_args.infilename = infilename;
	xray_args.channel = this_ge_channel;
	xray_args.transition = this_transition;
	xray_args.inhistname = xray_args.channel + "_" + muon_slice.str() + "_wTargetCoinc/hEnergyTime";
	xray_args.outfilename = outfilename;
	xray_args.outdirname = xray_args.channel + "_wSiL3Coinc_" + muon_slice.str() + "_" + transitions[i_transition] + "1s";
	xray_args.material = "Si";
	xray_args.rebin_factor = 1;
	xray_args.min_time = -20000;
	xray_args.max_time = 20000;
	xray_args.fit_window_min = -10;
	xray_args.fit_window_max = 10;
	xray_args.is2DHist = true;
	CountStoppedMuons_XRaySpectrum(xray_args);
      }
    }

    CountStoppedMuons_TargetSpectrumArgs tgt_args;
    tgt_args.infilename = infilename;
    tgt_args.inhistname = "Target/hEnergyTime";
    tgt_args.outfilename = outfilename;
    tgt_args.outdirname = "TargetSpectrum_forCrossCheck_" + muon_slice.str(); // will add the muon slice string itself
    tgt_args.min_muon_time = -300;
    tgt_args.max_muon_time = 300;
    tgt_args.min_muon_energy = i_muon_slice_start;
    tgt_args.max_muon_energy = i_muon_slice_end;
    
    CountStoppedMuons_TargetSpectrum(tgt_args);
  }
}
