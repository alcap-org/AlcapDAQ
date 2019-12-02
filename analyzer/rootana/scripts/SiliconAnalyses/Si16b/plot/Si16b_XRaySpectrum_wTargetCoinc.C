#include "../../Utils/XRaySpectrum_wTargetCoinc.C"

void Si16b_XRaySpectrum_wTargetCoinc(std::string infilename, std::string outfilename) {

  const int n_ge_channels = 2;
  std::string ge_channels[n_ge_channels] = {"GeLoGain", "GeHiGain"};
  
  for (int i_ge_channel = 0; i_ge_channel < n_ge_channels; ++i_ge_channel) {

    std::string this_ge_channel = ge_channels[i_ge_channel];
    
    XRaySpectrum_wTargetCoincArgs args;
    args.infilename = infilename;
    args.treename = "geTargetTree_" + this_ge_channel;

    args.layername = "thin";
    args.layerveto = false;
    args.layerreqd = false;
    
    args.outfilename = outfilename;
    
    args.min_ge_energy = 0;
    args.max_ge_energy = 2000;
    args.ge_energy_width = 0.5;
    args.min_ge_time = -20000;
    args.max_ge_time = 20000;
    args.ge_time_width = 10;

    args.min_muon_time = -200;
    args.max_muon_time = 200;

    double first_muon_energy_slice = 0;
    double last_muon_energy_slice = 1500;
    //    double muon_energy_slice = 100;
    double muon_energy_slice = 1500;
    std::stringstream muon_slice;
    for (double min_muon_energy = first_muon_energy_slice; min_muon_energy < last_muon_energy_slice; min_muon_energy += muon_energy_slice) {
      double max_muon_energy = min_muon_energy + muon_energy_slice;

      muon_slice.str("");
      muon_slice << "MuonSlice" << min_muon_energy << "_" << max_muon_energy;

      args.outdirname = this_ge_channel + "_wTargetCoinc_" + muon_slice.str();
      args.min_muon_energy = min_muon_energy;
      args.max_muon_energy = max_muon_energy;
      XRaySpectrum_wTargetCoinc(args);
    }
  }
}
