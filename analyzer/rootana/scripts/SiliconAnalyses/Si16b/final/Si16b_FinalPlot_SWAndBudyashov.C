void Si16b_FinalPlot_SWAndBudyashov() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();

  double x_max = 55000;
  const int n_points = 2;
  double axis_limits[n_points] = {0, x_max};
  double ys[n_points] = {0, 0};
  TGraph* axis_gre = new TGraph(n_points, axis_limits, ys);
  axis_gre->Draw("AP");
  axis_gre->GetYaxis()->SetRangeUser(1e-8, 4e-3);
  

  std::string SW_filename = "~/data/results/SiL3/SW-plots.root";
  //  std::string SW_filename = "~/data/results/Si16b/SW-plots-above1400keV.root";
  TFile* SW_file = new TFile(SW_filename.c_str(), "READ");
  TGraphErrors* SW_gre = (TGraphErrors*) SW_file->Get("Graph");
  SW_gre->Draw("PE SAME");
  SW_gre->SetTitle("Charged Particle Emission");
  SW_gre->GetXaxis()->SetRangeUser(0,x_max);
  SW_gre->GetXaxis()->SetTitle("Energy [keV]");
  SW_gre->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per keV");

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(SW_gre, "Sobottka-Wills", "pl");
  SW_gre->GetFunction("tdr_fit")->SetBit(TF1::kNotDraw);
  //  SW_gre->GetFunction("tdr_fit")->SetLineColor(kBlack);
  //  leg->AddEntry(SW_gre->GetFunction("tdr_fit"), "S-W Fit", "l");

  std::string Budyashov_filename = "~/data/results/Si16b/Budyashov-plots.root";
  TFile* Budyashov_file = new TFile(Budyashov_filename.c_str(), "READ");
  TGraphErrors* Budyashov_gre = (TGraphErrors*) Budyashov_file->Get("sum");
  Budyashov_gre->SetMarkerColor(kBlue);
  Budyashov_gre->SetMarkerStyle(7);
  Budyashov_gre->SetLineColor(kBlue);
  Budyashov_gre->Draw("PE SAME");
  Budyashov_gre->SetTitle("Charged Particle Emission");
  Budyashov_gre->GetXaxis()->SetRangeUser(0,x_max);
  Budyashov_gre->GetXaxis()->SetTitle("Energy [keV]");
  Budyashov_gre->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per keV");
  leg->AddEntry(Budyashov_gre, "Budyashov et al.", "pl");
    
  leg->Draw();
}
