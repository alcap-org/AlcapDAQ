void Si16b_FinalPlot_UnfoldedSpectra_DifferentMuonBeams() {

  //  const int n_settings = 5;
  //  std::string setting_names[n_settings] = {"_lowestE", "_lowerE", "", "_higherE", "_highestE"};
  //  Int_t colours[n_settings] = {kRed, kMagenta, kBlack, kCyan, kBlue};
  //  std::string leglabels[n_settings] = {"Ekin = 1.19 MeV", "Ekin = 1.34 MeV", "EKin = 1.37 MeV", "EKin = 1.40 MeV", "EKin = 1.55 MeV"};

  const int n_settings = 2;
  std::string setting_names[n_settings] = {"", "_retune"};
  Int_t colours[n_settings] = {kBlack, kRed};
  std::string leglabels[n_settings] = {"original", "retuned"};

  std::string infilename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  int rebin_factor = 5;
  std::stringstream axistitle;
  TLegend* leg = new TLegend(0.15,0.55,0.40,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);

  TCanvas* c = new TCanvas();
  c->SetLogy();
  
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = setting_names[i_setting];
    Int_t colour = colours[i_setting];
    
    std::string unfolded_histname = "ResponseMatrix_triton_TCutG" + setting + "/hCorrectedSpectrum";
    std::string outhisttitle = "Si16b Dataset, Right Arm, " + setting;
  
    TH1F* hUnfoldedSpectrum = (TH1F*) infile->Get(unfolded_histname.c_str());
    if (!hUnfoldedSpectrum) {
      std::cout << "Can't find " << unfolded_histname << std::endl;
      continue;
    }

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
  //  leg->Draw();
  //  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
}
