void Si16b_FinalPlot_UnfoldedSpectra_DifferentGeometries() {

  const int n_settings = 2;
  std::string setting_names[n_settings] = {"", "_1mmCloserSiR"};
  Int_t colours[n_settings] = {kRed, kBlue};
  std::string leglabels[n_settings] = {"nominal", "SiR 1 mm closer"};
  
  std::string infilename = "~/data/results/Si16b/unfold_newPP.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

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
    
    std::string unfolded_histname = "ResponseMatrix_proton_TCutG" + setting + "/hCorrectedSpectrum";
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
