void Si16b_FinalPlot_DecayElectronCorrection() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
  
  TFile* file = new TFile("~/data/results/Si16b/unfold.root", "READ");

  std::string dirname = "SiL3_DecayElectronCorrection_TimeSlice1000_5000";
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

  double min_energy = 350;
  double max_energy = 3000;
  int min_energy_bin = hInputSpectrum->GetXaxis()->FindBin(min_energy);
  int max_energy_bin = hInputSpectrum->GetXaxis()->FindBin(max_energy);

  double full_integral = hInputSpectrum->Integral(min_energy_bin, max_energy_bin);
  double decay_integral = hCorrection->Integral(min_energy_bin, max_energy_bin);

  std::cout << "Fraction of Spectrum below " << max_energy / 1000 << " MeV due to decay electron = " << decay_integral / full_integral << std::endl;
  
  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(hInputSpectrum, "w/FlatBkg Removal", "l");
  leg->AddEntry(hCorrection, "Decay Electron Correction", "l");
  leg->AddEntry(hCorrectedSpectrum, "w/Decay Electron Removal", "l");
  leg->Draw();
}
