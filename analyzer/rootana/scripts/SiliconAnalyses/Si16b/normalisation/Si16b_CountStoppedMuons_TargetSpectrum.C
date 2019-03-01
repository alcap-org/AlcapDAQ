#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_TargetSpectrum.C"

void Si16b_CountStoppedMuons_TargetSpectrum(std::string infilename, std::string outfilename) {

  CountStoppedMuons_TargetSpectrumArgs args;
  args.infilename = infilename;
  args.inhistname = "Target/hEnergyTime";
  args.outfilename = outfilename;
  args.outdirname = "TargetSpectrum";
  args.min_muon_time = -200;
  args.max_muon_time = 200;
  args.min_muon_energy = 500;
  args.max_muon_energy = 1500;  
  CountStoppedMuons_TargetSpectrum(args);
}
