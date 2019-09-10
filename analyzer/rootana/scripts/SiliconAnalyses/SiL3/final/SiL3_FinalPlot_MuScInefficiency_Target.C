void SiL3_FinalPlot_MuScInefficiency_Target() {
  std::string plots_file_name = "~/data/results/SiL3/plots_geq0TgtPulse_newPP.root";
  //  std::string plots_file_name = "~/data/results/SiL3/plots_09040_geq0TgtPulse.root";
  std::string full_spectrum_name = "Target/hEnergyTime";
    
  TFile* plots_file = new TFile(plots_file_name.c_str(), "READ");
  TH2F* hEnergyTime = (TH2F*) plots_file->Get(full_spectrum_name.c_str());

  double min_alltime = -5000;//-20000;
  double max_alltime = 5000;//20000;
  int min_alltime_bin = hEnergyTime->GetXaxis()->FindBin(min_alltime);
  int max_alltime_bin = hEnergyTime->GetXaxis()->FindBin(max_alltime)-1;
  TH1D* hAll = hEnergyTime->ProjectionY("hAll", min_alltime_bin, max_alltime_bin);

  double min_intime = -200;
  double max_intime = 200;
  int min_intime_bin = hEnergyTime->GetXaxis()->FindBin(min_intime);
  int max_intime_bin = hEnergyTime->GetXaxis()->FindBin(max_intime)-1;
  TH1D* hInTime = hEnergyTime->ProjectionY("hInTime", min_intime_bin, max_intime_bin);

  double min_outtime = -5000;//-20000;
  double max_outtime = -200;
  int min_outtime_bin = hEnergyTime->GetXaxis()->FindBin(min_outtime);
  int max_outtime_bin = hEnergyTime->GetXaxis()->FindBin(max_outtime)-1;
  TH1D* hOutTime = hEnergyTime->ProjectionY("hOutTime", min_outtime_bin, max_outtime_bin);

  double min_muon_energy = 3000;
  double max_muon_energy = 5000;
  int min_energy_bin = hInTime->GetXaxis()->FindBin(min_muon_energy);
  int max_energy_bin = hInTime->GetXaxis()->FindBin(max_muon_energy)-1;

  double all = hAll->Integral(min_energy_bin, max_energy_bin);
  double intime = hInTime->Integral(min_energy_bin, max_energy_bin);
  double outtime = hOutTime->Integral(min_energy_bin, max_energy_bin);

  std::cout << "All = " << all << ", intime = " << intime << ", outtime = " << outtime << std::endl;
  
  double efficiency = intime / all;
  std::cout << "Eff =  " << intime << " / " << all << " = " << efficiency << std::endl;

  efficiency = intime / (intime + outtime*2);
  std::cout << "Eff =  " << intime << " / " << intime + outtime*2 << " = " << efficiency << std::endl;

  std::stringstream text;
  TLatex* latex = new TLatex();
  latex->SetTextAlign(22);
  
  TCanvas* c_comp = new TCanvas("c_comp", "c_comp");
  c_comp->SetLogy();
  
  hAll->Draw();
  hAll->SetTitle("SiL3 Dataset, Active Target Analysis, Energy Spectra");
  hAll->SetStats(false);
  hAll->SetMaximum(2e7);
  hAll->SetMinimum(0.1);
  hAll->SetLineColor(kBlack);
  hAll->GetXaxis()->SetRangeUser(0, 10000);
  std::stringstream axislabel;
  axislabel.str("");
  axislabel << "Count / " << hAll->GetXaxis()->GetBinWidth(1) << " keV";
  hAll->SetYTitle(axislabel.str().c_str());

  hInTime->SetLineColor(kRed);
  hInTime->Draw("HIST SAME");
  text.str("");
  text << intime;
  latex->SetTextColor(kRed);
  latex->DrawLatexNDC(0.42, 0.55, text.str().c_str());

  hOutTime->Scale(2);  
  hOutTime->SetLineColor(kBlue);
  hOutTime->Draw("HIST SAME");
  text.str("");
  text << outtime*2;
  latex->SetTextColor(kBlue);
  latex->DrawLatexNDC(0.42, 0.15, text.str().c_str());

  text.str("");
  //  text << "#frac{in-time}{in-time + 2 #times out-of-time} = " << efficiency;
  text << std::fixed << std::setprecision(3) << "Efficiency = " << efficiency;
  latex->SetTextColor(kBlack);
  latex->DrawLatexNDC(0.70, 0.50, text.str().c_str());

  TLine* min_muon_line = new TLine(min_muon_energy, 0, min_muon_energy, hAll->GetMaximum());
  min_muon_line->SetLineWidth(2);
  min_muon_line->SetLineColor(kBlack);
  min_muon_line->Draw("LSAME");
  TLine* max_muon_line = new TLine(max_muon_energy, 0, max_muon_energy, hAll->GetMaximum());
  max_muon_line->SetLineWidth(2);
  max_muon_line->SetLineColor(kBlack);
  max_muon_line->Draw("LSAME");

  std::stringstream leglabel;
  TLegend* leg = new TLegend(0.50,0.65,0.75,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);
  leglabel.str("");
  leglabel << min_alltime << " ns < t < " << max_alltime << " ns (all)";
  leg->AddEntry(hAll, leglabel.str().c_str(), "l");

  leglabel.str("");
  leglabel << min_intime << " ns < t < " << max_intime << " ns (in-time)";
  leg->AddEntry(hInTime, leglabel.str().c_str(), "l");

  leglabel.str("");
  leglabel << min_outtime << " ns < t < " << max_outtime << " ns (out-of-time #times 2)";
  leg->AddEntry(hOutTime, leglabel.str().c_str(), "l");

  leg->Draw();

  TCanvas* c_EnergyTime = new TCanvas("c_EnergyTime", "c_EnergyTime");
  c_EnergyTime->SetLogz();
  hEnergyTime->SetStats(false);
  hEnergyTime->SetTitle("SiL3 Dataset, Active Target Analysis, Energy vs Time");
  hEnergyTime->Draw("COLZ");

  TLine* min_alltime_line = new TLine(min_alltime, hEnergyTime->GetYaxis()->GetXmin(), min_alltime, hEnergyTime->GetYaxis()->GetXmax());
  min_alltime_line->SetLineColor(kBlack);
  min_alltime_line->SetLineWidth(2);
  //  min_alltime_line->SetLineStyle(kDashed);
  min_alltime_line->Draw("LSAME");
  TLine* max_alltime_line = new TLine(max_alltime, hEnergyTime->GetYaxis()->GetXmin(), max_alltime, hEnergyTime->GetYaxis()->GetXmax());
  max_alltime_line->SetLineColor(kBlack);
  max_alltime_line->SetLineWidth(2);
  //  max_alltime_line->SetLineStyle(kDashed);
  max_alltime_line->Draw("LSAME");
  
  TLine* min_intime_line = new TLine(min_intime, hEnergyTime->GetYaxis()->GetXmin(), min_intime, hEnergyTime->GetYaxis()->GetXmax());
  min_intime_line->SetLineColor(kRed);
  min_intime_line->SetLineWidth(2);
  //  min_intime_line->SetLineStyle(kDashed);
  min_intime_line->Draw("LSAME");
  TLine* max_intime_line = new TLine(max_intime, hEnergyTime->GetYaxis()->GetXmin(), max_intime, hEnergyTime->GetYaxis()->GetXmax());
  max_intime_line->SetLineColor(kRed);
  max_intime_line->SetLineWidth(2);
  //  max_intime_line->SetLineStyle(kDashed);
  max_intime_line->Draw("LSAME");

  TLine* min_outtime_line = new TLine(min_outtime, hEnergyTime->GetYaxis()->GetXmin(), min_outtime, hEnergyTime->GetYaxis()->GetXmax());
  min_outtime_line->SetLineColor(kBlue);
  min_outtime_line->SetLineWidth(2);
  min_outtime_line->SetLineStyle(kDashed);
  min_outtime_line->Draw("LSAME");
  TLine* max_outtime_line = new TLine(max_outtime, hEnergyTime->GetYaxis()->GetXmin(), max_outtime, hEnergyTime->GetYaxis()->GetXmax());
  max_outtime_line->SetLineColor(kBlue);
  max_outtime_line->SetLineWidth(2);
  max_outtime_line->SetLineStyle(kDashed);
  max_outtime_line->Draw("LSAME");

  latex->SetTextColor(kRed);
  latex->DrawLatexNDC(0.57, 0.8, "In-Time");

  latex->SetTextColor(kBlue);
  latex->DrawLatexNDC(0.35, 0.5, "Out-of-Time");

  THStack* stack = new THStack();
  stack->Add(hInTime);
  stack->Add(hOutTime);
}
