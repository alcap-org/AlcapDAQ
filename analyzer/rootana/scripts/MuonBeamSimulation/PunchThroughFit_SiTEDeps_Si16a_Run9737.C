void PunchThroughFit_SiTEDeps_Si16a_Run9737() {
  gROOT->ProcessLine(".L scripts/MuonBeamSimulation/PunchThroughFit.C+");

  RooWorkspace workspace("workspace");
  workspace.factory("edep[200,700]");
  std::string id = "data_run9737";

  PunchThroughFit punch(&workspace, id);

  punch.pdf_params.landau_mean_min = 200;
  punch.pdf_params.landau_mean_max = 500;
  punch.pdf_params.landau_sigma_min = 0;
  punch.pdf_params.landau_sigma_max = 100;
  punch.pdf_params.gauss_sigma_min = 0;
  punch.pdf_params.gauss_sigma_max = 500;

  punch.hist_params.rebin_factor = 20;
  punch.hist_params.histname = "SiTEnergy";
  punch.hist_params.histtitle = "SiT Energy (Run 9737, SF = 0.98)";
  punch.hist_params.colour = kRed;
  punch.hist_params.paramBox_x_min = 0.40;
  punch.hist_params.paramBox_x_max = 0.75;
  punch.hist_params.paramBox_y_max = 0.4;

  std::string filename = "scripts/MuonBeamSimulation/hist_files/SiTEDepsFromTMETree_Si16aDataset_Run9737.root";
  std::string histname = "hCentralMuonEnergies";
  punch.initialise(filename, histname);
  
  punch.performFit();
  punch.draw();


  workspace.Print();
}
