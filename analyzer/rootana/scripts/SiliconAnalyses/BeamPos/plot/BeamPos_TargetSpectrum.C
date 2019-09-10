#include "scripts/SiliconAnalyses/Utils/TargetSpectrum.C"

void BeamPos_TargetSpectrum(std::string infilename, std::string outfilename) {

  TargetSpectrumArgs args;
  args.infilename = infilename;
  args.treename = "siBlockTree_Target";
  args.layername = "thin";
  args.outfilename = outfilename;
  args.min_energy = 0;
  args.max_energy = 25000;
  args.energy_width = 10;
  args.min_time = -20000;
  args.max_time = 20000;
  args.time_width = 10;
  
  args.layerreqd = false;
  args.layerveto = false;
  args.outdirname = "Target_Thin_All";
  TargetSpectrum(args);

  args.layerveto = false;
  args.layerreqd = false;
  args.treename = "siBlockTree_SiT";
  args.layername = "thin";
  args.outdirname = "SiT_All";
  args.min_energy = 0;
  args.max_energy = 1500;
  args.energy_width = 10;
  TargetSpectrum(args);
}
