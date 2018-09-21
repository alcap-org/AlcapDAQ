#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_TargetSpectrum.C"

void SiL3_CountStoppedMuons_TargetSpectrum(std::string infilename, std::string outfilename) {

  CountStoppedMuons_TargetSpectrumArgs args;
  args.infilename = infilename;
  args.inhistname = "Target/hEnergyTime";
  args.outfilename = outfilename;
  args.outdirname = "TargetSpectrum";
  args.min_muon_time = -200;
  args.max_muon_time = 200;
  args.min_muon_energy = 3000;
  args.max_muon_energy = 6000;
  CountStoppedMuons_TargetSpectrum(args);
}
