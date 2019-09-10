#include "scripts/SiliconAnalyses/Utils/TargetSpectrum.C"

void Si16a_TargetSpectrum(std::string infilename, std::string outfilename) {

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

  args.layer_coincidence_time = 200;
  args.layerveto = true;
  args.vetolayername = "thick";
  args.outdirname = "Target_Thin_wNoThick";
  TargetSpectrum(args);

  args.layerveto = false;
  args.layerreqd = true;
  args.reqdlayername = "thick";
  args.outdirname = "Target_Thin_wThick";
  TargetSpectrum(args);

  args.layerveto = false;
  args.layerreqd = false;
  args.layername = "thick";
  args.outdirname = "Target_Thick_All";
  TargetSpectrum(args);

  args.layerveto = true;
  args.vetolayername = "thin";
  args.outdirname = "Target_Thick_wNoThin";
  TargetSpectrum(args);

  args.layerveto = false;
  args.layerreqd = true;
  args.reqdlayername = "thin";
  args.outdirname = "Target_Thick_wThin";
  TargetSpectrum(args);

  args.layerveto = true;
  args.layerreqd = false;
  args.layername = "thick";
  args.vetolayername = "thin";
  args.layer_coincidence_time = 1000;
  args.outdirname = "Target_Thick_wNoThin_forDecayProds";
  args.min_energy = 0;
  args.max_energy = 30000;
  args.energy_width = 100;
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
