void SiL3_FinalPlot_FlatBackground() {

  std::string plots_file_name = "~/data/results/SiL3/plots_geq2TgtPulse.root";
  std::string full_spectrum_name = "Target/hEnergyTime";
    
  TFile* plots_file = new TFile(plots_file_name.c_str(), "READ");
  TH2F* hEnergyTime = (TH2F*) plots_file->Get(full_spectrum_name.c_str());

  const int n_energy_slices = 3;
  double min_energy_slices[n_energy_slices] = {0, 0, 3000};
  double max_energy_slices[n_energy_slices] = {5000, 3000, 5000};
  Int_t colours[n_energy_slices] = {kBlue, kRed, kBlack};

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);

  
  TCanvas* c_TimeSpectrum = new TCanvas("c_TimeSpectrum", "c_TimeSpectrum");
  std::stringstream histname, leglabel;
  for (int i_energy_slice = 0; i_energy_slice < n_energy_slices; ++i_energy_slice) {
    double min_energy_slice = min_energy_slices[i_energy_slice];
    double max_energy_slice = max_energy_slices[i_energy_slice];

    int min_energy_slice_bin = hEnergyTime->GetYaxis()->FindBin(min_energy_slice);
    int max_energy_slice_bin = hEnergyTime->GetYaxis()->FindBin(max_energy_slice)-1;

    histname.str("");
    histname << "hProj_" << min_energy_slice << "_" << max_energy_slice;
    TH1D* hTimeSpectrum = hEnergyTime->ProjectionX(histname.str().c_str(), min_energy_slice_bin, max_energy_slice_bin);
    hTimeSpectrum->Sumw2();
    //  hTimeSpectrum->Rebin(4);
    hTimeSpectrum->SetLineColor(colours[i_energy_slice]);
    //    std::string histtitle = "SiL3 Dataset";
    //    hTimeSpectrum->SetTitle(histtitle.c_str());
    hTimeSpectrum->SetStats(false);
    //    hTimeSpectrum->GetXaxis()->SetRangeUser(0, 30000);
    //    hTimeSpectrum->Scale(1.0 / hTimeSpectrum->GetXaxis()->GetBinWidth(1) );

    leglabel.str("");
    leglabel << min_energy_slice << " < E < " << max_energy_slice;
    leg->AddEntry(hTimeSpectrum, leglabel.str().c_str(), "l");
    
    hTimeSpectrum->Draw("HIST E SAME");
  }

  leg->Draw();

  hEnergyTime->Draw("COLZ");
  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.20, 0.70, "AlCap Preliminary");
}
