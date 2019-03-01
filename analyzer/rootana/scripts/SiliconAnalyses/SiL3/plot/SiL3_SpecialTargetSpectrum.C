#include "scripts/SiliconAnalyses/Utils/TargetSpectrum.C"

void SiL3_SpecialTargetSpectrum(std::string infilename, std::string outfilename) {

  TargetSpectrumArgs args;
  args.infilename = infilename;
  args.treename = "targetBlockTree";
  args.layername = "";
  args.layerveto = false;
  args.layerreqd = false;
  args.outfilename = outfilename;

  args.outdirname = "Target";
  args.min_energy = 0;
  args.max_energy = 50000;
  args.energy_width = 50;
  args.min_time = -20000;
  args.max_time = 20000;
  args.time_width = 10;
  TargetSpectrum(args);

  args.outdirname = "FlatBkg";
  args.min_time = -20000;
  args.max_time = -2000;
  TargetSpectrum(args);
}
