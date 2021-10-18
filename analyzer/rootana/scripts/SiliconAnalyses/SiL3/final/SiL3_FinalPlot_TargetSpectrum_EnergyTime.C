void SiL3_FinalPlot_TargetSpectrum_EnergyTime(std::string savedir = "", std::ostream& numbers_file = std::cout) {
  numbers_file << "% SiL3_FinalPlot_TargetSpectrum_EnergyTime.C" << std::endl;
  
  //  std::string plots_file_name = "~/data/results/SiL3/plots_geq2TgtPulse.root";
  //  std::string plots_file_name = "~/data/results/SiL3_active_no-templates/plots_geq2TgtPulse.root";
  std::string plots_file_name = "~/data/results/SiL3/plots_geq2TgtPulse_newPP20us_1.root";
  std::string full_spectrum_name = "Target_allRecoil/hEnergyTime";
    
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
  hEnergyTime->SetYTitle("Energy [MeV]");
  //  hEnergyTime->GetYaxis()->SetTitleOffset(1.3);

  hEnergyTime->GetXaxis()->SetRangeUser(-20000, 20000);
  hEnergyTime->GetYaxis()->SetRangeUser(0, 50.000);
  hEnergyTime->GetXaxis()->SetTitleOffset(0.9);
  hEnergyTime->GetYaxis()->SetTitleOffset(0.9);
  hEnergyTime->Draw("COLZ");

  alcaphistogram(hEnergyTime);
  //  alcapPreliminary(hEnergyTime);
  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.6, 0.6, "AlCap #bf{#it{Preliminary}}");
  hEnergyTime->SetDrawOption("COLZ");

  
  double y_max = 30.000;
  TCanvas* c_EnergyTime_Zoom = new TCanvas("c_EnergyTime_Zoom", "c_EnergyTime_Zoom");
  c_EnergyTime_Zoom->SetLogz();
  TH2F* hEnergyTime_Zoom = (TH2F*) hEnergyTime->Clone("hEnergyTime_Zoom");
  hEnergyTime_Zoom->GetXaxis()->SetRangeUser(-1000, 10000);
  hEnergyTime_Zoom->GetYaxis()->SetRangeUser(0, y_max);
  //  hEnergyTime_Zoom->GetXaxis()->SetRangeUser(2000, 10000);
  //  hEnergyTime_Zoom->GetYaxis()->SetRangeUser(10000, 30000);
  hEnergyTime_Zoom->Draw("COLZ");

  alcaphistogram(hEnergyTime_Zoom);
  //  alcapPreliminary(hEnergyTime_Zoom);
  //  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.6, 0.6, "AlCap #bf{#it{Preliminary}}");
  hEnergyTime_Zoom->SetDrawOption("COLZ");
  
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

  numbers_file << "\\newcommand\\SiLTimeSlice{\\SI{" << min_time << "}{\\nano\\second} $<t<$ \\SI{" << max_time << "}{\\nano\\second}}" << std::endl;
  
  if (savedir != "") {
    std::string savename = savedir + "AlCapData_SiL3Dataset_ActiveTarget_EnergyTime";
    std::string pngname = savename + ".png";
    c_EnergyTime->SaveAs(pngname.c_str());

    savename = savedir + "AlCapData_SiL3Dataset_ActiveTarget_EnergyTime_Zoom";
    pngname = savename + ".png";
    c_EnergyTime_Zoom->SaveAs(pngname.c_str());
  }
  numbers_file << std::endl;
}
