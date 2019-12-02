void Si16b_FinalPlot_UnfoldedSpectra_DifferentNTgtMuonCut() {

  const int n_settings = 4;
  //  std::string setting_names[n_settings] = {"", "_geq1TgtPulse", "_geq1TgtPulse_SiL1-8", "_geq1TgtPulse_SiL1-15"};
  //  std::string setting_names[n_settings] = {"", "_geq1TgtPulse", "_geq1TgtPulse_SiL1-2--6"};
  std::string setting_names[n_settings] = {"_geq1TgtPulse", "_geq1TgtPulse_SiL1-2--6", "_geq1TgtPulse_SiL1-7--10", "_geq1TgtPulse_SiL1-11--15"};
  Int_t colours[n_settings] = {kBlack, kRed, kBlue, kMagenta};
  //  std::string leglabels[n_settings] = {"#geq0 Tgt Pulse", "#geq1 Tgt Pulse", "#geq1 Tgt Pulse (SiL1-2 -- SiL1-6 only)"};//, "#geq1 Tgt Pulse (SiL1-15 only)"};
  std::string leglabels[n_settings] = {"#geq1 Tgt Pulse", "#geq1 Tgt Pulse (SiL1-2 -- SiL1-6 only)", "#geq1 Tgt Pulse (SiL1-7 -- SiL1-10 only)", "#geq1 Tgt Pulse (SiL1-11 -- SiL1-15 only)"};//, "#geq1 Tgt Pulse (SiL1-15 only)"};
  
  int rebin_factor = 5;
  std::stringstream axistitle;
  TLegend* leg = new TLegend(0.55,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);

  TCanvas* c = new TCanvas();
  c->SetLogy();
  
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = setting_names[i_setting];
    Int_t colour = colours[i_setting];

    std::string infilename = "~/data/results/Si16b/unfold_newPP" + setting + ".root";
    TFile* infile = new TFile(infilename.c_str(), "READ");

    //    std::string unfolded_histname = "FinalNormalisation_deuteron_PSel/hNormalisedSpectrum";
    std::string unfolded_histname = "FinalNormalisation_triton_PSel_retune/hNormalisedSpectrum";
    std::string outhisttitle = "Si16b Dataset, Right Arm, " + setting;
  
    TH1F* hUnfoldedSpectrum = (TH1F*) infile->Get(unfolded_histname.c_str());

    hUnfoldedSpectrum->SetTitle("");
    hUnfoldedSpectrum->SetStats(false);
    hUnfoldedSpectrum->SetLineColor(colour);
    hUnfoldedSpectrum->SetLineWidth(2);
    hUnfoldedSpectrum->Rebin(rebin_factor);
    hUnfoldedSpectrum->GetXaxis()->SetRangeUser(0, 15000);
    axistitle.str(""); axistitle << "Unfolded Count / " << hUnfoldedSpectrum->GetXaxis()->GetBinWidth(1) << " keV";
    hUnfoldedSpectrum->SetYTitle(axistitle.str().c_str());
    hUnfoldedSpectrum->GetYaxis()->SetTitleOffset(1.3);

    leg->AddEntry(hUnfoldedSpectrum, leglabels[i_setting].c_str(), "l");
    
    hUnfoldedSpectrum->Draw("HIST E SAME");
  }
  leg->Draw();
  //  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
}
