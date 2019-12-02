void Si16b_FinalPlot_FoldedSpectra_DifferentNTgtMuonCut() {

  /*
  const int n_settings = 5;
  std::string settings[n_settings] = {"", "_geq1TgtPulse", "_geq1TgtPulse_SiL1-2--6", "_geq1TgtPulse_SiL1-7--10", "_geq1TgtPulse_SiL1-11--15"};
  std::string leglabels[n_settings] = {"#geq0 Tgt Pulse", "#geq1 Tgt Pulse", "#geq1 Tgt Pulse (SiL1-2 -- SiL1-6)", "#geq1 Tgt Pulse (SiL1-7 -- SiL1-10)", "#geq1 Tgt Pulse (SiL1-11 -- SiL1-15)"};
  Int_t colours[n_settings] = {kRed, kBlue, kMagenta, kCyan, kSpring};
  */

  const int n_settings = 2;
  std::string settings[n_settings] = {"_geq1TgtPulse", "_0TgtPulse"};
  std::string leglabels[n_settings] = {"#geq1 Tgt Pulse", "#splitline{0 Tgt Pulse}{NB some inactive strips}"};
  Int_t colours[n_settings] = {kRed, kBlue};

  TCanvas* c_Spectra = new TCanvas("c_Spectra", "c_Spectra");
  int rebin_factor = 50;
  std::stringstream axistitle;

  TLegend* leg = new TLegend(0.50,0.45,0.80,0.75);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  std::string particle = "proton";
  //  std::string particle = "deuteron";
  //  std::string particle = "triton";
  //  std::string particle = "alpha";
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = settings[i_setting];

    std::string infilename = "~/data/results/Si16b/raw_spectra_newPP" + setting + ".root";
    TFile* infile = new TFile(infilename.c_str(), "READ");
    if (infile->IsZombie()) {
      std::cout << "Can't open file " << infilename << std::endl;
      continue;
    }

    std::string folded_histname = particle + "_TCutG/hRawSpectrum";
    TH1F* hFoldedSpectrum = (TH1F*) infile->Get(folded_histname.c_str());
    if (!hFoldedSpectrum) {
      std::cout << "Can't find histogram " << folded_histname << std::endl;
      continue;
    }

    //  hFoldedSpectrum->SetTitle(outhisttitle.c_str());
    hFoldedSpectrum->Rebin(rebin_factor);
    hFoldedSpectrum->SetStats(false);
    hFoldedSpectrum->SetLineColor(colours[i_setting]);
    hFoldedSpectrum->SetLineWidth(2);
    hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 25000);
    hFoldedSpectrum->GetYaxis()->SetTitleOffset(1.3);
    std::string histtitle = "Si16b Dataset, Right Arm, Folded Spectrum (" + particle + ")";
    hFoldedSpectrum->SetTitle(histtitle.c_str());
    axistitle.str("");
    axistitle << "Raw Count / " << hFoldedSpectrum->GetXaxis()->GetBinWidth(1) << " keV";
    //    hFoldedSpectrum->Scale(1.0 / hFoldedSpectrum->Integral());
    //    axistitle << "Unit Area / " << hFoldedSpectrum->GetXaxis()->GetBinWidth(1) << " keV";
    hFoldedSpectrum->SetYTitle(axistitle.str().c_str());
  
    hFoldedSpectrum->Draw("HIST E SAME");

    leg->AddEntry(hFoldedSpectrum, leglabels[i_setting].c_str(), "l");

    /*
    const int n_ranges = 5;
    double min_energies[n_ranges] = {1400,  10000, 5000,   3500, 4000};
    double max_energies[n_ranges] = {26000, 26000, 10000, 10000, 8000};
    
    for (int i_range = 0; i_range < n_ranges; ++i_range) {
      double min_energy = min_energies[i_range];
      double max_energy = max_energies[i_range];
      int min_energy_bin = hFoldedSpectrum->GetXaxis()->FindBin(min_energy);
      int max_energy_bin = hFoldedSpectrum->GetXaxis()->FindBin(max_energy) - 1;
      double error = 0;
      std::cout << "Folded Integral (" << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << hFoldedSpectrum->IntegralAndError(min_energy_bin, max_energy_bin, error) << " +/- " << error << std::endl;
    }
    */
  }
  leg->Draw();
}
