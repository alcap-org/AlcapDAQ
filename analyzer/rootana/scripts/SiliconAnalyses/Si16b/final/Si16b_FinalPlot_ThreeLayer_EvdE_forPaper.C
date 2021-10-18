void Si16b_FinalPlot_ThreeLayer_EvdE_forPaper(std::string outdir = "") {

  std::string infilename = "~/data/results/Si16b/plots_newPP_geq1TgtPulse_1.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  //  std::string dirname = "alpha_SiR_timecut0_10000ns_layerCoinc";
  std::string dirname = "all_SiR_timecut0_10000ns_layerCoinc500ns";
  //  std::string dirname = "proton_SiR_timecut0_10000ns_layerCoinc_PSel";
  std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  //  std::string dirname = "all_SiR_nolayercoinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";

  std::string inhistname = dirname + "/hEvdE_ThreeLayer_123";
  //  std::string inhistname = dirname + "/hEvdE_TwoLayer_123";
  TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
  std::string histtitle = "Si16b Dataset, Right Arm (" + layer1 + " && " + layer2 + " && " + layer3 + " && SiL1)";
  hEvdE->SetTitle(histtitle.c_str());
  hEvdE->SetStats(false);
  hEvdE->GetXaxis()->SetRangeUser(0, 25);
  hEvdE->GetYaxis()->SetRangeUser(0, 20);
  hEvdE->GetXaxis()->SetTitleOffset(1.0);
  hEvdE->GetYaxis()->SetTitleOffset(1.0);
  hEvdE->GetXaxis()->SetLabelSize(0.04);
  hEvdE->GetYaxis()->SetLabelSize(0.04);


  TCanvas* c_EvdE = new TCanvas("c_EvdE", "c_EvdE");
  hEvdE->Draw("");

  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
  // latex->SetTextSize(0.04);
  // latex->SetTextColor(kRed);
  // latex->DrawLatex(100, 2000, "proton");
  // latex->SetTextColor(kCyan);
  // latex->DrawLatex(100, 2500, "deuteron");
  // latex->SetTextColor(kMagenta);
  // latex->DrawLatex(1500, 2800, "triton");
  // latex->SetTextColor(kSpring);
  // latex->DrawLatex(8000, 7500, "#alpha");

  latex->SetTextColor(kBlack);
  latex->SetTextAlign(22);
  latex->SetTextSize(0.05);
  latex->DrawLatexNDC(0.3, 0.6, "AlCap #bf{#it{Preliminary}}");
  latex->SetTextSize(0.04);
  latex->DrawLatexNDC(0.3, 0.55, "Si Analysis");
  //  hEvdE->SetDrawOption("COLZ");

  TCanvas* c_EvdEZoom = new TCanvas("c_EvdEZoom", "c_EvdEZoom");
  TH2F* hEvdEZoom = (TH2F*) hEvdE->Clone("hEvdEZoom");
  hEvdEZoom->Rebin2D(5, 5);
  hEvdEZoom->GetXaxis()->SetRangeUser(0, 20);
  hEvdEZoom->GetYaxis()->SetRangeUser(0, 10);
  hEvdEZoom->Draw("");

  // latex->SetTextSize(0.04);
  // latex->SetTextColor(kRed);
  // latex->DrawLatex(100, 2000, "proton");
  // latex->SetTextColor(kCyan);
  // latex->DrawLatex(100, 2500, "deuteron");
  // latex->SetTextColor(kMagenta);
  // latex->DrawLatex(1500, 2800, "triton");
  // latex->SetTextColor(kSpring);
  // latex->DrawLatex(8000, 7500, "#alpha");

  latex->SetTextColor(kBlack);
  latex->SetTextAlign(22);
  latex->SetTextSize(0.05);
  latex->DrawLatexNDC(0.6, 0.8, "AlCap #bf{#it{Preliminary}}");
  latex->SetTextSize(0.04);
  latex->DrawLatexNDC(0.6, 0.75, "Si Analysis");

  if (outdir != "") {
    std::string savename = outdir + "AlCapData_Si16bDataset_ThreeLayer_EvdE_forPaper";
    std::string pngname = savename + ".png";
    c_EvdE->SaveAs(pngname.c_str());
  }
}
