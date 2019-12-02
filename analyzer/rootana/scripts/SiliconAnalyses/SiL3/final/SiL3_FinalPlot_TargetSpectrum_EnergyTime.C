void SiL3_FinalPlot_TargetSpectrum_EnergyTime() {

  //  std::string plots_file_name = "~/data/results/SiL3/plots_geq2TgtPulse.root";
  //  std::string plots_file_name = "~/data/results/SiL3_active_no-templates/plots_geq2TgtPulse.root";
  std::string plots_file_name = "~/data/results/SiL3/plots_geq2TgtPulse_newPP20us.root";
  std::string full_spectrum_name = "Target/hEnergyTime";
    
  TFile* plots_file = new TFile(plots_file_name.c_str(), "READ");
  TH2F* hEnergyTime = (TH2F*) plots_file->Get(full_spectrum_name.c_str());
  
  TCanvas* c_EnergyTime = new TCanvas("c_EnergyTime", "c_EnergyTime");
  c_EnergyTime->SetLogz();

  std::string histtitle = "SiL3 Dataset, Active Target Analysis, Energy vs Time";
  hEnergyTime->SetTitle(histtitle.c_str());
  hEnergyTime->SetStats(false);
  //  hEnergyTime->Rebin2D(2, 40);
  //  std::stringstream axislabel;
  //  std::cout << "Counts / " << hEnergyTime->GetXaxis()->GetBinWidth(1) << " ns" << std::endl;
  //  std::cout << "Counts / " << hEnergyTime->GetYaxis()->GetBinWidth(1) << " keV" << std::endl;
  //  hEnergyTime->SetYTitle(axislabel.str().c_str());
  //  hEnergyTime->GetYaxis()->SetTitleOffset(1.3);
  
  hEnergyTime->Draw("COLZ");

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.15, 0.70, "AlCap Preliminary");
  
  hEnergyTime->GetXaxis()->SetRangeUser(-20000, 20000);
  hEnergyTime->GetYaxis()->SetRangeUser(0, 100000);

  double y_max = 30000;
  TCanvas* c_EnergyTime_Zoom = new TCanvas("c_EnergyTime_Zoom", "c_EnergyTime_Zoom");
  c_EnergyTime_Zoom->SetLogz();
  TH2F* hEnergyTime_Zoom = (TH2F*) hEnergyTime->Clone("hEnergyTime_Zoom");
  hEnergyTime_Zoom->GetXaxis()->SetRangeUser(-1000, 10000);
  hEnergyTime_Zoom->GetYaxis()->SetRangeUser(0, y_max);
  //  hEnergyTime_Zoom->GetXaxis()->SetRangeUser(2000, 10000);
  //  hEnergyTime_Zoom->GetYaxis()->SetRangeUser(10000, 30000);
  hEnergyTime_Zoom->Draw("COLZ");

  double min_time = 2000;
  double max_time = 4000;
  TLine* min_time_line = new TLine(min_time, 0, min_time, y_max);
  min_time_line->SetLineWidth(2);
  min_time_line->SetLineColor(kRed);
  min_time_line->Draw("LSAME");
  TLine* max_time_line = new TLine(max_time, 0, max_time, y_max);
  max_time_line->SetLineWidth(2);
  max_time_line->SetLineColor(kRed);
  max_time_line->Draw("LSAME");
  
  latex->DrawLatexNDC(0.50, 0.70, "AlCap Preliminary");
}
