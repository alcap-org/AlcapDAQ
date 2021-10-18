void Si16b_FinalPlot_MuonSpot_BkgEstimate() {

  const int n_both_points = 5;
  double both_x[n_both_points] = {0.825, 1.700, 2.300, 1.500, 0.825};
  double both_y[n_both_points] = {0.625, 1.500, 0.950, 0.300, 0.625};
  TCutG* both = new TCutG("both_two_layer", n_both_points, both_x, both_y);
  both->SetLineColor(kBlack);
  both->SetLineWidth(2);
  both->SetFillStyle(0);

  std::string infilename = "~/data/results/Si16b/plots_newPP_geq0TgtPulse_1.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  std::string dirname = "all_SiR_timecut0_10000ns_layerCoinc500ns";
  std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";

  std::string inhistname = dirname + "/hEvdE_TwoLayer_12not3";
  //  std::string inhistname = dirname + "/hEvdE_TwoLayer_123";
  TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
  std::string histtitle = "Si16b Dataset, Right Arm (" + layer1 + " && " + layer2 + " && !" + layer3 + " && SiL1)";
  hEvdE->SetTitle(histtitle.c_str());
  hEvdE->SetStats(false);
  hEvdE->GetXaxis()->SetRangeUser(0, 18);
  hEvdE->GetYaxis()->SetRangeUser(0, 3);
  hEvdE->GetXaxis()->SetTitleOffset(1.0);
  hEvdE->GetYaxis()->SetTitleOffset(1.0);
  hEvdE->GetXaxis()->SetLabelSize(0.04);
  hEvdE->GetYaxis()->SetLabelSize(0.04);
  hEvdE->SetXTitle("E_1 + E_2 [MeV]");
  hEvdE->SetYTitle("E_1 [MeV]");


  TCanvas* c_EvdE = new TCanvas("c_EvdE", "c_EvdE");
  hEvdE->Draw("COLZ");
  both->Draw("LSAME");

  double integral = 0;
  for (int i_bin = 1; i_bin <= hEvdE->GetXaxis()->GetNbins(); ++i_bin) {
    for (int j_bin = 1; j_bin <= hEvdE->GetYaxis()->GetNbins(); ++j_bin) {
      double x = hEvdE->GetXaxis()->GetBinCenter(i_bin);
      double y = hEvdE->GetYaxis()->GetBinCenter(j_bin);
      if (both->IsInside(x, y)) {
	integral += hEvdE->GetBinContent(i_bin,j_bin);
      }
    }
  }
  std::cout << "Integral in spots = " << integral << std::endl;
}
