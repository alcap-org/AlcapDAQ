void SiL3RangeScan_FinalPlot_MuStopPeak() {

  /*
  const int n_runs = 11;
  int run_numbers[n_runs] = {8986, 8987, 8988, 8989, 8990, 8991, 8992, 8993, 8994, 8995, 8996};
  double scale_factors[n_runs] = {1.20, 1.21, 1.22, 1.23, 1.24, 1.25, 1.26, 1.27, 1.28, 1.29, 1.30};
  Int_t colours[n_runs] = {kBlue, kRed, kBlack, kMagenta, kSpring+2, kGray, kYellow, kCyan, kGreen};
  */
  const int n_runs = 3;
  int run_numbers[n_runs] = {8986, 8987, 8988};
  double scale_factors[n_runs] = {1.20, 1.21, 1.22};
  Int_t colours[n_runs] = {kBlue, kRed, kBlack};

  TLegend* leg = new TLegend(0.10,0.55,0.50,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);

  int rebin_factor = 2;
  double min_time = -200;
  double max_time = 200;
  
  std::stringstream filename, histname, histtitle, funcname, leglabel;
  for (int i_run = n_runs-1; i_run >= 0; --i_run) {
    int i_run_number = run_numbers[i_run];
    double i_scale_factor = scale_factors[i_run];
    Int_t i_colour = colours[i_run];
    
    filename.str("");
    filename << "~/data/results/SiL3RangeScan/plots0" << i_run_number << ".root";
    TFile* file = new TFile(filename.str().c_str(), "READ");
    TH2F* hEnergyTime = (TH2F*) file->Get("Target/hEnergyTime");

    histname.str("");
    histname << "h" << i_run_number;
    int min_time_bin = hEnergyTime->GetXaxis()->FindBin(min_time);
    int max_time_bin = hEnergyTime->GetXaxis()->FindBin(max_time)-1;
    TH1F* hEnergy = (TH1F*) hEnergyTime->ProjectionY(histname.str().c_str(), min_time_bin, max_time_bin);
    hEnergy->Rebin(rebin_factor);
    hEnergy->GetXaxis()->SetRangeUser(0, 7500);
    histtitle.str("");
    histtitle << "SiL3RangeScan Dataset, Target Energy Deposits (" << min_time << " ns < t < " << max_time << " ns)";
    hEnergy->SetTitle(histtitle.str().c_str());
    hEnergy->SetYTitle("Raw Count");
    hEnergy->SetStats(false);
    hEnergy->SetLineColor(i_colour);

    funcname.str("");
    funcname << "func_" << i_run_number;
    int max_bin = hEnergy->GetMaximumBin();
    double max_bin_center = hEnergy->GetBinCenter(max_bin);
    double bin_width = hEnergy->GetXaxis()->GetBinWidth(1);
    TF1* func = new TF1(funcname.str().c_str(), "[0]*TMath::Gaus(x, [1], [2])", max_bin_center-4*bin_width, max_bin_center+4*bin_width);
    func->SetParameters(100, max_bin_center, 50);
    //    func->SetParameters(0.01, 500, 50);
    func->SetLineColor(i_colour);
    hEnergy->Fit(func, "RN");

    hEnergy->Draw("HIST SAME");
    func->Draw("LSAME");

    leglabel.str("");
    leglabel << std::fixed << std::setprecision(2) << "SF = " << i_scale_factor << ": Stop Peak = " << std::setprecision(3) << func->GetParameter(1)/1000 << " #pm " <<func->GetParError(1)/1000 << " MeV";
    leg->AddEntry(hEnergy, leglabel.str().c_str(), "L");
  }
  leg->Draw();
}
