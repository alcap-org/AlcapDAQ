void Si16b_FinalPlot_Budyashov(std::string savedir = "") {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();

  double x_max = 55000;
  const int n_points = 2;
  double axis_limits[n_points] = {0, x_max};
  double ys[n_points] = {0, 0};
  TGraph* axis_gre = new TGraph(n_points, axis_limits, ys);
  axis_gre->Draw("AP");
  axis_gre->GetYaxis()->SetRangeUser(1e-8, 1e-5);
  axis_gre->SetTitle("Budyashov et al.");
  axis_gre->GetXaxis()->SetTitle("Energy [keV]");
  axis_gre->GetYaxis()->SetTitle("Counts per Muon Capture per keV");
  


  TLegend* leg = new TLegend(0.50,0.55,0.85,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);

  std::string Budyashov_filename = "~/data/results/Si16b/Budyashov-plots.root";
  TFile* Budyashov_file = new TFile(Budyashov_filename.c_str(), "READ");

  TGraphErrors* Budyashov_gre_proton = (TGraphErrors*) Budyashov_file->Get("proton");
  Budyashov_gre_proton->SetMarkerColor(kRed);
  Budyashov_gre_proton->SetMarkerStyle(7);
  Budyashov_gre_proton->SetLineColor(kRed);
  Budyashov_gre_proton->Draw("PE SAME");
  Budyashov_gre_proton->SetTitle("Charged Particle Emission");
  Budyashov_gre_proton->GetXaxis()->SetRangeUser(0,26000);
  Budyashov_gre_proton->GetXaxis()->SetTitle("Energy [keV]");
  Budyashov_gre_proton->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per keV");
  leg->AddEntry(Budyashov_gre_proton, "Budyashov et al. (proton)", "pl");

  TGraphErrors* Budyashov_gre_deuteron = (TGraphErrors*) Budyashov_file->Get("deuteron");
  Budyashov_gre_deuteron->SetMarkerColor(kCyan);
  Budyashov_gre_deuteron->SetMarkerStyle(7);
  Budyashov_gre_deuteron->SetLineColor(kCyan);
  Budyashov_gre_deuteron->Draw("PE SAME");
  Budyashov_gre_deuteron->SetTitle("Charged Particle Emission");
  Budyashov_gre_deuteron->GetXaxis()->SetRangeUser(0,26000);
  Budyashov_gre_deuteron->GetXaxis()->SetTitle("Energy [keV]");
  Budyashov_gre_deuteron->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per keV");
  leg->AddEntry(Budyashov_gre_deuteron, "Budyashov et al. (deuteron)", "pl");

  TGraphErrors* Budyashov_gre = (TGraphErrors*) Budyashov_file->Get("sum");
  Budyashov_gre->SetMarkerColor(kBlack);
  Budyashov_gre->SetMarkerStyle(7);
  Budyashov_gre->SetLineColor(kBlack);
  Budyashov_gre->Draw("PE SAME");
  Budyashov_gre->SetTitle("Charged Particle Emission");
  Budyashov_gre->GetXaxis()->SetRangeUser(0,x_max);
  Budyashov_gre->GetXaxis()->SetTitle("Energy [keV]");
  Budyashov_gre->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per keV");
  leg->AddEntry(Budyashov_gre, "Budyashov et al. (p + d)", "pl");
    
  leg->Draw();

  if (savedir != "") {
    std::string savename = savedir + "Budyashov_Digitised";

    std::string pdfname = savename + ".pdf";
    c1->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c1->SaveAs(pngname.c_str());
  }
}
