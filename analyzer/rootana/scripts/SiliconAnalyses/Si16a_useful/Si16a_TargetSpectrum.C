#include "scripts/SiliconAnalyses/Utils/TargetSpectrum.C"

void Si16a_TargetSpectrum(std::string infilename, std::string outfilename) {

  TargetSpectrumArgs args;
  args.infilename = infilename;
  args.treename = "siBlockTree_Target";
  args.layername = "thin";
  args.outfilename = outfilename;
  args.min_energy = 0;
  args.max_energy = 5000;
  args.energy_width = 50;
  args.min_time = -30000;
  args.max_time = 30000;
  args.time_width = 100;
  args.layerreqd = false;

  args.layerveto = false;
  args.outhistname = "hTarget_All";
  TargetSpectrum(args);

  args.layerveto = true;
  args.vetolayername = "thick";
  args.outhistname = "hTarget_woutSiL3";
  TargetSpectrum(args);

  args.layerveto = false;
  args.layerreqd = true;
  args.reqdlayername = "thick";
  args.outhistname = "hTarget_wSiL3";
  TargetSpectrum(args);

  args.layerveto = false;
  args.layerreqd = false;
  args.layername = "thick";
  args.outhistname = "hThick_All";
  TargetSpectrum(args);

  args.layerveto = true;
  args.vetolayername = "thin";
  args.outhistname = "hThick_woutSiL1";
  TargetSpectrum(args);

  args.layerveto = false;
  args.layerreqd = true;
  args.reqdlayername = "thin";
  args.outhistname = "hThick_wSiL1";
  TargetSpectrum(args);
}
