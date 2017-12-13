void SiL1_PunchThroughPeak_FitResults() {

  const int n_runs = 3;
  double scale_factors[n_runs] = {1.05, 1.04, 1.02};
  double peak_pos[n_runs] = {1186.48, 1148.06, 1100.06};
  double peak_pos_err[n_runs] = {1.07, 0.96, 0.63};
  
  TGraphErrors* gre = new TGraphErrors(n_runs, scale_factors, peak_pos, 0, peak_pos_err);
  gre->Draw("A*E");
}
