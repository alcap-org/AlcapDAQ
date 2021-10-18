void ProtonSpectrumFit_Si16bDataset() {
  gROOT->ProcessLine(".L scripts/Unfolding/spectrum-fit/RooProtonESpectrum.cxx++");
  gROOT->ProcessLine(".L scripts/Unfolding/spectrum-fit/ProtonSpectrumFit.C+");

  RooWorkspace workspace("workspace");
  workspace.factory("energy[1000,15000]");
  std::string id = "data_si16b";

  ProtonSpectrumFit protonSpectrum(&workspace, id, "energy");

  protonSpectrum.pdf_params.T0_min = 1000;
  protonSpectrum.pdf_params.T0_max = 2500;
  protonSpectrum.pdf_params.k_min = 0.1;
  protonSpectrum.pdf_params.k_max = 2;
  protonSpectrum.pdf_params.Q_min = 100;
  protonSpectrum.pdf_params.Q_max = 5000;
  //  protonSpectrum.pdf_params.Q2_min = 5000;
  //  protonSpectrum.pdf_params.Q2_max = 20000;
  //  protonSpectrum.pdf_params.ARatio_min = 0;
  //  protonSpectrum.pdf_params.ARatio_max = 1;


  protonSpectrum.hist_params.rebin_factor = 1;
  protonSpectrum.hist_params.histname = "ProtonEnergy";
  protonSpectrum.hist_params.histtitle = "Proton Energy (Si16b Dataset)";
  protonSpectrum.hist_params.colour = kRed;
  protonSpectrum.hist_params.paramBox_x_min = 0.65;
  protonSpectrum.hist_params.paramBox_x_max = 0.85;
  protonSpectrum.hist_params.paramBox_y_max = 0.90;

  std::string filename = "scripts/Unfolding/spectrum-fit/R15b_Si16b_UnfoldedProtonSpectrum.root";
  std::string histname = "SiR_response";
  //  std::string histname = "hEvdE_Si16b_Proton_Veto_px";
  protonSpectrum.initialise(filename, histname);
  
  protonSpectrum.performFit();
  protonSpectrum.draw();


  workspace.Print();
}
