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

  TCanvas* c_EnergyTime_Zoom = new TCanvas("c_EnergyTime_Zoom", "c_EnergyTime_Zoom");
  c_EnergyTime_Zoom->SetLogz();
  TH1F* hEnergyTime_Zoom = hEnergyTime->Clone("hEnergyTime_Zoom");
  hEnergyTime_Zoom->GetXaxis()->SetRangeUser(-1000, 10000);
  hEnergyTime_Zoom->GetYaxis()->SetRangeUser(0, 30000);
  //  hEnergyTime_Zoom->GetXaxis()->SetRangeUser(2000, 10000);
  //  hEnergyTime_Zoom->GetYaxis()->SetRangeUser(10000, 30000);
  hEnergyTime_Zoom->Draw("COLZ");
  latex->DrawLatexNDC(0.50, 0.70, "AlCap Preliminary");
}
