void Si16b_FinalPlot_RawSpectra_SiR3Veto() {

  std::string particle = "proton";
  //  std::string particle = "deuteron";
  //  std::string particle = "triton";
  //  std::string particle = "alpha";
  
  const int n_settings = 2;
  std::string setting_names[n_settings] = {"", "_noCorr"};
  Int_t colours[n_settings] = {kRed, kBlue};
  std::string leglabels[n_settings] = {"w/ SiR3 Veto Correction", "w/o SiR3 Veto Correction"};
  
  TLegend* leg = new TLegend(0.50,0.45,0.80,0.75);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);
  
  std::stringstream axistitle;
  int rebin_factor = 50;
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = setting_names[i_setting];
    Int_t colour = colours[i_setting];

    std::string infilename = "~/data/results/Si16b/raw_spectra_newPP_geq1TgtPulse" + setting + ".root";
    TFile* infile = new TFile(infilename.c_str(), "READ");
    
    std::string folded_histname = particle + "_PSel_noTimeCut/hRawSpectrum";
    std::string outhisttitle = "Si16b Dataset, Right Arm, " + particle;
  
    TH1F* hFoldedSpectrum = (TH1F*) infile->Get(folded_histname.c_str());

    hFoldedSpectrum->Rebin(rebin_factor);
    axistitle.str("");
    axistitle << "Raw Count / " << hFoldedSpectrum->GetXaxis()->GetBinWidth(1) << " keV";
    hFoldedSpectrum->SetTitle("Si16b Dataset, Right Arm, Raw Spectrum");
    hFoldedSpectrum->SetYTitle(axistitle.str().c_str());
    hFoldedSpectrum->SetStats(false);
    hFoldedSpectrum->SetLineColor(colour);
    hFoldedSpectrum->SetLineWidth(2);
    //    hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 15000);
    
    hFoldedSpectrum->Draw("HIST E SAME");

    std::stringstream leglabel;
    leglabel.str("");
    leglabel.str("");
    leglabel << leglabels[i_setting] << " (" << hFoldedSpectrum->Integral() << " entries)";
    leg->AddEntry(hFoldedSpectrum, leglabel.str().c_str(), "l");

    double before_drop = 15000;
    double after_drop = 15500;
    int bin_before_drop = hFoldedSpectrum->GetXaxis()->FindBin(before_drop);
    int bin_after_drop = hFoldedSpectrum->GetXaxis()->FindBin(after_drop);
    std::cout << leglabel.str() << std::endl;
    std::cout << "Count before drop = " << hFoldedSpectrum->GetBinContent(bin_before_drop) << std::endl;
    std::cout << "Count after drop = " << hFoldedSpectrum->GetBinContent(bin_after_drop) << std::endl;
  }

  leg->Draw();
  
  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.80, "AlCap Preliminary");
}
