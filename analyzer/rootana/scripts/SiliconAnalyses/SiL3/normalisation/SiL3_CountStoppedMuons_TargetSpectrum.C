#include "../../Utils/CountStoppedMuons_TargetSpectrum.C"

void SiL3_CountStoppedMuons_TargetSpectrum(std::string infilename, std::string outfilename) {

  CountStoppedMuons_TargetSpectrumArgs args;
  args.infilename = infilename;
  args.inhistname = "Target/hEnergyTime";
  args.outfilename = outfilename;
  args.outdirname = "TargetSpectrum";
  args.min_muon_time = -300;
  args.max_muon_time = 300;
  args.min_muon_energy = 0;//3000;
  args.max_muon_energy = 30000;//6000;
  CountStoppedMuons_TargetSpectrum(args);
}
