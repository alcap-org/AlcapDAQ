void Si16a_FinalPlot_DataVsMC_BeamEnergy_ThinStop() {
  /*  
  const int n_runs = 8;
  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743}; // want to do all runs individually
  double scale_factors[n_runs] = {1.035, 0.99, 0.98, 1.01, 1.02, 1.03, 1.04, 1.05};
  */
  
  const int n_runs = 3;
  int run_numbers[n_runs] = {9740, 9741, 9743}; // want to do all runs individually
  double scale_factors[n_runs] = {1.02, 1.03, 1.05};
  
  // const int n_runs = 1;
  // int run_numbers[n_runs] = {9740}; // want to do all runs individually
  // double scale_factors[n_runs] = {1.02};

  const int n_settings = 2;
  //  std::string histnames[n_settings] = {"Thin_wNoThick_TimeSlice-200_200/hRawSpectrum", "muonbeam/hRawSpectrum"};
  //  std::string histnames[n_settings] = {"muonbeam_thickStop/hRawSpectrum", "muonbeam_thinStop/hRawSpectrum"};
  std::string prefilenames[n_settings] = {"~/data/results/Si16a/raw_spectra", "~/data/mc/Si16a/MC_raw_spectra"};
  std::string postfilenames[n_settings] = {"_newPP.root", ".root"};
  std::string leglabels[n_settings] = {"Data", "MC"};
  Int_t colours[n_settings] = {kBlack, kRed};
  int rebin_factors[n_settings] = {5, 5};
  std::string inhistnames[n_settings] = {"Thin_wNoThick_TimeSlice-200_200/hRawSpectrum", "hThinEnergy_Veto_SiL"};

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

      std::string inhistname = inhistnames[i_setting];
      TH1F* hEnergy = (TH1F*) infile->Get(inhistname.c_str());
      if (!hEnergy) {
	std::cout << "ERROR: Can't find histogram " << inhistname << std::endl;
	return;
      }

      std::string histtitle;
      histtitle = "Si16a Dataset, Run " + run_str.str() + ", SF = " + sf_str.str();
      hEnergy->SetTitle(histtitle.c_str());
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

      leglabel.str("");
      leglabel << leglabels[i_setting];
      leg->AddEntry(hEnergy, leglabel.str().c_str(), "l");
    }
    leg->Draw();
  }
}
