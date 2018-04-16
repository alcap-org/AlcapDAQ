#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_XRaySpectrum.C"
#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_TargetSpectrum.C"
//#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_CrossCheckSpectrum.C"

void SiL3_CountStoppedMuons_CrossCheckSpectrum(std::string infilename, std::string outfilename) {

  CountStoppedMuons_XRaySpectrumArgs xray_args;
  xray_args.infilename = infilename;
  xray_args.channel = "GeLoGain";
  xray_args.inhistname = "hCrossCheck_" + xray_args.channel;
  xray_args.outfilename = outfilename;
  xray_args.outtreename = "indirect_cross_check";
  xray_args.outwsname = "ws_cross_check";
  xray_args.material = "Si";
  xray_args.transition = "2p-1s";
  xray_args.rebin_factor = 2;
  xray_args.ge_eff_run = 9302;
  //  xray_args.ge_eff_run = 10319;
  xray_args.min_time = -200;
  xray_args.max_time = 200;
  CountStoppedMuons_XRaySpectrum(xray_args);

  CountStoppedMuons_TargetSpectrumArgs tgt_args;
  tgt_args.infilename = infilename;
  tgt_args.inhistname = "hTarget";
  tgt_args.outfilename = outfilename;
  tgt_args.outtreename = "direct_cross_check";
  tgt_args.min_muon_time = -200;
  tgt_args.max_muon_time = 200;
  tgt_args.min_muon_energy = 3900;
  tgt_args.max_muon_energy = 4000;
  
  CountStoppedMuons_TargetSpectrum(tgt_args);
}
