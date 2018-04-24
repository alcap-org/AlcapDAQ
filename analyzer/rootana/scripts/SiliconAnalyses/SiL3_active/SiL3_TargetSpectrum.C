#include "scripts/SiliconAnalyses/Utils/TargetSpectrum.C"

void SiL3_TargetSpectrum(std::string infilename, std::string outfilename) {

  TargetSpectrumArgs args;
  args.infilename = infilename;
  args.treename = "siBlockTree_Target";
  args.layername = "thick";
  args.layerveto = false;
  args.outfilename = outfilename;
  
  args.outhistname = "hTarget";
  args.min_energy = 0;
  args.max_energy = 30000;
  args.energy_width = 100;
  args.min_time = -30000;
  args.max_time = 30000;
  args.time_width = 100;
  TargetSpectrum(args);
}
