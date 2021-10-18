#include "../../Utils/TargetSpectrum.C"

void Si16b_TargetSpectrum(std::string infilename, std::string outfilename) {

  // For a possible active target analysis
  TargetSpectrumArgs args;
  args.infilename = infilename;
  args.treename = "siBlockTree_Target";
  args.layername = "thin";
  args.outfilename = outfilename;
  args.min_energy = 0;
  args.max_energy = 25;
  args.energy_width = 0.05;
  args.min_time = -20000;
  args.max_time = 20000;
  args.time_width = 100;
  args.recoil_fraction = 0.0;
  
  args.layerreqd = false;
  args.layerveto = false;
  args.outdirname = "Target";
  TargetSpectrum(args);

  
  // for the inclusive charged particle analysis (SiL3)
  args.treename = "siBlockTree_SiL";
  args.layername = "thick";
  args.max_energy = 25;
  args.energy_width = 0.050;
  args.time_width = 10;
  args.layerreqd = false;
  args.layerveto = false;
  args.outdirname = "SiL3";
  TargetSpectrum(args);

  /*
  // for the inclusive charged particle analysis (SiR2)
  args.treename = "siBlockTree_SiR";
  args.layername = "thick";
  args.max_energy = 50000;
  args.energy_width = 50;
  args.min_time = -20000;
  args.max_time = 20000;
  args.time_width = 10;
  args.layerreqd = false;
  args.layerveto = false;
  args.outdirname = "SiR2";
  TargetSpectrum(args);
  
  args.vetolayername = "thin";
  args.reqdlayername = "third";
  args.layerreqd = true;
  args.layerveto = true;
  //  args.layer_coincidence_time = 200;
  args.outdirname = "SiR2_Michels";
  TargetSpectrum(args);
  */
}
