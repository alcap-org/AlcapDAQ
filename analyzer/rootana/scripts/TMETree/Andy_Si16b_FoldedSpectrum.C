void Andy_Si16b_FoldedSpectrum() {
  TFile* file = new TFile("Andy_Si16b_EvdEPlot.root");
  TH2F* hProton_NoVeto = (TH2F*) file->Get("hEvdE_Si16b_Proton_NoVeto");
  TH2F* hProton_Veto = (TH2F*) file->Get("hEvdE_Si16b_Proton_Veto");

  TH1D* hFoldedSpectrum_NoVeto = hProton_NoVeto->ProjectionX();
  hFoldedSpectrum_NoVeto->Rebin(50);
  hFoldedSpectrum_NoVeto->SetYTitle("Counts / 500 keV");
  hFoldedSpectrum_NoVeto->SetLineColor(kBlue);
  hFoldedSpectrum_NoVeto->SetLineWidth(2);
  
  double error;
  int first_bin = 1;
  int last_bin = hFoldedSpectrum_NoVeto->GetNbinsX();
  double min_integral = 0;
  double max_integral = 10000;
  int min_integral_bin = hFoldedSpectrum_NoVeto->FindBin(min_integral);
  int max_integral_bin = hFoldedSpectrum_NoVeto->FindBin(max_integral);
  std::cout << "No Veto:" << std::endl;
  std::cout << "Total Integral = " << hFoldedSpectrum_NoVeto->IntegralAndError(first_bin, last_bin, error) << " +/- " << error << std::endl;
  std::cout << min_integral << " -- " << max_integral << " keV Integral = " << hFoldedSpectrum_NoVeto->IntegralAndError(min_integral_bin, max_integral_bin, error) << " +/- " << error << std::endl;
  std::cout << std::endl;
  hFoldedSpectrum_NoVeto->Draw();


  TH1D* hFoldedSpectrum_Veto = hProton_Veto->ProjectionX();
  hFoldedSpectrum_Veto->Rebin(50);
  hFoldedSpectrum_Veto->SetYTitle("Counts / 500 keV");
  hFoldedSpectrum_Veto->SetLineColor(kRed);
  hFoldedSpectrum_Veto->SetLineWidth(2);
  
  double error;
  int first_bin = 1;
  int last_bin = hFoldedSpectrum_Veto->GetNbinsX();
  std::cout << "Veto: " << std::endl;
  std::cout << "Total Integral = " << hFoldedSpectrum_Veto->IntegralAndError(first_bin, last_bin, error) << " +/- " << error << std::endl;
  std::cout << min_integral << " -- " << max_integral << " keV Integral = " << hFoldedSpectrum_Veto->IntegralAndError(min_integral_bin, max_integral_bin, error) << " +/- " << error << std::endl;
  hFoldedSpectrum_Veto->Draw("SAME");
}
