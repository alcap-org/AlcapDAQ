void SiL3_CountComparisons() {

  const int n_xray_counts = 2;
  const int n_target_counts = 2;

  std::string xray_count_tags[n_xray_counts] = {"Full XRay Spectrum", "XRay Spectrum w/ SiL3 Hit in TME"};
  std::string target_count_tags[n_target_counts] = {"-200 < t < 200 ns && 3000 < E < 600 keV", "-200 < t < 200 ns && 3900 < E < 4000 keV"};

  double xray_count_vals[n_xray_counts] = {17.5e6, 2.12e6};
  double xray_count_errs[n_xray_counts] = {0.4e6, 0.07e6};

  double target_count_vals[n_target_counts] = {16.92e6, 2.638e6};
  double target_count_errs[n_target_counts] = {0.04e6, 0.016e6};

  double min_axis = 1e6;
  double max_axis = 20e6;
  TGraphErrors* gre = new TGraphErrors(n_xray_counts, xray_count_vals, target_count_vals, xray_count_errs, target_count_errs);
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
