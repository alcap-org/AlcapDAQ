#include "scripts/SiliconAnalyses/Utils/XRaySpectrum_wTargetCoinc.C"

void Si16a_XRaySpectrum_wTargetCoinc(std::string infilename, std::string outfilename, double min_muon_energy, double max_muon_energy, double muon_slice_width) {

  const int n_ge_channels = 2;
  std::string ge_channels[n_ge_channels] = {"GeLoGain", "GeHiGain"};
  std::stringstream muon_slice;
  
  for (int i_ge_channel = 0; i_ge_channel < n_ge_channels; ++i_ge_channel) {

    std::string this_ge_channel = ge_channels[i_ge_channel];
    
    XRaySpectrum_wTargetCoincArgs args;
    args.infilename = infilename;
    args.treename = "geTargetTree_" + this_ge_channel;

    for (double i_muon_slice_start = min_muon_energy; i_muon_slice_start < max_muon_energy; i_muon_slice_start += muon_slice_width) {
      double i_muon_slice_end = i_muon_slice_start + muon_slice_width;
      muon_slice.str("");
      muon_slice << "MuonSlice" << (int)i_muon_slice_start << "_" << (int)i_muon_slice_end;
      
      args.layer_coincidence_time = 200;
      args.layername = "thin";
      args.layerreqd = false;
      args.layerveto = true;
      args.vetolayername = "thick";
      
      args.outfilename = outfilename;
      
      args.outhistname = "hXRaySpectrum_wSiL1Coinc_" + this_ge_channel + "_" + muon_slice.str();
      args.min_ge_energy = 0;
      args.max_ge_energy = 2000;
      args.ge_energy_width = 0.5;
      args.min_ge_time = -30000;
      args.max_ge_time = 30000;
      args.ge_time_width = 10;
      
      args.min_muon_energy = i_muon_slice_start;
      args.max_muon_energy = i_muon_slice_end;
      args.min_muon_time = -200;
      args.max_muon_time = 200;
      XRaySpectrum_wTargetCoinc(args);
      
      args.layername = "thick";
      args.layerveto = false;
      args.layerreqd = true;
      args.reqdlayername = "thin";
      args.outhistname = "hXRaySpectrum_wSiL3Coinc_" + this_ge_channel + "_" + muon_slice.str();
      XRaySpectrum_wTargetCoinc(args);
    }
  }
}
