void Si16b_FinalPlot_TwoLayer_EvdE() {

  std::string infilename = "~/data/results/Si16b/plots_newPP_geq1TgtPulse.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  //  std::string dirname = "alpha_SiR_timecut0_10000ns_layerCoinc";
  std::string dirname = "all_SiR_timecut0_10000ns_layerCoinc";
  //  std::string dirname = "proton_SiR_timecut0_10000ns_layerCoinc_PSel";
  std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  //  std::string dirname = "all_SiR_nolayercoinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";

  std::string inhistname = dirname + "/hEvdE_TwoLayer_12not3";
  //  std::string inhistname = dirname + "/hEvdE_TwoLayer_123";
  TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
  std::string histtitle = "Si16b Dataset, Right Arm (" + layer1 + " && " + layer2 + " && " + layer3 + " && SiL1)";
  hEvdE->SetTitle(histtitle.c_str());
  hEvdE->SetStats(false);
  hEvdE->GetXaxis()->SetRangeUser(0, 20000);
  hEvdE->GetYaxis()->SetRangeUser(0, 3000);
  hEvdE->GetYaxis()->SetTitleOffset(0.9);
  hEvdE->GetXaxis()->SetTitleOffset(0.9);


  TCanvas* c_EvdE = new TCanvas("c_EvdE", "c_EvdE");
  hEvdE->Draw("COLZ");

  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");

  latex->SetTextColor(kRed);
  latex->DrawLatex(1000, 1200, "#mu^{-}");
  latex->DrawLatex(1600, 2100, "p");
  latex->DrawLatex(2200, 2500, "d");
  latex->DrawLatex(2800, 2900, "t");
  latex->DrawLatex(8000, 7500, "#alpha");

  alcaphistogram(hEvdE);
  alcapPreliminary(hEvdE);
  hEvdE->SetDrawOption("COLZ");

  TCanvas* c_EvdEZoom = new TCanvas("c_EvdEZoom", "c_EvdEZoom");
  TH2F* hEvdEZoom = (TH2F*) hEvdE->Clone("hEvdEZoom");
  hEvdEZoom->Rebin2D(5, 5);
  hEvdEZoom->GetXaxis()->SetRangeUser(0, 20000);
  hEvdEZoom->GetYaxis()->SetRangeUser(0, 10000);
  hEvdEZoom->Draw("COLZ");

  latex->SetTextColor(kRed);
  latex->DrawLatex(1000, 1200, "#mu^{-}");
  latex->DrawLatex(1600, 2100, "p");
  latex->DrawLatex(2200, 2500, "d");
  latex->DrawLatex(2800, 2900, "t");
  latex->DrawLatex(8000, 7500, "#alpha");

  alcaphistogram(hEvdEZoom);
  alcapPreliminary(hEvdEZoom);
  hEvdEZoom->SetDrawOption("COLZ");
}
