void BeamPos_FinalPlot_DataVsMC_BeamEnergy() {

  //  const int n_runs = 3;
  //  int run_numbers[n_runs] = {10473, 10477, 10404}; // want to do all runs individually
  //  double scale_factors[n_runs] = {1.05, 1.02, 1.02};

  const int n_runs = 3;
  int run_numbers[n_runs] = {10404, 10477, 10473};
  double scale_factors[n_runs] = {1.02, 1.02, 1.05};

  const int n_settings = 2;
  std::string prefilenames[n_settings] = {"~/data/results/BeamPos/raw_spectra", "~/data/mc/BeamPos/MC_raw_spectra"};
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
  std::string plot_names[n_plots] = {"SiT", "all thin hits"};
  std::string xaxis_labels[n_plots] = {"SiT Energy [keV]", "SiL1 Energy [keV]"};

  int rebin_factors[n_plots][n_settings] = { {2, 2},
					     {5, 5}
  };
  const char* inhistnames[n_plots][n_settings] = { { "SiT_All_TimeSlice-200_200/hRawSpectrum", "hThinEnergy_noVeto_SiT" },
  						   { "Thin_All_TimeSlice-200_200/hRawSpectrum", "hThinEnergy_noVeto_SiL" }
  };

  std::stringstream run_str, sf_str, axislabel;
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    std::cout << "Run " << run_numbers[i_run] << std::endl;
    run_str.str("");
    run_str << run_numbers[i_run];

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
	histtitle = "BeamPos Dataset, Run " + run_str.str() + ", SF = " + sf_str.str() + " (" + plot_names[i_plot] + ")";
	hEnergy->SetTitle(histtitle.c_str());
	hEnergy->Sumw2();
	hEnergy->SetStats(false);
	hEnergy->SetMarkerColor(colours[i_setting]);
	hEnergy->SetLineColor(colours[i_setting]);
	hEnergy->SetLineStyle(1);
	hEnergy->GetXaxis()->SetTitle(xaxis_labels[i_plot].c_str());
	hEnergy->Rebin(rebin_factors[i_plot][i_setting]);
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
