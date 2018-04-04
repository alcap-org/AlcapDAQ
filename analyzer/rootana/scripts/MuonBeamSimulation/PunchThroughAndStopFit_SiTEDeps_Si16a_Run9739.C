void PunchThroughAndStopFit_SiTEDeps_Si16a_Run9739() {
  gROOT->ProcessLine(".L scripts/MuonBeamSimulation/RooGaussExp.cxx++");
  gROOT->ProcessLine(".L scripts/MuonBeamSimulation/PunchThroughAndStopFit.C+");

  RooWorkspace workspace("workspace");
  workspace.factory("edep[200,1000]");
  std::string id = "data_wNoSiL1Hit_run9739";

  PunchThroughAndStopFit punchAndStop_wNoSiL1Hit(&workspace, id, "edep");

  punchAndStop_wNoSiL1Hit.pdf_params.landau_mean_min = 200;
  punchAndStop_wNoSiL1Hit.pdf_params.landau_mean_max = 600;
  punchAndStop_wNoSiL1Hit.pdf_params.landau_sigma_min = 0;
  punchAndStop_wNoSiL1Hit.pdf_params.landau_sigma_max = 100;
  punchAndStop_wNoSiL1Hit.pdf_params.gauss_sigma_min = 0;
  punchAndStop_wNoSiL1Hit.pdf_params.gauss_sigma_max = 500;
  punchAndStop_wNoSiL1Hit.pdf_params.stop_edep0_min = 800;
  punchAndStop_wNoSiL1Hit.pdf_params.stop_edep0_max = 1000;
  punchAndStop_wNoSiL1Hit.pdf_params.stop_sigma_min = 0;
  punchAndStop_wNoSiL1Hit.pdf_params.stop_sigma_max = 300;
  punchAndStop_wNoSiL1Hit.pdf_params.stop_alpha_min = 0;
  punchAndStop_wNoSiL1Hit.pdf_params.stop_alpha_max = 1;
  punchAndStop_wNoSiL1Hit.pdf_params.n_punch_min = 0;
  punchAndStop_wNoSiL1Hit.pdf_params.n_punch_max = 3000000;
  punchAndStop_wNoSiL1Hit.pdf_params.n_stop_min = 0;
  punchAndStop_wNoSiL1Hit.pdf_params.n_stop_max = 3000000;

  punchAndStop_wNoSiL1Hit.hist_params.rebin_factor = 20;
  punchAndStop_wNoSiL1Hit.hist_params.histname = "SiTEnergy";
  punchAndStop_wNoSiL1Hit.hist_params.histtitle = "SiT Energy (wNoSiL1Hit, Run 9739, SF = 1.01)";
  punchAndStop_wNoSiL1Hit.hist_params.colour = kRed;
  punchAndStop_wNoSiL1Hit.hist_params.paramBox_x_min = 0.65;
  punchAndStop_wNoSiL1Hit.hist_params.paramBox_x_max = 0.85;
  punchAndStop_wNoSiL1Hit.hist_params.paramBox_y_max = 0.90;

  std::string filename = "scripts/MuonBeamSimulation/hist_files/SiTEDepsFromTMETree_Si16aDataset_Run9739.root";
  std::string histname = "hCentralMuonEnergies_wNoSiL1Hit";
  punchAndStop_wNoSiL1Hit.initialise(filename, histname);
  
  punchAndStop_wNoSiL1Hit.performFit();
  punchAndStop_wNoSiL1Hit.draw();

  
  std::string id = "data_run9739";
  PunchThroughAndStopFit punchAndStop(&workspace, id, "edep");

  punchAndStop.pdf_params.landau_mean_min = punchAndStop_wNoSiL1Hit.getPunchLandauMeanValue()-50;
  punchAndStop.pdf_params.landau_mean_max = punchAndStop_wNoSiL1Hit.getPunchLandauMeanValue()+50;
  punchAndStop.pdf_params.landau_sigma_min = punchAndStop_wNoSiL1Hit.getPunchLandauSigmaValue()-10;
  punchAndStop.pdf_params.landau_sigma_max = punchAndStop_wNoSiL1Hit.getPunchLandauSigmaValue()+10;
  punchAndStop.pdf_params.gauss_sigma_min = punchAndStop_wNoSiL1Hit.getPunchGaussSigmaValue()-20;
  punchAndStop.pdf_params.gauss_sigma_max = punchAndStop_wNoSiL1Hit.getPunchGaussSigmaValue()+20;
  punchAndStop.pdf_params.stop_edep0_min = punchAndStop_wNoSiL1Hit.getStopGaussExpEDep0Value()-5;
  punchAndStop.pdf_params.stop_edep0_max = punchAndStop_wNoSiL1Hit.getStopGaussExpEDep0Value()+5;
  punchAndStop.pdf_params.stop_sigma_min = punchAndStop_wNoSiL1Hit.getStopGaussExpSigmaValue()-1;
  punchAndStop.pdf_params.stop_sigma_max = punchAndStop_wNoSiL1Hit.getStopGaussExpSigmaValue()+1;
  punchAndStop.pdf_params.stop_alpha_min = punchAndStop_wNoSiL1Hit.getStopGaussExpAlphaValue()-0.2;
  punchAndStop.pdf_params.stop_alpha_max = punchAndStop_wNoSiL1Hit.getStopGaussExpAlphaValue()+0.2;
  punchAndStop.pdf_params.n_punch_min = punchAndStop_wNoSiL1Hit.getNPunchValue()-1000;
  punchAndStop.pdf_params.n_punch_max = 3000000;
  punchAndStop.pdf_params.n_stop_min = punchAndStop_wNoSiL1Hit.getNStopValue()-1000;
  punchAndStop.pdf_params.n_stop_max = 3000000;

  punchAndStop.hist_params.rebin_factor = 20;
  punchAndStop.hist_params.histname = "SiTEnergy";
  punchAndStop.hist_params.histtitle = "SiT Energy (Run 9739, SF = 1.01)";
  punchAndStop.hist_params.colour = kRed;
  punchAndStop.hist_params.paramBox_x_min = 0.65;
  punchAndStop.hist_params.paramBox_x_max = 0.85;
  punchAndStop.hist_params.paramBox_y_max = 0.90;

  std::string filename = "scripts/MuonBeamSimulation/hist_files/SiTEDepsFromTMETree_Si16aDataset_Run9739.root";
  std::string histname = "hCentralMuonEnergies";
  punchAndStop.initialise(filename, histname);

  //  punchAndStop.setParConstant(punchAndStop.punchLandauMeanName);
  //  punchAndStop.setParConstant(punchAndStop.punchLandauSigmaName);
  //  punchAndStop.setParConstant(punchAndStop.punchGaussSigmaName);
  //  punchAndStop.setParConstant(punchAndStop.stopGaussExpEDep0Name);
  //  punchAndStop.setParConstant(punchAndStop.stopGaussExpSigmaName);
  //  punchAndStop.setParConstant(punchAndStop.stopGaussExpAlphaName);
  
  punchAndStop.performFit();
  punchAndStop.draw();


  workspace.Print();
}
