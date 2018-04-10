void UnfoldedSpectrum() {

  /////////////////////////////////////
  // User parameters
  std::string filename = "~/data/results/Si16b_passive/unfolded-wVeto_NewCuts.root";
  std::string histname = "hUnfoldedSpectrum";

  TFile* file = new TFile(filename.c_str(), "READ");
    
  TH1F* hUnfoldedSpectrum = (TH1F*) file->Get(histname.c_str());
  hUnfoldedSpectrum->GetXaxis()->SetRangeUser(0, 15000);
  hUnfoldedSpectrum->SetTitle("Unfolded Proton Spectrum (Si16b, w/ SiR3 Veto)");
  hUnfoldedSpectrum->Draw("HIST E");

}
