#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_XRaySpectrum.C"
#include "scripts/SiliconAnalyses/Utils/CountStoppedMuons_TargetSpectrum.C"

void Si16a_CountStoppedMuons_XRaySpectrum_wTargetCoinc(std::string infilename, std::string outfilename, double min_muon_energy, double max_muon_energy) {

  std::stringstream muon_slice;
  muon_slice.str("");
  muon_slice << "MuonSlice" << (int)min_muon_energy << "_" << (int)max_muon_energy;
  
  CountStoppedMuons_XRaySpectrumArgs xray_args;
  xray_args.infilename = infilename;
  xray_args.channel = "GeLoGain";
  xray_args.inhistname = "hXRaySpectrum_wSiL1Coinc_" + xray_args.channel + "_" + muon_slice.str();
  xray_args.outfilename = outfilename;
  xray_args.outtreename = "hXRaySpectrum_count_wSiL1Coinc_" + xray_args.channel + "_" + muon_slice.str();
  xray_args.outwsname = "ws_hXRaySpectrum_count_wSiL1Coinc_" + xray_args.channel + "_" + muon_slice.str();
  xray_args.material = "Si";
  xray_args.transition = "2p-1s";
  xray_args.rebin_factor = 1;
  xray_args.ge_eff_run = 9302;
  //  xray_args.ge_eff_run = 10319;
  xray_args.min_time = -20000;
  xray_args.max_time = 20000;
  xray_args.fit_window_min = -10;
  xray_args.fit_window_max = 10;

  CountStoppedMuons_XRaySpectrum(xray_args);

  xray_args.inhistname = "hXRaySpectrum_wSiL3Coinc_" + xray_args.channel + "_" + muon_slice.str();
  xray_args.outtreename = "hXRaySpectrum_count_wSiL3Coinc_" + xray_args.channel + "_" + muon_slice.str();
  xray_args.outwsname = "ws_hXRaySpectrum_count_wSiL3Coinc_" + xray_args.channel + "_" + muon_slice.str();
  CountStoppedMuons_XRaySpectrum(xray_args);

  CountStoppedMuons_TargetSpectrumArgs tgt_args;
  tgt_args.infilename = infilename;
  tgt_args.inhistname = "hTarget_woutSiL3";
  tgt_args.outfilename = outfilename;
  tgt_args.outtreename = "hTarget_woutSiL3_count";
  tgt_args.min_muon_time = -200;
  tgt_args.max_muon_time = 200;
  tgt_args.min_muon_energy = min_muon_energy;
  tgt_args.max_muon_energy = max_muon_energy;
  CountStoppedMuons_TargetSpectrum(tgt_args);

  tgt_args.inhistname = "hThick_wSiL1";
  tgt_args.outtreename = "hThick_wSiL1_count";
  CountStoppedMuons_TargetSpectrum(tgt_args);

}
