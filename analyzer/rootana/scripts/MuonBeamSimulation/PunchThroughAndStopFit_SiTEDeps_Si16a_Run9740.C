void PunchThroughAndStopFit_SiTEDeps_Si16a_Run9740() {
  gROOT->ProcessLine(".L scripts/MuonBeamSimulation/PunchThroughFit.C+");
  gROOT->ProcessLine(".L scripts/MuonBeamSimulation/RooGaussExp.cxx++");
  gROOT->ProcessLine(".L scripts/MuonBeamSimulation/PunchThroughAndStopFit.C+");

  RooWorkspace workspace("workspace");

  workspace.factory("edep[700,1000]");
  std::string id = "data_wNoSiL1Hit_punchOnly_run9740";

  PunchThroughFit punch(&workspace, id, "edep");

  punch.pdf_params.landau_mean_min = 200;
  punch.pdf_params.landau_mean_max = 600;
  punch.pdf_params.landau_sigma_min = 0;
  punch.pdf_params.landau_sigma_max = 100;
  punch.pdf_params.gauss_sigma_min = 0;
  punch.pdf_params.gauss_sigma_max = 500;

  punch.hist_params.rebin_factor = 20;
  punch.hist_params.histname = "SiTEnergy";
  punch.hist_params.histtitle = "SiT Energy (wNoSiL1Hit, Run 9740, SF = 1.02)";
  punch.hist_params.colour = kRed;
  punch.hist_params.paramBox_x_min = 0.65;
  punch.hist_params.paramBox_x_max = 0.85;
  punch.hist_params.paramBox_y_max = 0.90;

  std::string filename = "scripts/MuonBeamSimulation/hist_files/SiTEDepsFromTMETree_Si16aDataset_Run9740.root";
  std::string histname = "hCentralMuonEnergies_wNoSiL1Hit";
  punch.initialise(filename, histname);
  
  punch.performFit();
  punch.draw();
  workspace.Print();

  
  workspace.factory("edep2[700,1000]");
  id = "data_wNoSiL1Hit_run9740";
  PunchThroughAndStopFit punchAndStop(&workspace, id, "edep2");

  punchAndStop.pdf_params.landau_mean_min = 200;//punch.getPunchLandauMeanValue()-25;
  punchAndStop.pdf_params.landau_mean_max = 600;//punch.getPunchLandauMeanValue()+25;
  punchAndStop.pdf_params.landau_sigma_min = 0;//punch.getPunchLandauSigmaValue()-5;
  punchAndStop.pdf_params.landau_sigma_max = 100;//punch.getPunchLandauSigmaValue()+5;
  punchAndStop.pdf_params.gauss_sigma_min = 0;//punch.getPunchGaussSigmaValue()-10;
  punchAndStop.pdf_params.gauss_sigma_max = 500;//punch.getPunchGaussSigmaValue()+10;
  punchAndStop.pdf_params.stop_edep0_min = 850;
  punchAndStop.pdf_params.stop_edep0_max = 1000;
  punchAndStop.pdf_params.stop_sigma_min = 1;
  punchAndStop.pdf_params.stop_sigma_max = 300;
  punchAndStop.pdf_params.stop_alpha_min = 0.00001;
  punchAndStop.pdf_params.stop_alpha_max = 0.5;
  punchAndStop.pdf_params.n_punch_min = 1;
  punchAndStop.pdf_params.n_punch_max = 3000000;
  punchAndStop.pdf_params.n_stop_min = 1;
  punchAndStop.pdf_params.n_stop_max = 3000000;

  punchAndStop.hist_params.rebin_factor = 20;
  punchAndStop.hist_params.histname = "SiTEnergy";
  punchAndStop.hist_params.histtitle = "SiT Energy (wNoSiL1Hit, Run 9740, SF = 1.02)";
  punchAndStop.hist_params.colour = kRed;
  punchAndStop.hist_params.paramBox_x_min = 0.65;
  punchAndStop.hist_params.paramBox_x_max = 0.85;
  punchAndStop.hist_params.paramBox_y_max = 0.90;

  filename = "scripts/MuonBeamSimulation/hist_files/SiTEDepsFromTMETree_Si16aDataset_Run9740.root";
  histname = "hCentralMuonEnergies_wNoSiL1Hit";
  punchAndStop.initialise(filename, histname);
  
  punchAndStop.performFit();
  punchAndStop.draw();



  workspace.factory("edep3[200,1000]");
  id = "data_run9740";
  PunchThroughAndStopFit punchAndStopFull(&workspace, id, "edep3");

  punchAndStopFull.pdf_params.landau_mean_min = 200;
  punchAndStopFull.pdf_params.landau_mean_max = 600;
  punchAndStopFull.pdf_params.landau_sigma_min = punchAndStop.getPunchLandauSigmaValue()-5;
  punchAndStopFull.pdf_params.landau_sigma_max = punchAndStop.getPunchLandauSigmaValue()+5;
  punchAndStopFull.pdf_params.gauss_sigma_min = punchAndStop.getPunchLandauSigmaValue()-50;
  punchAndStopFull.pdf_params.gauss_sigma_max = punchAndStop.getPunchLandauSigmaValue()+50;
  punchAndStopFull.pdf_params.stop_edep0_min = punchAndStop.getStopGaussExpEDep0Value()-10;
  punchAndStopFull.pdf_params.stop_edep0_max = punchAndStop.getStopGaussExpEDep0Value()+10;
  punchAndStopFull.pdf_params.stop_sigma_min = punchAndStop.getStopGaussExpSigmaValue()-10;
  punchAndStopFull.pdf_params.stop_sigma_max = punchAndStop.getStopGaussExpSigmaValue()+10;
  punchAndStopFull.pdf_params.stop_alpha_min = punchAndStop.getStopGaussExpAlphaValue()-0.2;
  punchAndStopFull.pdf_params.stop_alpha_max = punchAndStop.getStopGaussExpAlphaValue()+0.2;
  punchAndStopFull.pdf_params.n_punch_min = punchAndStop.getNPunchValue()-1000;
  punchAndStopFull.pdf_params.n_punch_max = 3000000;
  punchAndStopFull.pdf_params.n_stop_min = punchAndStop.getNStopValue()-1000;
  punchAndStopFull.pdf_params.n_stop_max = 3000000;

  punchAndStopFull.hist_params.rebin_factor = 20;
  punchAndStopFull.hist_params.histname = "SiTEnergy";
  punchAndStopFull.hist_params.histtitle = "SiT Energy (Run 9740, SF = 1.02)";
  punchAndStopFull.hist_params.colour = kRed;
  punchAndStopFull.hist_params.paramBox_x_min = 0.65;
  punchAndStopFull.hist_params.paramBox_x_max = 0.85;
  punchAndStopFull.hist_params.paramBox_y_max = 0.90;

  filename = "scripts/MuonBeamSimulation/hist_files/SiTEDepsFromTMETree_Si16aDataset_Run9740.root";
  histname = "hCentralMuonEnergies_wNoSiL1Hit";
  punchAndStopFull.initialise(filename, histname);
  
  punchAndStopFull.performFit();
  punchAndStopFull.draw();
  
  workspace.Print();
}
