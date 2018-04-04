void FoldedSpectrum() {

  /////////////////////////////////////
  // User parameters
  std::string filename = "~/data/results/Si16b_passive/EvdEPlots.root";
  std::string histname = "hEvdE_Si16b_Proton_Veto";

  
  TFile* file = new TFile(filename.c_str(), "READ");
  TH2F* hProtonEnergy = (TH2F*) file->Get(histname.c_str());

  TH1D* hFoldedSpectrum = hProtonEnergy->ProjectionX();
  hFoldedSpectrum->Rebin(50);
  hFoldedSpectrum->SetYTitle("Counts / 500 keV");
  hFoldedSpectrum->SetLineColor(kBlue);
  hFoldedSpectrum->SetLineWidth(2);
  
  double error;
  int first_bin = 1;
  int last_bin = hFoldedSpectrum->GetNbinsX();
  double min_integral = 0;
  double max_integral = 10000;
  int min_integral_bin = hFoldedSpectrum->FindBin(min_integral);
  int max_integral_bin = hFoldedSpectrum->FindBin(max_integral);
  std::cout << "Total Integral = " << hFoldedSpectrum->IntegralAndError(first_bin, last_bin, error) << " +/- " << error << std::endl;
  std::cout << min_integral << " -- " << max_integral << " keV Integral = " << hFoldedSpectrum->IntegralAndError(min_integral_bin, max_integral_bin, error) << " +/- " << error << std::endl;
  std::cout << std::endl;
  hFoldedSpectrum->Draw();
}
