void Si16b_FinalPlot_TwoLayer_EvdE_NoCuts() {

  std::string infilename = "~/data/results/Si16b/plots_newPP_geq1TgtPulse.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  const int n_settings = 4;
  std::string settings[n_settings] = {"_noLayerCoinc", "_layerCoinc", "_timecut0_10000ns_noLayerCoinc", "_timecut0_10000ns_layerCoinc"};
  std::string histtitles[n_settings] = {"No Layer Coinc or Time Cut", "with Layer Coinc Cut", "with Time Cut", "with both Layer Coinc and Time Cuts"};

  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {

    std::string setting = settings[i_setting];
    std::string dirname = "all_SiR" + setting;
    std::string canvasname = "c_" + setting;
    
    TCanvas* c_EvdE = new TCanvas(canvasname.c_str(), canvasname.c_str());
    std::string inhistname = dirname + "/hEvdE_TwoLayer_12not3";
    TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
    if (!hEvdE) {
      std::cout << "Problem getting histogram " << inhistname << std::endl;
      continue;
    }	
    std::string histtitle = "Si16b Dataset, Right Arm (" + histtitles[i_setting] + ")";
    hEvdE->SetTitle(histtitle.c_str());
    hEvdE->SetStats(false);
    //    hEvdE->Rebin2D(5, 5);
    hEvdE->GetXaxis()->SetRangeUser(0, 20000);
    hEvdE->GetYaxis()->SetRangeUser(0, 3000);
    //    hEvdE->GetYaxis()->SetRangeUser(0, 10000);
    hEvdE->Draw("COLZ");
  }
}
