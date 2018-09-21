void EfficiencyComparisons() {

  const int n_runs = 3;
  int run_numbers[n_runs] = {9302, 9685, 10319};//, 9282, 9684, 9686};
  Int_t colours[n_runs] = {kBlack, kRed, kBlue};//, kGray+1, kMagenta, kCyan};
  bool absolute_eff_run[n_runs] = {true, true, true};//, false, false, false};
  double geometric_acceptance = 1;//2.8e-3;
  TGraphErrors* gres[n_runs];
  TGraphErrors* gre_intervals[n_runs];
  double effs[n_runs];
  double eff_errs[n_runs];

  const int n_useful_energies = 6;
  double useful_energies[n_useful_energies] = {400.177, 476.8, 346.828, 412.87, 931, 1122};
  double energy_2p1s = 477;
  //  double energy_2p1s = 347;
    
  TLegend* leg = new TLegend(0.25,0.55,0.65,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.04);
  leg->SetFillColor(kWhite);

  std::stringstream basename, leglabel;
  std::string ge_channel = "GeHiGain";
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    basename.str("");
    basename << ge_channel << "_" << run_numbers[i_run];

    //    std::string filename = "noCorrections/" + basename.str() + ".root";
    //    std::string filename = "tBlock96ms_" + ge_channel + "/" + basename.str() + ".root";
    std::string filename = "tBlock96ms_wExtraDeadTime_" + ge_channel + "/" + basename.str() + ".root";
    //    std::string filename = basename.str() + ".root";
    TFile* file = new TFile(filename.c_str(), "READ");

    std::string foldername = basename.str(); // just happens to be the same
    std::string histname = foldername + "/hEffGr";
    TGraphErrors* gre = (TGraphErrors*) file->Get(histname.c_str());

    gre->SetTitle("^{152}Eu Runs");
    gre->SetLineColor(colours[i_run]);
    gre->SetLineWidth(2);
    gre->SetMarkerColor(colours[i_run]);

    TF1* eff = gre->GetFunction("effcal");
    eff->SetLineColor(colours[i_run]);

    if (!absolute_eff_run[i_run]) {
      for (int i_point = 0; i_point < gre->GetN(); ++i_point) {
	gre->SetPoint(i_point, *(gre->GetX()+i_point), *(gre->GetY()+i_point)*geometric_acceptance);
	gre->SetPointError(i_point, *(gre->GetEX()+i_point), *(gre->GetEY()+i_point)*geometric_acceptance);
      }
      eff->SetParameter(0, eff->GetParameter(0)*geometric_acceptance);
    }

    histname = foldername + "/hEffGrIntervals";
    TGraphErrors* gre_interval = (TGraphErrors*) file->Get(histname.c_str());
    gre_interval->SetFillColor(colours[i_run]-9);
    gre_interval->SetFillStyle(3001);
    
    if (i_run == 0) {
      gre_interval->SetFillColor(kGray);
      gre->Draw("APE");
    }
    //    else {
    gre_interval->Draw("4 SAME");
    gre->SetFillColor(gre_interval->GetFillColor());
    gre->SetFillStyle(gre_interval->GetFillStyle());
    gre->Draw("PE");
    //    }

    int n_points = gre_interval->GetN();
    double eu_err = 0;
    for (int i_point = 0; i_point < n_points; ++i_point) {
      double x, y;
      gre_interval->GetPoint(i_point, x, y);
      if (x == energy_2p1s) {
	eu_err = gre_interval->GetEY()[i_point];
	break;
      }
    }

    leglabel.str("");
    leglabel << "Run " << run_numbers[i_run] << ", #varepsilon(" << std::fixed << std::setprecision(0) << energy_2p1s << " keV) = (" << std::setprecision(1) << eff->Eval(energy_2p1s)*1e4 << " #pm " << std::setprecision(1) << eu_err*1e4 << ") #times 10^{-4}";

    leg->AddEntry(gre, leglabel.str().c_str(), "lpf");

    gres[i_run] = gre;
    gre_intervals[i_run] = gre_interval;
    effs[i_run] = eff->Eval(energy_2p1s);
    eff_errs[i_run] = eu_err;

    file->Close();
  }

  // Plot the intervals first and then the points on top
  gres[0]->Draw("APE");
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    gre_intervals[i_run]->Draw("4 SAME");
  }
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    gres[i_run]->Draw("PE SAME");
  }

  
  double mean_eu_eff;
  double mean_eu_eff_err;
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    mean_eu_eff += effs[i_run];
    mean_eu_eff_err += (eff_errs[i_run]*eff_errs[i_run]);
  }
  mean_eu_eff /= n_runs;
  mean_eu_eff_err = sqrt(mean_eu_eff_err)/n_runs;
  //  mean_eu_eff_err /= sqrt(n_runs);

  TGraphErrors* gre_mean_eu = new TGraphErrors(1, &energy_2p1s, &mean_eu_eff, NULL, &mean_eu_eff_err);
  gre_mean_eu->SetMarkerStyle(kFullStar);
  gre_mean_eu->SetMarkerColor(kSpring-6);
  gre_mean_eu->SetMarkerSize(1.8);
  gre_mean_eu->SetLineColor(kSpring-6);
  gre_mean_eu->SetLineWidth(2);
  gre_mean_eu->Draw("PE SAME");
  leglabel.str("");
  leglabel << "Mean Eu Runs, #varepsilon(" << std::fixed << std::setprecision(0) << energy_2p1s << " keV) = (" << std::setprecision(1) << mean_eu_eff*1e4 << " #pm " << std::setprecision(1) << mean_eu_eff_err*1e4 << ") #times 10^{-4}";
  std::cout << leglabel.str() << std::endl;
  leg->AddEntry(gre_mean_eu, leglabel.str().c_str(), "p");


  //  double active_energy = 400;
  //  double active_eff = 5.88e-4; // GeLoGain
  //  double active_eff_err = 0.09e-4;
  //  double active_eff = 6.14e-4; // GeHiGain
  //  double active_eff_err = 0.09e-4;
  
  double active_energy = 476.8;
  double active_eff = 4.80e-4; //GeLoGain
  double active_eff_err = 0.25e-4;
  //  double active_eff = 5.03e-4; //GeHiGain
  //  double active_eff_err = 0.24e-4;
  
  //  double active_energy = 503.58;
  //  double active_eff = 3.87e-4;
  //  double active_eff_err = 0.45e-4;
  TGraphErrors* gre_active = new TGraphErrors(1, &active_energy, &active_eff, NULL, &active_eff_err);
  gre_active->SetMarkerStyle(kFullStar);
  gre_active->SetMarkerColor(kMagenta+2);
  gre_active->SetMarkerSize(1.8);
  gre_active->SetLineColor(kMagenta+2);
  gre_active->SetLineWidth(2);
  gre_active->Draw("PE SAME");
  leglabel.str("");
  leglabel << "SiL3 Active Target, #varepsilon(" << std::fixed << std::setprecision(0) << active_energy << " keV) = (" << std::setprecision(2) << active_eff*1e4 << " #pm " << active_eff_err*1e4 << ") #times 10^{-4}";
  leg->AddEntry(gre_active, leglabel.str().c_str(), "p");

  leg->Draw();
}
