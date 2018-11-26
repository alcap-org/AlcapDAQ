void Ti50_FinalPlot_SiR23_EnergyTime() {

  std::string plots_file_name = "~/data/results/Ti50/plots.root";
  std::string full_spectrum_name = "SiR23/hEnergyTime";
    
  TFile* plots_file = new TFile(plots_file_name.c_str(), "READ");
  TH2F* hEnergyTime = (TH2F*) plots_file->Get(full_spectrum_name.c_str());
  
  TCanvas* c_EnergyTime = new TCanvas("c_EnergyTime", "c_EnergyTime");
  c_EnergyTime->SetLogz();

  std::string histtitle = "Ti50 Dataset, SiR2 Spectrum (!SiR1 && SiR3), Energy vs Time";
  hEnergyTime->SetTitle(histtitle.c_str());
  hEnergyTime->SetStats(false);

  //  std::stringstream axislabel;
  //  axislabel << "Counts / " << hEnergyTime->GetBinWidth(1) << " keV";
  //  hEnergyTime->SetYTitle(axislabel.str().c_str());
  //  hEnergyTime->GetYaxis()->SetTitleOffset(1.3);
  
  hEnergyTime->Draw("COLZ");

  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.15, 0.70, "AlCap Preliminary");

  hEnergyTime->GetXaxis()->SetRangeUser(-1000, 10000);
  hEnergyTime->GetYaxis()->SetRangeUser(0, 10000);
  latex->DrawLatexNDC(0.50, 0.70, "AlCap Preliminary");
}
