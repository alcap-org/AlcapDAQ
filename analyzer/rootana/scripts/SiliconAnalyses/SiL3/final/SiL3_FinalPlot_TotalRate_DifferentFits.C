void SiL3_FinalPlot_TotalRate_DifferentFits() {

  const int n_points = 4;
  std::string fits[n_points] = {"combined", "combinedLow", "combinedHigh", "proton"};
  double xs[n_points] = {1, 2, 3, 4};
  double rates[n_points] = {0.168441, 0.168014, 0.16897, 0.164765};
  double rate_errs[n_points] = {0.00282126, 0, 0.00295102, 0.00310635};
  
  TGraphErrors* gr_all = new TGraphErrors(n_points, xs, rates, NULL, rate_errs);
  gr_all->Draw("A*E");
  gr_all->SetTitle("SiL3 Rates");
  gr_all->GetXaxis()->SetTitle("Fit Number");
  gr_all->GetYaxis()->SetTitle("Rate");

  // TLegend* leg = new TLegend(0.30,0.2,0.80,0.35);
  // leg->SetBorderSize(0);
  // leg->SetTextSize(0.035);
  // leg->SetFillColor(kWhite);
  // std::stringstream leglabel;
  // leglabel.str("");
  // leglabel << std::fixed << std::setprecision(2) << "John, Al50, Mean = " << gr_john->GetMean(2) << ", Std Dev = " << gr_john->GetRMS(2);
  // leg->AddEntry(gr_john, leglabel.str().c_str(), "pl");

  // leglabel.str("");
  // leglabel << "Mark, Al50, Mean = " << gr_mark->GetMean(2) << ", Std Dev = " << gr_mark->GetRMS(2);
  // leg->AddEntry(gr_mark, leglabel.str().c_str(), "pl");

  // leglabel.str("");
  // leglabel << "Andy, Si16b, Mean = " << gr_andy->GetMean(2) << ", Std Dev = " << gr_andy->GetRMS(2);
  // leg->AddEntry(gr_andy, leglabel.str().c_str(), "pl");

  // leglabel.str("");
  // leglabel << "All, Mean = " << gr_nomc->GetMean(2) << ", Std Dev = " << gr_nomc->GetRMS(2);
  // leg->AddEntry(gr_nomc, leglabel.str().c_str(), "pl");

  // leg->Draw();
}
