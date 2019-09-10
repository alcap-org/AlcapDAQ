void Si16a_FinalPlot_TargetSpectrum_EvdE() {
  
  //  const int n_runs = 8;
  //  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743}; // want to do all runs individually
  //  double scale_factors[n_runs] = {1.035, 0.99, 0.98, 1.01, 1.02, 1.03, 1.04, 1.05};
  const int n_runs = 1;
  int run_numbers[n_runs] = {9740}; // want to do all runs individually
  double scale_factors[n_runs] = {1.02};

  std::stringstream run_str, sf_str, axislabel;
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    TCanvas* c = new TCanvas();
    c->SetLogz();
    
    run_str.str("");
    run_str << "0" << run_numbers[i_run];

    sf_str.str("");
    sf_str << scale_factors[i_run];

    std::string infilename = "~/data/results/Si16a/plots" + run_str.str() + "_newPP.root";    
    TFile* infile = new TFile(infilename.c_str(), "READ");
    if (infile->IsZombie()) {
      std::cout << "ERROR: Can't find file " << infilename << std::endl;
      return;
    }

    std::string histname = "all_Target_timecut-200_200ns_layerCoinc/hEvdE_TwoLayer_12";
    TH2F* hEvdE = (TH2F*) infile->Get(histname.c_str());
    if (!hEvdE) {
      std::cout << "ERROR: Can't find histogram " << histname << std::endl;
      return;
    }
    
    std::string histtitle = "Si16a Dataset, Target Spectrum (Run " + run_str.str() + ", SF = " + sf_str.str() + ")";
    hEvdE->SetTitle(histtitle.c_str());
    hEvdE->GetXaxis()->SetRangeUser(0,5000);
    hEvdE->GetYaxis()->SetRangeUser(0,3000);
    hEvdE->GetXaxis()->SetTitle("E_{1} + E_{2} [keV]");
    hEvdE->GetYaxis()->SetTitle("E_{1}[keV]");
    hEvdE->SetStats(false);
    
    hEvdE->Draw("COLZ");
  }
}
