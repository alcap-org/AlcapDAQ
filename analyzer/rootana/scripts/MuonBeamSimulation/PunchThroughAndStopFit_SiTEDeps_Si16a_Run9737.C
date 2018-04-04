void PunchThroughAndStopFit_SiTEDeps_Si16a_Run9737() {
  gROOT->ProcessLine(".L scripts/MuonBeamSimulation/RooGaussExp.cxx++");
  gROOT->ProcessLine(".L scripts/MuonBeamSimulation/PunchThroughAndStopFit.C+");

  RooWorkspace workspace("workspace");
  workspace.factory("edep[200,1200]");
  std::string id = "data_run9737";

  PunchThroughAndStopFit punchAndStop(&workspace, id, "edep");

  punchAndStop.pdf_params.landau_mean_min = 200;
  punchAndStop.pdf_params.landau_mean_max = 600;
  punchAndStop.pdf_params.landau_sigma_min = 0;
  punchAndStop.pdf_params.landau_sigma_max = 100;
  punchAndStop.pdf_params.gauss_sigma_min = 0;
  punchAndStop.pdf_params.gauss_sigma_max = 500;
  punchAndStop.pdf_params.stop_edep0_min = 800;
  punchAndStop.pdf_params.stop_edep0_max = 1000;
  punchAndStop.pdf_params.stop_sigma_min = 0;
  punchAndStop.pdf_params.stop_sigma_max = 300;
  punchAndStop.pdf_params.stop_alpha_min = 0;
  punchAndStop.pdf_params.stop_alpha_max = 0.5;
  punchAndStop.pdf_params.n_punch_min = 1;
  punchAndStop.pdf_params.n_punch_max = 3000000;
  punchAndStop.pdf_params.n_stop_min = 1;
  punchAndStop.pdf_params.n_stop_max = 3000000;

  punchAndStop.hist_params.rebin_factor = 20;
  punchAndStop.hist_params.histname = "SiTEnergy";
  punchAndStop.hist_params.histtitle = "SiT Energy (Run 9737, SF = 0.98)";
  punchAndStop.hist_params.colour = kRed;
  punchAndStop.hist_params.paramBox_x_min = 0.65;
  punchAndStop.hist_params.paramBox_x_max = 0.85;
  punchAndStop.hist_params.paramBox_y_max = 0.90;

  std::string filename = "scripts/MuonBeamSimulation/hist_files/SiTEDepsFromTMETree_Si16aDataset_Run9737.root";
  std::string histname = "hCentralMuonEnergies";
  punchAndStop.initialise(filename, histname);
  
  punchAndStop.performFit();
  punchAndStop.draw();


  workspace.Print();
}
