void FoldedSpectrum() {

  /////////////////////////////////////
  // User parameters
  std::string filename = "~/data/results/Si16b_passive/unfolded-wVeto_NewCuts.root";
  std::string histname = "hFoldedSpectrum";
  
  TFile* file = new TFile(filename.c_str(), "READ");

  TH1F* hFoldedSpectrum = (TH1F*) file->Get(histname.c_str());
  hFoldedSpectrum->SetLineColor(kRed);
  hFoldedSpectrum->SetLineWidth(2);
  hFoldedSpectrum->SetTitle("Folded Spectrum, Si16b (w/ SiR3 Veto)");
  
  hFoldedSpectrum->Draw("HIST E");
}
