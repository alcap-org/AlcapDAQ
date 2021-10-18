void Si16b_FinalPlot_RawSpectra_SiR3Veto(std::string savedir = "") {

  std::string particle = "proton";
  //  std::string particle = "deuteron";
  //  std::string particle = "triton";
  //  std::string particle = "alpha";
  
  //  const int n_settings = 2;
  //  std::string setting_names[n_settings] = {"_3sigma", "_noCorr"};
  //  std::string leglabels[n_settings] = {"w/ SiR3 Veto Correction", "w/o SiR3 Veto Correction"};
  //  Int_t colours[n_settings] = {kRed, kBlue};

  const int n_settings = 4;
  Int_t colours[n_settings] = {kRed, kBlue, kBlack, kMagenta};
  //  std::string setting_names[n_settings] = {"_JohnVetoEff", "", "_PerfVetoEff", "_RealVetoEff"};
  //  std::string leglabels[n_settings] = {"John's Veto Efficiency (0.81)", "My Veto Efficiency (0.71)", "Perfect Efficiency (1.00)", "Real Veto EFficienty (0.7 #pm 0.1)"};

  std::string setting_names[n_settings] = {"_PrevVetoEff", "_PerfVetoEff", "_RealVetoEff", "_RealVetoEff2"};
  std::string leglabels[n_settings] = {"Previous Veto Efficiency (0.81(2))", "Perfect Efficiency (1.00(0))", "Real Veto Efficiency (0.74(5))", "Real Veto Efficiency (0.74(10))"};

  TLegend* leg = new TLegend(0.50,0.60,0.80,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  TCanvas* c1 = new TCanvas();
  std::stringstream axistitle;
  int rebin_factor = 50;
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = setting_names[i_setting];
    Int_t colour = colours[i_setting];

    //    std::string infilename = "~/data/results/Si16b/raw_spectra_newPP_geq1TgtPulse" + setting + ".root";
    std::string infilename = "~/data/results/Si16b/raw_spectra_newPP_geq1TgtPulse" + setting + "_1.root";
    TFile* infile = new TFile(infilename.c_str(), "READ");
    std::cout << infilename << std::endl;
    //    std::string folded_histname = particle + "_TCutG_noTimeCut/hRawSpectrum";
    std::string folded_histname = particle + "_TCutG_2sig_layerCoinc500ns_tGT0ns/hRawSpectrum";
    std::cout << folded_histname << std::endl;
    std::string outhisttitle = "Si16b Dataset, Right Arm, " + particle;
  
    TH1F* hFoldedSpectrum = (TH1F*) infile->Get(folded_histname.c_str());

    hFoldedSpectrum->Rebin(rebin_factor);
    axistitle.str("");
    axistitle << "Raw Count / " << hFoldedSpectrum->GetXaxis()->GetBinWidth(1) << " MeV";
    hFoldedSpectrum->SetTitle("Si16b Dataset, Right Arm, Raw Spectrum");
    hFoldedSpectrum->SetXTitle("Energy [MeV]");
    hFoldedSpectrum->SetYTitle(axistitle.str().c_str());
    hFoldedSpectrum->SetStats(false);
    hFoldedSpectrum->SetLineColor(colour);
    hFoldedSpectrum->SetLineWidth(2);
    //    hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 20000);
    hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 20);
    hFoldedSpectrum->GetXaxis()->SetTitleOffset(0.9);
    hFoldedSpectrum->GetYaxis()->SetTitleOffset(0.9);
    
    hFoldedSpectrum->Draw("HIST E1 SAME");

    std::stringstream leglabel;
    leglabel.str("");
    leglabel.str("");
    leglabel << leglabels[i_setting];// << " (" << hFoldedSpectrum->Integral() << " entries)";
    leg->AddEntry(hFoldedSpectrum, leglabel.str().c_str(), "l");

    double before_drop = 15;
    double after_drop = 15.5;
    int bin_before_drop = hFoldedSpectrum->GetXaxis()->FindBin(before_drop);
    int bin_after_drop = hFoldedSpectrum->GetXaxis()->FindBin(after_drop);
    std::cout << leglabel.str() << std::endl;
    std::cout << "Count before drop = " << hFoldedSpectrum->GetBinContent(bin_before_drop) << std::endl;
    std::cout << "Count after drop = " << hFoldedSpectrum->GetBinContent(bin_after_drop) << std::endl;

    alcaphistogram(hFoldedSpectrum);
    if (i_setting == 0) {
      alcapPreliminary(hFoldedSpectrum);
    }
    else {
      hFoldedSpectrum->SetDrawOption("HIST E1 SAME");
    }
  }

  leg->Draw();

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_Si16bDataset_RawSpectra_SiR3Veto";

    std::string pdfname = savename + ".pdf";
    c1->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c1->SaveAs(pngname.c_str());

    c1->SetLogy();
    savename = savedir + "AlCapData_Si16bDataset_RawSpectra_SiR3Veto_LogY";

    pdfname = savename + ".pdf";
    c1->SaveAs(pdfname.c_str());
    pngname = savename + ".png";
    c1->SaveAs(pngname.c_str());
  }

  //  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.55, 0.80, "AlCap Preliminary");
}
