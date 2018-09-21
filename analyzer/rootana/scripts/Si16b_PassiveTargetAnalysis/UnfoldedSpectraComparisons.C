void UnfoldedSpectraComparisons() {

  /////////////////////////////////////
  // User parameters
  const int n_settings = 3;
  std::string suffix[n_settings] = {"", "-shallower", "-shallower-still"};
  std::string basefilename = "~/data/results/Si16b_passive/unfolded-wVeto";
  std::string histname = "hUnfoldedSpectrum";

  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string filename = basefilename + suffix[i_setting] + ".root";
    TFile* file = new TFile(filename.c_str(), "READ");
    
    TH1F* hUnfoldedSpectrum = (TH1F*) file->Get(histname.c_str());
    hUnfoldedSpectrum->GetXaxis()->SetRangeUser(0, 15000);
    hUnfoldedSpectrum->SetTitle("Unfolded Proton Spectrum (Si16b, w/ SiR3 Veto)");
    hUnfoldedSpectrum->Draw("HIST E SAME");
  }

}
