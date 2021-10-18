void Si16b_FinalPlot_FoldedSpectra() {

  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  Int_t colours[n_particles] = {kRed, kCyan, kMagenta, kSpring};

  TCanvas* c_Spectra = new TCanvas("c_Spectra", "c_Spectra");
  int rebin_factor = 1;
  std::stringstream axistitle;

  TLegend* leg = new TLegend(0.70,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  std::string infilename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_1.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    
    std::string folded_histname = "TimeCut_" + particle + "_TCutG_2sig_layerCoinc500ns_tGT0ns/hInputSpectrum";
    //    std::string folded_histname = "PIDCut_" + particle + "_PSel/hCorrectedSpectrum";
    TH1F* hFoldedSpectrum = (TH1F*) infile->Get(folded_histname.c_str());
    if (!hFoldedSpectrum) {
      std::cout << "Can't find histogram " << folded_histname << std::endl;
      continue;
    }

    //  hFoldedSpectrum->SetTitle(outhisttitle.c_str());
    hFoldedSpectrum->Rebin(rebin_factor);
    hFoldedSpectrum->SetStats(false);
    hFoldedSpectrum->SetLineColor(colours[i_particle]);
    hFoldedSpectrum->SetLineWidth(2);
    hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 26000);
    hFoldedSpectrum->GetYaxis()->SetTitleOffset(1.3);
    axistitle.str("");
    axistitle << "Raw Count / " << hFoldedSpectrum->GetXaxis()->GetBinWidth(1) << " keV";
    hFoldedSpectrum->SetTitle("Si16b Dataset, Right Arm, Raw Spectrum");
    hFoldedSpectrum->SetYTitle(axistitle.str().c_str());
  
    hFoldedSpectrum->Draw("HIST E SAME");

    leg->AddEntry(hFoldedSpectrum, particle.c_str(), "l");
    
    const int n_ranges = 5;
    double min_energies[n_ranges] = {1.400,  10.000, 5.000,   3.500, 4.000};
    double max_energies[n_ranges] = {26.000, 26.000, 10.000, 10.000, 8.000};
    
    for (int i_range = 0; i_range < n_ranges; ++i_range) {
      double min_energy = min_energies[i_range];
      double max_energy = max_energies[i_range];
      int min_energy_bin = hFoldedSpectrum->GetXaxis()->FindBin(min_energy);
      int max_energy_bin = hFoldedSpectrum->GetXaxis()->FindBin(max_energy) - 1;
      double error = 0;
      std::cout << "Folded Integral (" << min_energy << " MeV -- " << max_energy << " MeV) = " << hFoldedSpectrum->IntegralAndError(min_energy_bin, max_energy_bin, error) << " +/- " << error << std::endl;
    }
  }
  leg->Draw();
}
