void Si16a_EfficiencyComparisons() {

  const int n_efficiencies = 13;

  //  std::string xray_count_tags[n_counts] = {"Full XRay Spectrum", "XRay Spectrum w/ SiL3 Hit in TME"};
  //  std::string target_count_tags[n_counts] = {"-200 < t < 200 ns && 3000 < E < 600 keV", "-200 < t < 200 ns && 3900 < E < 4000 keV"};

  TH1F* hEfficiencies = new TH1F("hEfficiencies", "Calculated Efficiencies", n_efficiencies,0,n_efficiencies);

  std::string bin_labels[n_efficiencies] = {"from Ge Eff run 9302", "from Ge Eff run 10139",
					    "SiL3 (All)", "SiL3 (Cross Check)", "SiL3 (Cross Check Tight)",
					    "Run 9737 (SF = 0.98)", "Run 9736 (SF = 0.99)",
					    "Run 9739 (SF = 1.01)", "Run 9740 (SF = 1.02)",
					    "Run 9741 (SF = 1.03)", "Run 9735 (SF = 1.035)",
					    "Run 9742 (SF = 1.04)", "Run 9743 (SF = 1.05)"};
  
  double efficiency_vals[n_efficiencies] = {7.4e-4, 6.7e-4,
					    7.73e-4, 5.86e-4, 5.95e-4,
					    5.15e-4, 10.61e-4,
					    5.09e-4, 6.90e-4,
					    6.63e-4, 6.79e-4,
					    6.05e-4, 5.12e-4};
  double efficiency_errs[n_efficiencies] = {0.1e-4, 0.3e-4,
					    0.11e-4, 0.09e-4, 0.18e-4,
					    2.79e-4, 0.71e-4,
					    0.66e-4, 0.73e-4,
					    0.51e-4, 0.51e-4,
					    0.07e-4, 0.85e-4};

  for (int i_eff = 0; i_eff < n_efficiencies; ++i_eff) {
    int i_bin = i_eff+1;
    hEfficiencies->GetXaxis()->SetBinLabel(i_bin, bin_labels[i_eff].c_str());
    hEfficiencies->SetBinContent(i_bin, efficiency_vals[i_eff]);
    hEfficiencies->SetBinError(i_bin, efficiency_errs[i_eff]);
  }

  double min_axis = 0;
  double max_axis = 12e-4;
  hEfficiencies->GetYaxis()->SetRangeUser(min_axis, max_axis);
  hEfficiencies->SetStats(false);
  hEfficiencies->Draw("HIST E");
  
  double ge_eff_run_element_start = 0;
  double ge_eff_run_element_end = 2;
  TLine* ge_eff_run_line = new TLine(ge_eff_run_element_end, min_axis, ge_eff_run_element_end, max_axis);
  ge_eff_run_line->Draw("LSAME");

  TLatex* latex = new TLatex();
  latex->SetTextAlign(22);
  latex->SetTextSize(0.035);
  double latex_x = ge_eff_run_element_start + (ge_eff_run_element_end - ge_eff_run_element_start)/2;
  double latex_y = 9.75e-4;
  latex->DrawLatex(latex_x, latex_y, "Ge Eff Runs");

  double sil3_run_element_start = ge_eff_run_element_end;
  double sil3_run_element_end = 5;
  TLine* sil3_run_line = new TLine(sil3_run_element_end, min_axis, sil3_run_element_end, max_axis);
  sil3_run_line->Draw("LSAME");

  latex_x = sil3_run_element_start + (sil3_run_element_end - sil3_run_element_start)/2;
  latex->DrawLatex(latex_x, latex_y, "SiL3 Runs");

  double si16a_run_element_start = sil3_run_element_end;
  double si16a_run_element_end = n_efficiencies;
  TLine* si16a_run_line = new TLine(si16a_run_element_end, min_axis, si16a_run_element_end, max_axis);
  si16a_run_line->Draw("LSAME");

  latex_x = si16a_run_element_start + (si16a_run_element_end - si16a_run_element_start)/2;
  latex->DrawLatex(latex_x, latex_y, "Si16a Runs");
}
