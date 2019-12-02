#include "../../Utils/XRaySpectrum_wTargetCoinc.C"

void SiL3_XRaySpectrum_wTargetCoinc(std::string infilename, std::string outfilename) {

  const int n_ge_channels = 2;
  std::string ge_channels[n_ge_channels] = {"GeLoGain", "GeHiGain"};

  const int n_slices = 3;
  double min_muon_energies[n_slices] = {3000, 3000, 4000};
  double max_muon_energies[n_slices] = {6000, 4000, 5000};

  for (int i_ge_channel = 0; i_ge_channel < n_ge_channels; ++i_ge_channel) {

    std::string this_ge_channel = ge_channels[i_ge_channel];
    
    XRaySpectrum_wTargetCoincArgs args;
    args.infilename = infilename;
    args.treename = "geTargetTree_" + this_ge_channel;
    args.outfilename = outfilename;

    args.layername = "thick";
    args.layerveto = false;
    
    args.min_ge_energy = 0;
    args.max_ge_energy = 2000;
    args.ge_energy_width = 0.5;
    args.min_ge_time = -20000;
    args.max_ge_time = 20000;
    args.ge_time_width = 10;

    std::stringstream muon_slice;
    for (int i_slice = 0; i_slice < n_slices; ++i_slice) {
      double i_muon_slice_start = min_muon_energies[i_slice];
      double i_muon_slice_end = max_muon_energies[i_slice];

      args.min_muon_energy = i_muon_slice_start;
      args.max_muon_energy = i_muon_slice_end;
      args.min_muon_time = -300;
      args.max_muon_time = 300;

      muon_slice.str("");
      muon_slice << "MuonSlice" << (int)args.min_muon_energy << "_" << (int)args.max_muon_energy;

      args.outdirname = this_ge_channel + "_" + muon_slice.str() + "_wTargetCoinc";
      XRaySpectrum_wTargetCoinc(args);
    }
  }
}
