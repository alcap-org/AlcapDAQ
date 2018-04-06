void UnfoldedSpectrum() {

  /////////////////////////////////////
  // User parameters
  std::string filename = "~/data/results/Si16b_passive/unfolded-woutVeto.root";
  std::string histname = "hUnfoldedSpectrum";

  TFile* file = new TFile(filename.c_str(), "READ");
    
  TH1F* hUnfoldedSpectrum = (TH1F*) file->Get(histname.c_str());
  hUnfoldedSpectrum->GetXaxis()->SetRangeUser(0, 15000);
  hUnfoldedSpectrum->SetTitle("Unfolded Proton Spectrum (Si16b, w/o SiR3 Veto)");
  hUnfoldedSpectrum->Draw("HIST E");

  double integral_min = 2000;
  double integral_max = 15000;
  int integral_bin_min = hUnfoldedSpectrum->FindBin(integral_min);
  int integral_bin_max = hUnfoldedSpectrum->FindBin(integral_max);
  double integral, error;
  integral = hUnfoldedSpectrum->IntegralAndError(integral_bin_min, integral_bin_max, error);
  std::cout << "Integral (" << integral_min << " -- " << integral_max << " keV): " << integral << " +- " << error << std::endl;
  
  integral_min = 3000;
  integral_max = 10000;
  integral_bin_min = hUnfoldedSpectrum->FindBin(integral_min);
  integral_bin_max = hUnfoldedSpectrum->FindBin(integral_max);
  integral = hUnfoldedSpectrum->IntegralAndError(integral_bin_min, integral_bin_max, error);
  std::cout << "Integral (" << integral_min << " -- " << integral_max << " keV): " << integral << " +- " << error << std::endl;
  
  integral_min = 4000;
  integral_max = 8000;
  integral_bin_min = hUnfoldedSpectrum->FindBin(integral_min);
  integral_bin_max = hUnfoldedSpectrum->FindBin(integral_max);
  integral = hUnfoldedSpectrum->IntegralAndError(integral_bin_min, integral_bin_max, error);
  std::cout << "Integral (" << integral_min << " -- " << integral_max << " keV): " << integral << " +- " << error << std::endl;
  
}
