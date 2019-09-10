void Si16a_FinalPlot_DataVsMC_BeamEnergy() {

  //  const int n_runs = 1;
  //  int run_numbers[n_runs] = {9742}; // want to do all runs individually
  //  double scale_factors[n_runs] = {1.04};

  const int n_runs = 4;
  int run_numbers[n_runs] = {9740, 9741, 9742, 9743}; // want to do all runs individually
  double scale_factors[n_runs] = {1.02, 1.03, 1.04, 1.05};

  const int n_settings = 2;
  std::string prefilenames[n_settings] = {"~/data/results/Si16a/raw_spectra", "~/data/mc/Si16a/MC_raw_spectra"};
  std::string postfilenames[n_settings] = {"_newPP.root", ".root"};
  std::string leglabels[n_settings] = {"Data", "MC"};
  Int_t colours[n_settings] = {kBlack, kRed};
  
  TLegend* leg = new TLegend(0.40,0.45,0.80,0.75);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    TH1F* h = new TH1F("h", "", 1,0,1);
    h->SetLineColor(colours[i_setting]);
    leg->AddEntry(h, leglabels[i_setting].c_str(), "l");
  }

  //  const int n_plots = 6;
  const int n_plots = 3;
  std::string plot_names[n_plots] = {"SiT", "all thin hits",
				     //"thin hits with no thick", "thick hits with thin hit", "thick hits with thin hit",
				     "all thick hits"};
  std::string xaxis_labels[n_plots] = {"SiT Energy [keV]", "SiL1 Energy [keV]",
				       //"SiL1 Energy [keV]", "SiL3 Energy [keV]", "SiL1 Energy + SiL3 Energy [keV]",
				       "SiL3 Energy [keV]"};

  int rebin_factors[n_plots][n_settings] = { {2, 2},
					     {5, 5},
					     //{5, 5},
					     //{2, 4},
					     //{1, 2},
					     {10, 10}
  };
  const char* inhistnames[n_plots][n_settings] = { { "SiT_All_TimeSlice-200_200/hRawSpectrum", "hThinEnergy_noVeto_SiT" },
  						   { "Thin_All_TimeSlice-200_200/hRawSpectrum", "hThinEnergy_noVeto_SiL" },
						   //{ "Thin_wNoThick_TimeSlice-200_200/hRawSpectrum", "hThinEnergy_Veto_SiL" },
						   //{ "muonbeam_thickStop_E2only/hRawSpectrum", "hThickEnergy_Veto_SiL" },
						   //{ "muonbeam_thickStop/hRawSpectrum", "hEvdE_SiL_px" },
						   { "Thick_All_TimeSlice-200_200/hRawSpectrum", "hThickEnergy_noVeto_SiL" },
  };

  std::stringstream run_str, sf_str, axislabel;
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    std::cout << "Run " << run_numbers[i_run] << std::endl;
    run_str.str("");
    run_str << "0" << run_numbers[i_run];

    sf_str.str("");
    sf_str << scale_factors[i_run];

    for (int i_plot = 0; i_plot < n_plots; ++i_plot) {
      TCanvas* c = new TCanvas();
      std::cout << "New Canvas for plot " << plot_names[i_plot] << std::endl;
      for (int i_setting = 0; i_setting < n_settings; ++i_setting) {

	std::string infilename = prefilenames[i_setting] + run_str.str() + postfilenames[i_setting];
	TFile* infile = new TFile(infilename.c_str(), "READ");
	if (infile->IsZombie()) {
	  std::cout << "ERROR: Can't find file " << infilename << std::endl;
	  return;
	}

	std::string inhistname = inhistnames[i_plot][i_setting];
	TH1F* hEnergy = (TH1F*) infile->Get(inhistname.c_str());
	if (!hEnergy) {
	  std::cout << "ERROR: Can't find histogram " << inhistname << std::endl;
	  return;
	}

	std::string histtitle;
	histtitle = "Si16a Dataset, Run " + run_str.str() + ", SF = " + sf_str.str() + " (" + plot_names[i_plot] + ")";
	hEnergy->SetTitle(histtitle.c_str());
	hEnergy->Sumw2();
	hEnergy->SetStats(false);
	hEnergy->SetMarkerColor(colours[i_setting]);
	hEnergy->SetLineColor(colours[i_setting]);
	hEnergy->SetLineStyle(1);
	hEnergy->GetXaxis()->SetTitle(xaxis_labels[i_plot].c_str());
	hEnergy->Rebin(rebin_factors[i_plot][i_setting]);

	double min_integral = 0;
	double max_integral = 1000;
	int min_integral_bin = hEnergy->GetXaxis()->FindBin(min_integral);
	int max_integral_bin = hEnergy->GetXaxis()->FindBin(max_integral);
	std::cout << leglabels[i_setting] << " Entries = " << hEnergy->Integral(min_integral_bin, max_integral_bin) << std::endl;

	hEnergy->Scale(1.0 / hEnergy->GetMaximum());
	hEnergy->GetXaxis()->SetRangeUser(0,5000);

	
	axislabel.str("");
	axislabel << "Counts / " << hEnergy->GetXaxis()->GetBinWidth(1) << " keV";
	hEnergy->GetYaxis()->SetTitle(axislabel.str().c_str());

	hEnergy->Draw("HIST E SAME");
      }
      leg->Draw();
    }
  }
}
