#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_XRaySpectrum.C"
#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_TargetSpectrum.C"
//#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_CrossCheckSpectrum.C"

void Si16a_CountStoppedMuons_CrossCheckSpectrum(std::string infilename, std::string outfilename, double min_muon_energy, double max_muon_energy) {

  CountStoppedMuons_XRaySpectrumArgs xray_args;
  xray_args.infilename = infilename;
  xray_args.channel = "GeLoGain";
  xray_args.inhistname = "hCrossCheck_" + xray_args.channel;
  xray_args.outfilename = outfilename;
  xray_args.outtreename = "cross_check_indirect";
  xray_args.outwsname = "ws_cross_check";
  xray_args.material = "Si";
  xray_args.transition = "2p-1s";
  xray_args.rebin_factor = 3;
  xray_args.ge_eff_run = 9302;
  xray_args.min_time = -20000;
  xray_args.max_time = 20000;
  CountStoppedMuons_XRaySpectrum(xray_args);

  CountStoppedMuons_TargetSpectrumArgs tgt_args;
  tgt_args.infilename = infilename;
  tgt_args.inhistname = "hTarget_woutSiL3";
  tgt_args.outfilename = outfilename;
  tgt_args.outtreename = "cross_check_direct";
  tgt_args.min_muon_time = -200;
  tgt_args.max_muon_time = 200;
  tgt_args.min_muon_energy = min_muon_energy;
  tgt_args.max_muon_energy = max_muon_energy;
  
  CountStoppedMuons_TargetSpectrum(tgt_args);
}
