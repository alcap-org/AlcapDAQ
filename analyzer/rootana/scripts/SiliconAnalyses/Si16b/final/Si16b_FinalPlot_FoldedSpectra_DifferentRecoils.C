void Si16b_FinalPlot_FoldedSpectra_DifferentRecoils() {

  std::string pid = "TCutG";

  //  std::string particle = "proton";
  //  std::string particle = "deuteron";
  //  std::string particle = "triton";
  std::string particle = "alpha";
  
  const int n_recoils = 2;
  std::string recoils[n_recoils] = {"noRecoil", "withRecoil"};
  std::string leglabels[n_recoils] = {"no recoil", "with recoil"};
  Int_t colours[n_recoils] = {kBlack, kRed};

  TCanvas* c_Spectra = new TCanvas("c_Spectra", "c_Spectra");
  int rebin_factor = 25;
  std::stringstream axistitle;

  TLegend* leg = new TLegend(0.50,0.45,0.80,0.75);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  std::string infilename = "~/data/results/Si16b/raw_spectra_newPP_geq1TgtPulse.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");
  for (int i_recoil = 0; i_recoil < n_recoils; ++i_recoil) {
    std::string recoil = recoils[i_recoil];
    
    std::string folded_histname = particle + "_" + pid + "_noTimeCut_" + recoil + "/hRawSpectrum";
    TH1F* hFoldedSpectrum = (TH1F*) infile->Get(folded_histname.c_str());

    //  hFoldedSpectrum->SetTitle(outhisttitle.c_str());
    hFoldedSpectrum->Rebin(rebin_factor);
    hFoldedSpectrum->SetStats(false);
    hFoldedSpectrum->SetLineColor(colours[i_recoil]);
    hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 20000);
    hFoldedSpectrum->GetYaxis()->SetTitleOffset(1.3);
    axistitle.str("");
    axistitle << "Raw Count / " << hFoldedSpectrum->GetXaxis()->GetBinWidth(1) << " keV";
    std::string histtitle = "Si16b Dataset, Right Arm, Raw Spectrum, " + particle + " (with recoil correction)";
    hFoldedSpectrum->SetTitle(histtitle.c_str());
    hFoldedSpectrum->SetYTitle(axistitle.str().c_str());
  
    hFoldedSpectrum->Draw("HIST E SAME");

    leg->AddEntry(hFoldedSpectrum, leglabels[i_recoil].c_str(), "l");
    
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
  }
  leg->Draw();
}
