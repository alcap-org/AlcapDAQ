#include "scripts/SiliconAnalyses/Utils/XRaySpectrum_wTargetCoinc.C"

void Si16a_XRaySpectrum_wTargetCoinc(std::string infilename, std::string outfilename) {

  const int n_ge_channels = 2;
  std::string ge_channels[n_ge_channels] = {"GeLoGain", "GeHiGain"};
  std::stringstream muon_slice;

  const int n_slices = 7;
  double min_muon_energies[n_slices] = {0, 400, 800, 1200, 1600, 500, 900};
  double max_muon_energies[n_slices] = {400, 800, 1200, 1600, 2000, 1500, 1100};
  
  for (int i_ge_channel = 0; i_ge_channel < n_ge_channels; ++i_ge_channel) {

    std::string this_ge_channel = ge_channels[i_ge_channel];
    
    XRaySpectrum_wTargetCoincArgs args;
    args.infilename = infilename;
    args.treename = "geTargetTree_" + this_ge_channel;
    args.outfilename = outfilename;

    args.min_ge_energy = 0;
    args.max_ge_energy = 2000;
    args.ge_energy_width = 0.5;
    args.min_ge_time = -20000;
    args.max_ge_time = 20000;
    args.ge_time_width = 10;

    for (int i_slice = 0; i_slice < n_slices; ++i_slice) {
      double i_muon_slice_start = min_muon_energies[i_slice];
      double i_muon_slice_end = max_muon_energies[i_slice];

      args.min_muon_energy = i_muon_slice_start;
      args.max_muon_energy = i_muon_slice_end;
      args.min_muon_time = -300;
      args.max_muon_time = 300;

      muon_slice.str("");
      muon_slice << "MuonSlice" << (int)args.min_muon_energy << "_" << (int)args.max_muon_energy;
      
      args.layer_coincidence_time = 200;
      args.layername = "thin";
      args.layerreqd = false;
      args.layerveto = true;
      args.vetolayername = "thick";      
      args.outdirname = this_ge_channel + "_" + muon_slice.str() + "_wThinNoThickCoinc";
      XRaySpectrum_wTargetCoinc(args);
      
      args.layername = "thick";
      args.layerveto = false;
      args.layerreqd = true;
      args.reqdlayername = "thin";
      args.outdirname = this_ge_channel + "_" + muon_slice.str() + "_wThinAndThickCoinc";
      XRaySpectrum_wTargetCoinc(args);
    }
  }
}
