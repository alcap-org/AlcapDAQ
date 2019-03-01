void SiL3_FinalPlot_FoldedSpectra() {

  std::string infilename = "~/data/results/SiL3/unfold.root";
  std::string folded_histname = "GeomAcceptance_TimeSlice2000_4000/hCorrectedSpectrum";
  //  std::string folded_histname = "ResponseMatrix_alpha/hUnfoldedSpectrum";
  //  std::string outhisttitle = "SiL3 Dataset, Right Arm, " + particle;
  
  TFile* infile = new TFile(infilename.c_str(), "READ");
  TH1F* hFoldedSpectrum = (TH1F*) infile->Get(folded_histname.c_str());
  //  TH1F* hUnfoldedSpectrum = (TH1F*) infile->Get(unfolded_histname.c_str());

  TCanvas* c_Spectra = new TCanvas("c_Spectra", "c_Spectra");

  //  hFoldedSpectrum->SetTitle(outhisttitle.c_str());
  hFoldedSpectrum->SetStats(false);
  hFoldedSpectrum->SetLineColor(kBlack);
  hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 15000);
  hFoldedSpectrum->GetYaxis()->SetTitleOffset(1.3);
  
  hFoldedSpectrum->Draw("HIST E");
  c_Spectra->Update();
  
  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");

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
