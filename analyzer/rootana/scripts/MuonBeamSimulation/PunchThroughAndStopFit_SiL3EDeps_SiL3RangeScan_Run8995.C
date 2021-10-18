void PunchThroughAndStopFit_SiL3EDeps_SiL3RangeScan_Run8995() {
  gROOT->ProcessLine(".L scripts/MuonBeamSimulation/PunchThroughFit.C+");
  gROOT->ProcessLine(".L scripts/MuonBeamSimulation/RooGaussExp.cxx++");
  gROOT->ProcessLine(".L scripts/MuonBeamSimulation/PunchThroughAndStopFit.C+");

  RooWorkspace workspace("workspace");

  workspace.factory("edep[2000,7000]");
  std::string id = "data_run8995";
  PunchThroughAndStopFit punchAndStop(&workspace, id, "edep");

  punchAndStop.pdf_params.landau_mean_min = 3000;
  punchAndStop.pdf_params.landau_mean_max = 4000;
  punchAndStop.pdf_params.landau_sigma_min = 1;
  punchAndStop.pdf_params.landau_sigma_max = 1000;
  punchAndStop.pdf_params.gauss_sigma_min = 1;
  punchAndStop.pdf_params.gauss_sigma_max = 300;
  punchAndStop.pdf_params.stop_edep0_min = 5000;
  punchAndStop.pdf_params.stop_edep0_max = 6500;
  punchAndStop.pdf_params.stop_sigma_min = 0;
  punchAndStop.pdf_params.stop_sigma_max = 100;
  punchAndStop.pdf_params.stop_alpha_min = 0.1;//0.00001;
  punchAndStop.pdf_params.stop_alpha_max = 1.0;//0.5;
  punchAndStop.pdf_params.n_punch_min = 1;
  punchAndStop.pdf_params.n_punch_max = 3000000;
  punchAndStop.pdf_params.n_stop_min = 1;
  punchAndStop.pdf_params.n_stop_max = 3000000;

  punchAndStop.hist_params.rebin_factor = 100;
  punchAndStop.hist_params.histname = "SiL3Energy";
  punchAndStop.hist_params.histtitle = "SiL3 Energy (Run 8995, SF = 1.29)";
  punchAndStop.hist_params.colour = kRed;
  punchAndStop.hist_params.paramBox_x_min = 0.65;
  punchAndStop.hist_params.paramBox_x_max = 0.85;
  punchAndStop.hist_params.paramBox_y_max = 0.90;

  filename = "scripts/MuonBeamSimulation/hist_files/SiL3EDepsFromTMETree_SiL3RangeScanDataset_Run8995.root";
  histname = "hSiL3EDeps";
  punchAndStop.initialise(filename, histname);
  
  punchAndStop.performFit();
  punchAndStop.draw();
  
  workspace.Print();
}
