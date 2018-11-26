#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_XRaySpectrum.C"
#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_TargetSpectrum.C"

void Si16a_CountStoppedMuons_XRaySpectrum_wTargetCoinc(std::string infilename, std::string outfilename) {

  const int n_ge_channels = 2;
  std::string ge_channels[n_ge_channels] = {"GeLoGain", "GeHiGain"};  

  const int n_slices = 7;
  double min_muon_energies[n_slices] = {0, 400, 800, 1200, 1600, 500, 900};
  double max_muon_energies[n_slices] = {400, 800, 1200, 1600, 2000, 1500, 1100};

  const int n_transitions = 2;
  std::string transitions[n_transitions] = {"2p-1s", "3p-1s"};

  std::stringstream muon_slice;

  for (int i_slice = 0; i_slice < n_slices; ++i_slice) {
    double i_muon_slice_start = min_muon_energies[i_slice];
    double i_muon_slice_end = max_muon_energies[i_slice];

    muon_slice.str("");
    muon_slice << "MuonSlice" << (int)i_muon_slice_start << "_" << (int)i_muon_slice_end;

    for (int i_ge_channel = 0; i_ge_channel < n_ge_channels; ++i_ge_channel) {
      
      std::string this_ge_channel = ge_channels[i_ge_channel];

      for (int i_transition = 0; i_transition < n_transitions; ++i_transition) {
	std::string this_transition = transitions[i_transition];
  
	CountStoppedMuons_XRaySpectrumArgs xray_args;
	xray_args.infilename = infilename;
	xray_args.channel = this_ge_channel;
	xray_args.transition = this_transition;
	xray_args.outfilename = outfilename;
	xray_args.material = "Si";
	xray_args.rebin_factor = 1;
	xray_args.min_time = -20000;
	xray_args.max_time = 20000;
	xray_args.fit_window_min = -10;
	xray_args.fit_window_max = 10;
	
	xray_args.inhistname = xray_args.channel + "_" + muon_slice.str() + "_wThinNoThickCoinc/hEnergyTime";
	xray_args.outdirname = xray_args.channel + "_wThinNoThickCoinc_" + muon_slice.str() + "_" + xray_args.transition;
	CountStoppedMuons_XRaySpectrum(xray_args);

	xray_args.inhistname = xray_args.channel + "_" + muon_slice.str() + "_wThinAndThickCoinc/hEnergyTime";
	xray_args.outdirname = xray_args.channel + "_wThinAndThickCoinc_" + muon_slice.str() + "_" + xray_args.transition;
	CountStoppedMuons_XRaySpectrum(xray_args);
      }
    }

    CountStoppedMuons_TargetSpectrumArgs tgt_args;
    tgt_args.infilename = infilename;
    tgt_args.outfilename = outfilename;
    tgt_args.min_muon_time = -300;
    tgt_args.max_muon_time = 300;
    tgt_args.min_muon_energy = i_muon_slice_start;
    tgt_args.max_muon_energy = i_muon_slice_end;

    tgt_args.inhistname = "Target_Thin_wNoThick/hEnergyTime";
    tgt_args.outdirname = "TargetSpectrum_wThinNoThickCoinc_forCrossCheck_" + muon_slice.str();;
    CountStoppedMuons_TargetSpectrum(tgt_args);

    tgt_args.inhistname = "Target_Thin_wThick/hEnergyTime";
    tgt_args.outdirname = "TargetSpectrum_wThinAndThickCoinc_forCrossCheck_" + muon_slice.str();;
    CountStoppedMuons_TargetSpectrum(tgt_args);
  }    
}
