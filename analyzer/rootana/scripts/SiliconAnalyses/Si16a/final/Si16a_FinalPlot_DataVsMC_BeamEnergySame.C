void Si16a_FinalPlot_DataVsMC_BeamEnergySame() {

   const int n_runs = 1;
   int run_numbers[n_runs] = {9740}; // want to do all runs individually
   double scale_factors[n_runs] = {1.02};

  // const int n_runs = 4;
  // int run_numbers[n_runs] = {9740, 9741, 9742, 9743}; // want to do all runs individually
  // double scale_factors[n_runs] = {1.02, 1.03, 1.04, 1.05};

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

  const int n_plots = 2;
  std::string plot_names[n_plots] = {"all thin hits", "all thick hits"};
  std::string xaxis_labels[n_plots] = {"SiL1 Energy [keV]", "SiL3 Energy [keV]"};
  
  int rebin_factors[n_plots][n_settings] = { {5, 5},
					     {5, 5}
  };
  const char* inhistnames[n_plots][n_settings] = { { "Thin_All_TimeSlice-200_200/hRawSpectrum", "hThinEnergy_noVeto_SiL" },
  						   { "Thick_All_TimeSlice-200_200/hRawSpectrum", "hThickEnergy_noVeto_SiL" }
  };
  // const char* inhistnames[n_plots][n_settings] = { { "Thin_wNoThick_TimeSlice-200_200/hRawSpectrum", "hThinEnergy_Veto_SiL" },
  // 						   { "Thick_wThin_TimeSlice-200_200/hRawSpectrum", "hThickEnergy_noVeto_SiL" }
  // };
  double scales[n_settings] = {-1, -1};
  int n_entries[n_plots][n_settings] = { {0, 0}, {0, 0} };

  std::stringstream run_str, sf_str, axislabel, text;
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    // reset scales
    for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
      scales[i_setting] = -1;
    }
    std::cout << "Run " << run_numbers[i_run] << std::endl;
    run_str.str("");
    run_str << "0" << run_numbers[i_run];

    sf_str.str("");
    sf_str << scale_factors[i_run];

    TCanvas* c = new TCanvas();
    for (int i_plot = 0; i_plot < n_plots; ++i_plot) {
    //    for (int i_plot = n_plots-1; i_plot >= 0; --i_plot) {

      std::cout << "Plot " << plot_names[i_plot] << std::endl;
      for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
      //      for (int i_setting = n_settings-1; i_setting >=0; --i_setting) {

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
	histtitle = "Si16a Dataset, Run " + run_str.str() + ", SF = " + sf_str.str();
	hEnergy->SetTitle(histtitle.c_str());
	hEnergy->Sumw2();
	hEnergy->SetStats(false);
	hEnergy->SetMarkerColor(colours[i_setting]);
	hEnergy->SetLineColor(colours[i_setting]);
	hEnergy->SetLineStyle(1);
	hEnergy->GetXaxis()->SetTitle("Energy [keV]");
	hEnergy->Rebin(rebin_factors[i_plot][i_setting]);

	double min_integral = 500;
	double max_integral = 1000;
	int min_integral_bin = hEnergy->GetXaxis()->FindBin(min_integral);
	int max_integral_bin = hEnergy->GetXaxis()->FindBin(max_integral);
	std::cout << leglabels[i_setting] << " Integral = " << hEnergy->Integral(min_integral_bin, max_integral_bin) << std::endl;
	std::cout << leglabels[i_setting] << " Entries = " << hEnergy->GetEntries() << std::endl;
	std::cout << hEnergy->GetMaximum() << std::endl;
	if (scales[i_setting] < 0) {
	  scales[i_setting] = hEnergy->GetMaximum();
	  //	  scales[i_setting] = hEnergy->GetEntries();
	}
	std::cout << "AE: " << scales[i_setting] << std::endl;
	hEnergy->Scale(1.0 / scales[i_setting]);
	hEnergy->GetXaxis()->SetRangeUser(0,5000);

	
	axislabel.str("");
	axislabel << "Counts / " << hEnergy->GetXaxis()->GetBinWidth(1) << " keV";
	hEnergy->GetYaxis()->SetTitle(axislabel.str().c_str());

	hEnergy->Draw("HIST E SAME");

	n_entries[i_plot][i_setting] = hEnergy->GetEntries();

	if (i_plot == n_plots-1 && i_setting == n_settings-1) {
	  TLatex* latex = new TLatex();
	  latex->SetTextAlign(22);
	  latex->SetTextSize(0.035);
	  int data_SiL3 = n_entries[1][0];
	  int data_SiL1 = n_entries[0][0];
	  double data_ratio = (double)data_SiL3 / data_SiL1;
	  text.str("");
	  text << "Data: SiL3 Entries / SiL1 Entries = " << data_SiL3 << " / " << data_SiL1 << " = " << std::fixed << std::setprecision(2) << data_ratio;
	  latex->DrawLatexNDC(0.6, 0.7, text.str().c_str());

	  int mc_SiL3 = n_entries[1][1];
	  int mc_SiL1 = n_entries[0][1];
	  double mc_ratio = (double)mc_SiL3 / mc_SiL1;
	  text.str("");
	  text << "MC: SiL3 Entries / SiL1 Entries = " << mc_SiL3 << " / " << mc_SiL1 << " = " << std::fixed << std::setprecision(2) << mc_ratio;
	  latex->SetTextColor(kRed);
	  latex->DrawLatexNDC(0.6, 0.6, text.str().c_str());

	  double mc_data_ratio = mc_ratio / data_ratio;
	  text.str("");
	  text << "MC / data = " << mc_data_ratio;
	  latex->SetTextColor(kBlack);
	  latex->DrawLatexNDC(0.6, 0.5, text.str().c_str());
	}
      }
      //      leg->Draw();
    }
  }
}
