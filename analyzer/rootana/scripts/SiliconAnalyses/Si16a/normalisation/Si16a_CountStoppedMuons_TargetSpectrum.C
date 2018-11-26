#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_TargetSpectrum.C"

void Si16a_CountStoppedMuons_TargetSpectrum(std::string infilename, std::string outfilename) {

  CountStoppedMuons_TargetSpectrumArgs args;
  args.infilename = infilename;
  args.inhistname = "Target_Thin_wNoThick/hEnergyTime";
  args.outfilename = outfilename;
  args.outdirname = "TargetSpectrum_Thin_wNoThick";
  args.min_muon_time = -200;
  args.max_muon_time = 200;
  args.min_muon_energy = 400;
  args.max_muon_energy = 600;  
  CountStoppedMuons_TargetSpectrum(args);

  args.inhistname = "Target_Thin_All/hEnergyTime";
  args.outdirname = "TargetSpectrum_Thin_All";
  CountStoppedMuons_TargetSpectrum(args);
}
