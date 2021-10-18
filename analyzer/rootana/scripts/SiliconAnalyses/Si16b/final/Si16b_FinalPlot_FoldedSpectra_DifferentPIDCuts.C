void Si16b_FinalPlot_FoldedSpectra_DifferentPIDCuts() {

  const int n_PIDs = 2;
  std::string PIDs[n_PIDs] = {"TCutG", "PSel"};
  std::string leglabels[n_PIDs] = {"Mark's PID", "John's PID"};
  Int_t colours[n_PIDs] = {kRed, kBlue};

  TCanvas* c_Spectra = new TCanvas("c_Spectra", "c_Spectra");
  int rebin_factor = 25;
  std::stringstream axistitle;

  TLegend* leg = new TLegend(0.50,0.45,0.80,0.75);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  //  std::string infilename = "~/data/results/Si16b/unfold_newPP.root";
  std::string infilename = "~/data/results/Si16b/raw_spectra_newPP_geq1TgtPulse_3sigma.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");
  for (int i_PID = 0; i_PID < n_PIDs; ++i_PID) {
    std::string i_pid = PIDs[i_PID];
    
    //    std::string folded_histname = "PIDCut_proton_" + i_pid + "/hInputSpectrum";
    //    std::string folded_histname = "PIDCut_proton_" + i_pid + "/hCorrectedSpectrum";
    std::string folded_histname = "proton_" + i_pid + "/hRawSpectrum";
    TH1F* hFoldedSpectrum = (TH1F*) infile->Get(folded_histname.c_str());


    //  hFoldedSpectrum->SetTitle(outhisttitle.c_str());
    hFoldedSpectrum->Rebin(rebin_factor);
    hFoldedSpectrum->SetStats(false);
    hFoldedSpectrum->SetLineColor(colours[i_PID]);
    hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 15000);
    hFoldedSpectrum->GetYaxis()->SetTitleOffset(1.3);
    axistitle.str("");
    axistitle << "Raw Count / " << hFoldedSpectrum->GetXaxis()->GetBinWidth(1) << " keV";
    hFoldedSpectrum->SetTitle("Si16b Dataset, Right Arm, Raw Spectrum (with eff correction)");
    hFoldedSpectrum->SetYTitle(axistitle.str().c_str());
  
    hFoldedSpectrum->Draw("HIST E SAME");

    leg->AddEntry(hFoldedSpectrum, leglabels[i_PID].c_str(), "l");
    
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
