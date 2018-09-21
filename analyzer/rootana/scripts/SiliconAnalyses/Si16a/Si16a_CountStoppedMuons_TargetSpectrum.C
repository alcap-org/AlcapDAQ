#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_TargetSpectrum.C"

void Si16a_CountStoppedMuons_TargetSpectrum(std::string infilename, std::string outfilename) {

  CountStoppedMuons_TargetSpectrumArgs args;
  args.infilename = infilename;
  args.inhistname = "hTarget_woutSiL3";
  args.outfilename = outfilename;
  args.outtreename = "target_direct_veto";
  args.min_muon_time = -200;
  args.max_muon_time = 200;
  args.min_muon_energy = 400;
  args.max_muon_energy = 600;  
  CountStoppedMuons_TargetSpectrum(args);

  args.inhistname = "hTarget_All";
  args.outtreename = "target_direct_noveto";
  CountStoppedMuons_TargetSpectrum(args);
}
