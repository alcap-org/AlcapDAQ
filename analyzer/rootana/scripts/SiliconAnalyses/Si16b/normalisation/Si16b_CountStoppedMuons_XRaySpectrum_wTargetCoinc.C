#include "../../Utils/CountStoppedMuons_XRaySpectrum.C"
#include "../../Utils/CountStoppedMuons_TargetSpectrum.C"

void Si16b_CountStoppedMuons_XRaySpectrum_wTargetCoinc(std::string infilename, std::string outfilename) {

  double first_muon_energy_slice = 0;
  double last_muon_energy_slice = 1500;
  //  double muon_energy_slice = 100;
  double muon_energy_slice = 1500;
  
  std::stringstream muon_slice;
  
  for (double min_muon_energy = first_muon_energy_slice; min_muon_energy < last_muon_energy_slice; min_muon_energy += muon_energy_slice) {
    double max_muon_energy = min_muon_energy + muon_energy_slice;

    muon_slice.str("");
    muon_slice << "MuonSlice" << min_muon_energy << "_" << max_muon_energy;

    const int n_ge_channels = 2;
    std::string ge_channels[n_ge_channels] = {"GeLoGain", "GeHiGain"};
    
    for (int i_ge_channel = 0; i_ge_channel < n_ge_channels; ++i_ge_channel) {
      
      std::string this_ge_channel = ge_channels[i_ge_channel];
      
      CountStoppedMuons_XRaySpectrumArgs xray_args;
      xray_args.infilename = infilename;
      xray_args.channel = this_ge_channel;
      xray_args.inhistname = xray_args.channel + "_wTargetCoinc_" + muon_slice.str() + "/hEnergyTime";
      xray_args.outfilename = outfilename;
      xray_args.outdirname = xray_args.channel + "_wTargetCoinc_" + muon_slice.str();
      
      xray_args.material = "Si";
      xray_args.transition = "2p-1s";
      xray_args.rebin_factor = 2;
      xray_args.fit_window_min = -10;
      xray_args.fit_window_max = 10;
      xray_args.min_time = -200;
      xray_args.max_time = 200;
      xray_args.is2DHist = true;
      CountStoppedMuons_XRaySpectrum(xray_args);
    }
    
    CountStoppedMuons_TargetSpectrumArgs tgt_args;
    tgt_args.infilename = infilename;
    tgt_args.inhistname = "Target/hEnergyTime";
    tgt_args.outfilename = outfilename;
    tgt_args.outdirname = "Target_" + muon_slice.str();
    tgt_args.min_muon_time = -200;
    tgt_args.max_muon_time = 200;
    tgt_args.min_muon_energy = min_muon_energy;
    tgt_args.max_muon_energy = max_muon_energy;
    CountStoppedMuons_TargetSpectrum(tgt_args);
  }
}
