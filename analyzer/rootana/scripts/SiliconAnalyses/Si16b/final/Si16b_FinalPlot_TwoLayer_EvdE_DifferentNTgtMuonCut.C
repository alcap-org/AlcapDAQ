void Si16b_FinalPlot_TwoLayer_EvdE_DifferentNTgtMuonCut() {

  /*
  const int n_settings = 5;
  std::string settings[n_settings] = {"", "_geq1TgtPulse", "_geq1TgtPulse_SiL1-2--6", "_geq1TgtPulse_SiL1-7--10", "_geq1TgtPulse_SiL1-11--15"};
  std::string leglabels[n_settings] = {"#geq0 Tgt Pulse", "#geq1 Tgt Pulse", "#geq1 Tgt Pulse (SiL1-2 -- SiL1-6)", "#geq1 Tgt Pulse (SiL1-7 -- SiL1-10)", "#geq1 Tgt Pulse (SiL1-11 -- SiL1-15)"};
  Int_t colours[n_settings] = {kRed, kBlue, kMagenta, kCyan, kSpring};
  */

  const int n_settings = 2;
  std::string settings[n_settings] = {"_geq1TgtPulse", "_0TgtPulse"};
  std::string histtitles[n_settings] = {"#geq1 Tgt Pulse", "0 Tgt Pulse"};
  std::string leglabels[n_settings] = {"#geq1 Tgt Pulse", "#splitline{0 Tgt Pulse}{NB some inactive strips}"};
  Int_t colours[n_settings] = {kRed, kBlue};

  int rebin_factor = 50;
  std::stringstream axistitle;

  TLegend* leg = new TLegend(0.50,0.45,0.80,0.75);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = settings[i_setting];

    std::string canvasname = "c_" + setting;
    TCanvas* c_Spectra = new TCanvas(canvasname.c_str(), canvasname.c_str());
      
    std::string infilename = "~/data/results/Si16b/plots_newPP" + setting + ".root";
    TFile* infile = new TFile(infilename.c_str(), "READ");
    if (infile->IsZombie()) {
      std::cout << "Can't open file " << infilename << std::endl;
      continue;
    }

    std::string histname = "all_SiR_timecut0_10000ns_layerCoinc/hEvdE_TwoLayer_12not3";
    TH2F* hEvdE = (TH2F*) infile->Get(histname.c_str());
    if (!hEvdE) {
      std::cout << "Can't find histogram " << histname << std::endl;
      continue;
    }

    //  hEvdE->SetTitle(outhisttitle.c_str());
    //    hEvdE->Rebin(rebin_factor);
    hEvdE->SetStats(false);
    //    hEvdE->SetLineColor(colours[i_setting]);
    //    hEvdE->SetLineWidth(2);
    //    hEvdE->GetXaxis()->SetRangeUser(0, 25000);
    hEvdE->GetYaxis()->SetRangeUser(0, 3000);
    std::string histtitle = "Si16b Dataset, Right Arm (" + histtitles[i_setting] + ")";
    hEvdE->SetTitle(histtitle.c_str());
  
    hEvdE->Draw("COLZ");
  }
  //  leg->Draw();
}
