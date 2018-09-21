void Si16a_CountComparisons() {

  const int n_counts = 8;

  //  std::string xray_count_tags[n_counts] = {"Full XRay Spectrum", "XRay Spectrum w/ SiL3 Hit in TME"};
  //  std::string target_count_tags[n_counts] = {"-200 < t < 200 ns && 3000 < E < 600 keV", "-200 < t < 200 ns && 3900 < E < 4000 keV"};

  double xray_count_vals[n_counts] = {21.1e3, 57.7e3, 105.1e3, 115.0e3, 129.6e3, 138.4e3, 131.0e3, 106.4e3};
  double xray_count_errs[n_counts] = {0.1e3, 0.2e3, 0.3e3, 0.3e3, 0.4e3, 0.4e3, 0.4e3, 0.3e3};

  double target_count_vals[n_counts] = {14.6e3, 82.3e3, 72.0e3, 106.7e3, 115.6e3, 126.5e3, 106.7e3, 73.2e3 };
  double target_count_errs[n_counts] = {7.9e3, 5.6e3, 9.3e3, 11.3e3, 9.1e3, 9.7e3, 1.9e3, 12.1e3};

  double min_axis = 1e3;
  double max_axis = 150e3;
  TGraphErrors* gre = new TGraphErrors(n_counts, xray_count_vals, target_count_vals, xray_count_errs, target_count_errs);
  gre->Draw("A*E");
  gre->GetXaxis()->SetTitle("X-Ray Count");
  gre->GetYaxis()->SetTitle("Target Count");
  gre->GetXaxis()->SetRangeUser(min_axis, max_axis);
  gre->GetYaxis()->SetRangeUser(min_axis, max_axis);

  TLine* line = new TLine(min_axis, min_axis, max_axis, max_axis);
  line->SetLineWidth(2);
  line->SetLineColor(kBlack);
  line->SetLineStyle(kDashed);
  line->Draw("LSAME");
}
