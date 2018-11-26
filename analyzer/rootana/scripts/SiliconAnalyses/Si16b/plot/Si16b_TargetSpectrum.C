#include "scripts/SiliconAnalyses/Utils/TargetSpectrum.C"

void Si16b_TargetSpectrum(std::string infilename, std::string outfilename) {

  TargetSpectrumArgs args;
  args.infilename = infilename;
  args.treename = "siBlockTree_Target";
  args.layername = "thin";
  args.outfilename = outfilename;
  args.min_energy = 0;
  args.max_energy = 5000;
  args.energy_width = 50;
  args.min_time = -20000;
  args.max_time = 20000;
  args.time_width = 100;
  
  args.layerreqd = false;
  args.layerveto = false;
  args.outdirname = "Target";
  TargetSpectrum(args);

  // for the integrated charged particle analysis
  args.treename = "siBlockTree_SiL";
  args.layername = "thick";
  args.max_energy = 30000;
  args.energy_width = 50;
  args.time_width = 10;
  args.layerreqd = false;
  args.layerveto = false;
  args.outdirname = "SiL3";
  TargetSpectrum(args);

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

  args.layerreqd = false;
  args.layerveto = false;
  args.treename = "siBlockTree_SiL";
  args.outdirname = "SiL3_FlatBkg";
  args.min_time = -20000;
  args.max_time = -2000;  
  TargetSpectrum(args);
}
