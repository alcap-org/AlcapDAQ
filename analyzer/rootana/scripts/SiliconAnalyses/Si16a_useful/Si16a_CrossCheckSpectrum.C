#include "scripts/SiliconAnalyses/Utils/CrossCheckSpectrum.C"

void Si16a_CrossCheckSpectrum(std::string infilename, std::string outfilename, double min_muon_energy, double max_muon_energy) {

  const int n_ge_channels = 2;
  std::string ge_channels[n_ge_channels] = {"GeLoGain", "GeHiGain"};
  
  for (int i_ge_channel = 0; i_ge_channel < n_ge_channels; ++i_ge_channel) {

    std::string this_ge_channel = ge_channels[i_ge_channel];
    
    CrossCheckSpectrumArgs args;
    args.infilename = infilename;
    args.treename = "geTargetTree_" + this_ge_channel;

    args.layername = "thin";
    args.layerveto = true;
    args.vetolayername = "thick";

    args.outfilename = outfilename;
    
    args.outhistname = "hCrossCheck_" + this_ge_channel;
    args.min_ge_energy = 0;
    args.max_ge_energy = 2000;
    args.ge_energy_width = 0.5;
    args.min_ge_time = -30000;
    args.max_ge_time = 30000;
    args.ge_time_width = 10;

    args.min_muon_energy = min_muon_energy;
    args.max_muon_energy = max_muon_energy;
    args.min_muon_time = -200;
    args.max_muon_time = 200;
    CrossCheckSpectrum(args);
  }
}
