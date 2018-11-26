#include "scripts/SiliconAnalyses/Utils/TargetSpectrum.C"

void Ti50_TargetSpectrum(std::string infilename, std::string outfilename) {

  // for the integrated charged particle analysis
  TargetSpectrumArgs args;
  args.infilename = infilename;
  args.outfilename = outfilename;
  args.min_energy = 0;
  args.max_energy = 30000;
  args.energy_width = 50;
  args.min_time = -20000;
  args.max_time = 20000;
  args.time_width = 10;
  args.treename = "siBlockTree_SiL";
  args.layername = "thick";  
  args.layerreqd = false;
  args.layerveto = false;
  args.outdirname = "SiL3";
  TargetSpectrum(args);

  // for the integrated charged particle analysis
  args.treename = "siBlockTree_SiR";
  args.layername = "thick";
  args.max_energy = 30000;
  args.energy_width = 50;
  args.time_width = 10;
  args.layerreqd = false;
  args.layerveto = false;
  args.outdirname = "SiR2";
  TargetSpectrum(args);

  args.vetolayername = "thin";
  args.reqdlayername = "third";
  args.layerreqd = true;
  args.layerveto = true;
  args.outdirname = "SiR23";
  TargetSpectrum(args);

  
  args.outdirname = "SiL3_FlatBkg";
  args.min_time = -2000;
  args.max_time = 0;
  args.time_width = 10;
  TargetSpectrum(args);
}
