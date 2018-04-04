void PunchThroughAndStopFit_SiTEDeps_Si16a_Run9743() {
  gROOT->ProcessLine(".L scripts/MuonBeamSimulation/RooGaussExp.cxx++");
  gROOT->ProcessLine(".L scripts/MuonBeamSimulation/PunchThroughAndStopFit.C+");

  RooWorkspace workspace("workspace");
  workspace.factory("edep[200,1200]");
  std::string id = "data_run9743";

  PunchThroughAndStopFit punchAndStop(&workspace, id);

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
  punchAndStop.pdf_params.stop_alpha_max = 1;
  punchAndStop.pdf_params.n_punch_min = 0;
  punchAndStop.pdf_params.n_punch_max = 3000000;
  punchAndStop.pdf_params.n_stop_min = 0;
  punchAndStop.pdf_params.n_stop_max = 3000000;

  punchAndStop.hist_params.rebin_factor = 20;
  punchAndStop.hist_params.histname = "SiTEnergy";
  punchAndStop.hist_params.histtitle = "SiT Energy (Run 9743, SF = 1.05)";
  punchAndStop.hist_params.colour = kRed;
  punchAndStop.hist_params.paramBox_x_min = 0.65;
  punchAndStop.hist_params.paramBox_x_max = 0.85;
  punchAndStop.hist_params.paramBox_y_max = 0.90;

  std::string filename = "scripts/MuonBeamSimulation/hist_files/SiTEDepsFromTMETree_Si16aDataset_Run9743.root";
  std::string histname = "hCentralMuonEnergies";
  punchAndStop.initialise(filename, histname);
  
  punchAndStop.performFit();
  punchAndStop.draw();


  workspace.Print();
}
