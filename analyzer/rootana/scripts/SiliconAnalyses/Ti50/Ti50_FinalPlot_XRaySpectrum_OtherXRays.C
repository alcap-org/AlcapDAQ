void Ti50_FinalPlot_XRaySpectrum_OtherXRays() {

  std::string plots_file_name = "~/data/results/Ti50/plots_test.root";
  std::string full_spectrum_name = "hXRaySpectrum_GeLoGain";
    
  TFile* plots_file = new TFile(plots_file_name.c_str(), "READ");
  TH1D* hXRaySpectrum = ((TH2F*) plots_file->Get(full_spectrum_name.c_str()))->ProjectionY();

  TCanvas* c_XRaySpectrum = new TCanvas("c_XRaySpectrum", "c_XRaySpectrum");

  hXRaySpectrum->Rebin(4);
  //  hXRaySpectrum->SetLineColor(kBlue);
  hXRaySpectrum->SetTitle("Ti50 Dataset, Full X-Ray Spectrum");
  hXRaySpectrum->SetStats(false);
  hXRaySpectrum->GetXaxis()->SetRangeUser(1000, 1300);

  std::stringstream axislabel;
  axislabel << "Counts / " << hXRaySpectrum->GetBinWidth(1) << " keV";
  hXRaySpectrum->SetYTitle(axislabel.str().c_str());
  hXRaySpectrum->GetYaxis()->SetTitleOffset(1.3);
  
  hXRaySpectrum->Draw("HIST E");

  const int n_xrays = 5;
  double xray_energies[n_xrays] = {1122, 1189, 1220, 1236, 1246};
  double xray_intensities[n_xrays] = {10.20, 3.10, 3.60, 4.10, 2.30};
  double label_offsets[n_xrays] = {10, 10, 20, 50, 20};

  TLatex* latex = new TLatex();
  
  for (int i_xray = 0; i_xray < n_xrays; ++i_xray) {
    double xray_energy = xray_energies[i_xray];
    double xray_intensity = xray_intensities[i_xray];
    double label_offset = label_offsets[i_xray];
    
    int bin_xray = hXRaySpectrum->FindBin(xray_energy);
    double arrow_y_end = hXRaySpectrum->GetBinContent(bin_xray)*1.05;
    double arrow_y_start = arrow_y_end+25;
    TArrow* arrow = new TArrow(xray_energy, arrow_y_start, xray_energy, arrow_y_end, 0.01);
    arrow->SetLineColor(kRed);
    arrow->SetLineWidth(2);
    arrow->Draw();

    std::stringstream text;
    text.str("");
    text << i_xray+3 << "p-1s";
    latex->SetTextAlign(22);
    latex->SetTextSize(0.03);
    latex->DrawLatex(xray_energy, arrow_y_start+label_offset+20, text.str().c_str());
  
    text.str("");
    text << "(" << xray_energy << " keV, I = " << xray_intensity << "%)";
    latex->SetTextSize(0.03);
    latex->DrawLatex(xray_energy, arrow_y_start+label_offset, text.str().c_str());
  }
}
