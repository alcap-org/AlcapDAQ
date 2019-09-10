void Si16a_FinalPlot_DataVsMC_BeamEnergy_ThickStop() {
  /*  
  const int n_runs = 8;
  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743}; // want to do all runs individually
  double scale_factors[n_runs] = {1.035, 0.99, 0.98, 1.01, 1.02, 1.03, 1.04, 1.05};
  TH1F* hSums[n_runs] = {0};
  double means[n_runs] = {0};
  double mean_errors[n_runs] = {0};
  double sigmas[n_runs] = {0};
  double sigma_errors[n_runs] = {0};
  */
  const int n_runs = 3;
  int run_numbers[n_runs] = {9740, 9741, 9743}; // want to do all runs individually
  double scale_factors[n_runs] = {1.02, 1.03, 1.05};

  const int n_settings = 2;
  //  std::string histnames[n_settings] = {"Thin_wNoThick_TimeSlice-200_200/hRawSpectrum", "muonbeam/hRawSpectrum"};
  //  std::string histnames[n_settings] = {"muonbeam_thickStop/hRawSpectrum", "muonbeam_thinStop/hRawSpectrum"};
  std::string prefilenames[n_settings] = {"~/data/results/Si16a/fits", "~/data/mc/Si16a/MC_fits"};
  std::string postfilenames[n_settings] = {"_newPP.root",".root"};
  std::string leglabels[n_settings] = {"Data", "MC"};
  Int_t colours[n_settings] = {kBlack, kRed};
  int rebin_factors[n_settings] = {1, 2};
  std::string inhistname = "GausFit_ThickStop/hFittedSpectrum";
  std::string infitname = "GausFit_ThickStop/gaus_fit";

  std::stringstream run_str, sf_str, axislabel, leglabel;
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    TLegend* leg = new TLegend(0.40,0.45,0.80,0.75);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.03);
    leg->SetFillStyle(0);
    leg->SetFillColor(kWhite);

    TCanvas* c = new TCanvas();
    run_str.str("");
    run_str << "0" << run_numbers[i_run];

    sf_str.str("");
    sf_str << scale_factors[i_run];

    for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
      std::string infilename = prefilenames[i_setting] + run_str.str() + postfilenames[i_setting];
      TFile* infile = new TFile(infilename.c_str(), "READ");
      if (infile->IsZombie()) {
	std::cout << "ERROR: Can't find file " << infilename << std::endl;
	return;
      }
      
      TH1F* hEnergy = (TH1F*) infile->Get(inhistname.c_str());
      if (!hEnergy) {
	std::cout << "ERROR: Can't find histogram " << inhistname << std::endl;
	return;
      }
      
      hEnergy->Sumw2();
      hEnergy->SetStats(false);
      hEnergy->SetMarkerColor(colours[i_setting]);
      hEnergy->SetLineColor(colours[i_setting]);
      hEnergy->SetLineStyle(1);
      hEnergy->GetXaxis()->SetTitle("Energy [keV]");
      hEnergy->Rebin(rebin_factors[i_setting]);
      hEnergy->Scale(1.0 / hEnergy->GetMaximum());
      hEnergy->GetXaxis()->SetRangeUser(0,5000);
      
      axislabel.str("");
      axislabel << "Counts / " << hEnergy->GetXaxis()->GetBinWidth(1) << " keV";
      hEnergy->GetYaxis()->SetTitle(axislabel.str().c_str());
      
      hEnergy->Draw("HIST E SAME");
      
      TF1* fit = (TF1*) infile->Get(infitname.c_str());
      fit->SetLineColor(colours[i_setting]);
      fit->Draw("LSAME");
      
      leglabel.str("");
      leglabel << leglabels[i_setting] << " #left(Fit: #splitline{Mean = " << std::fixed << std::setprecision(3) << fit->GetParameter(1)/1e3 << " #pm " << fit->GetParError(1)/1e3 << " MeV}{Sigma = " << fit->GetParameter(2)/1e3 << " #pm " << fit->GetParError(2)/1e3 << " MeV}#right)";
      leg->AddEntry(hEnergy, leglabel.str().c_str(), "l");
    }
    leg->Draw();
  }
}
