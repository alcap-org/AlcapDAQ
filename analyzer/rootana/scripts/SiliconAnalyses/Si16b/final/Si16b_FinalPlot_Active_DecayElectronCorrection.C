void Si16b_FinalPlot_Active_DecayElectronCorrection() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
  
  TFile* file = new TFile("~/data/results/Si16b/unfold_newPP20us_geq2TgtPulse.root", "READ");

  std::string dirname = "DecayElectron_Active";
  std::string histname = dirname + "/hInputSpectrum";
  TH1F* hInputSpectrum = (TH1F*) file->Get(histname.c_str());
  hInputSpectrum->SetLineColor(kBlue);
  hInputSpectrum->SetStats(false);
  hInputSpectrum->GetXaxis()->SetRangeUser(0, 10000);
  
  histname = dirname + "/hCorrection";
  TH1F* hCorrection = (TH1F*) file->Get(histname.c_str());
  hCorrection->SetLineColor(kBlack);

  histname = dirname + "/hCorrectedSpectrum";
  TH1F* hCorrectedSpectrum = (TH1F*) file->Get(histname.c_str());
  hCorrectedSpectrum->SetLineColor(kRed);

  hInputSpectrum->Draw("HIST E");
  hCorrection->Draw("HIST E SAME");
  hCorrectedSpectrum->Draw("HIST E SAME");

  
  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(hInputSpectrum, "w/FlatBkg Removal", "l");
  leg->AddEntry(hCorrection, "Decay Electron Correction", "l");
  leg->AddEntry(hCorrectedSpectrum, "w/Decay Electron Removal", "l");
  leg->Draw();
}
