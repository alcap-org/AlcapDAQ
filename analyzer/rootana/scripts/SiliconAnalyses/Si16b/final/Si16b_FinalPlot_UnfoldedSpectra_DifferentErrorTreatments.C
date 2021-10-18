void Si16b_FinalPlot_UnfoldedSpectra_DifferentErrorTreatments() {

  const int n_settings = 3;
  std::string setting_names[n_settings] = {"", "_covError", "_covToyError"};
  //  std::string setting_names[n_settings] = {"", "_covError", "_incSyst"};
  Int_t colours[n_settings] = {kRed, kBlue, kBlack};
  std::string leglabels[n_settings] = {"default", "kCovariance", "kCovError"};

  int rebin_factor = 1;
  std::stringstream axistitle;
  TLegend* leg = new TLegend(0.55,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);

  TCanvas* c = new TCanvas();
  //  c->SetLogy();

  
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = setting_names[i_setting];
    Int_t colour = colours[i_setting];

    std::string infilename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse" + setting + "_1.root";
    TFile* infile = new TFile(infilename.c_str(), "READ");

    
    std::string unfolded_histname = "ResponseMatrix_proton_TCutG_2sig_layerCoinc500ns_tGT0ns/hCorrectedSpectrum";
    std::string outhisttitle = "Si16b Dataset, Right Arm, Unfolded Protons";
  
    TH1F* hUnfoldedSpectrum = (TH1F*) infile->Get(unfolded_histname.c_str());

    hUnfoldedSpectrum->SetTitle("");
    hUnfoldedSpectrum->SetStats(false);
    hUnfoldedSpectrum->SetLineColor(colour);
    hUnfoldedSpectrum->SetLineWidth(2);
    if (i_setting == 0) {
      hUnfoldedSpectrum->SetLineWidth(4);
      hUnfoldedSpectrum->SetFillColor(colour);
      hUnfoldedSpectrum->SetFillStyle(3004);
    }
    hUnfoldedSpectrum->Rebin(rebin_factor);
    hUnfoldedSpectrum->GetXaxis()->SetRangeUser(0, 15);
    axistitle.str(""); axistitle << "Unfolded Count / " << hUnfoldedSpectrum->GetXaxis()->GetBinWidth(1) << " keV";
    hUnfoldedSpectrum->SetYTitle(axistitle.str().c_str());
    hUnfoldedSpectrum->GetYaxis()->SetTitleOffset(1.3);

    leg->AddEntry(hUnfoldedSpectrum, leglabels[i_setting].c_str(), "lf");
    
    hUnfoldedSpectrum->Draw("HIST E SAME");
  }
  leg->Draw();
  //  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
}
