void SiL3_FinalPlot_DataVsMC_BeamEnergy(std::string savedir = "") {

  const int n_runs = 1;
  int run_numbers[n_runs] = {9040}; // want to do all runs individually
  double scale_factors[n_runs] = {1.12};

  const int n_settings = 4;
  std::string prefilenames[n_settings] = {"~/data/results/SiL3/raw_spectra", "~/data/mc/SiL3/MC_raw_spectra", "~/data/mc/SiL3/MC_raw_spectra", "~/data/mc/SiL3/MC_raw_spectra"};
  //  std::string postfilenames[n_settings] = {"_geq0TgtPulse_newPP20us.root", ".root"};
  std::string postfilenames[n_settings] = {"_geq1TgtPulse_newPP20us.root", "09040_nominal.root", "09040_lowerE.root", "09040_higherE.root"};
  std::string leglabels[n_settings] = {"Data", "MC (nominal)", "MC (lowerE)", "MC (higherE)"};
  Int_t colours[n_settings] = {kBlack, kRed, kBlue, kMagenta};
  
  TLegend* leg = new TLegend(0.20,0.45,0.40,0.75);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    TH1F* h = new TH1F("h", "", 1,0,1);
    h->SetLineColor(colours[i_setting]);
    leg->AddEntry(h, leglabels[i_setting].c_str(), "l");
  }

  const int n_plots = 1;
  std::string plot_names[n_plots] = {"all thick hits"};
  std::string xaxis_labels[n_plots] = {"SiL3 Energy [keV]"};

  //  int rebin_factors[n_plots][n_settings] = { {1, 2}  };
  //  const char* inhistnames[n_plots][n_settings] = { { "SiL3_ActiveTarget_TimeSlice-200_200/hRawSpectrum", "hThickEnergy_noVeto_SiL" }  };
  int rebin_factors[n_plots][n_settings] = { {1, 1, 1, 1}  };
  const char* inhistnames[n_plots][n_settings] = { { "SiL3_ActiveTarget_TimeSlice-200_200/hRawSpectrum", "hEDep_beam", "hEDep_beam", "hEDep_beam" }  };

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

	//	std::string infilename = prefilenames[i_setting] + run_str.str() + postfilenames[i_setting];
	std::string infilename = prefilenames[i_setting] + postfilenames[i_setting];
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
	histtitle = "SiL3 Dataset, Run " + run_str.str() + ", SF = " + sf_str.str() + " (" + plot_names[i_plot] + ")";
	hEnergy->SetTitle(histtitle.c_str());
	hEnergy->Sumw2();
	hEnergy->SetStats(false);
	hEnergy->SetMarkerColor(colours[i_setting]);
	hEnergy->SetLineColor(colours[i_setting]);
	hEnergy->SetLineStyle(1);
	hEnergy->GetXaxis()->SetTitle(xaxis_labels[i_plot].c_str());
	hEnergy->GetXaxis()->SetTitleOffset(0.9);
	hEnergy->GetYaxis()->SetTitleOffset(0.9);
	hEnergy->Rebin(rebin_factors[i_plot][i_setting]);
	hEnergy->Scale(1.0 / hEnergy->GetMaximum());
	hEnergy->GetXaxis()->SetRangeUser(0,7500);
      
	axislabel.str("");
	axislabel << "Counts / " << hEnergy->GetXaxis()->GetBinWidth(1) << " keV";
	hEnergy->GetYaxis()->SetTitle(axislabel.str().c_str());

	hEnergy->Draw("HIST E SAME");
      }
      leg->Draw();

      if (savedir != "") {
	std::string savename = savedir + "AlCapDataVsMC_SiL3Dataset_BeamEnergy_MuonStopSystematic";
	std::string pdfname = savename + ".pdf";
	c->SaveAs(pdfname.c_str());
	std::string pngname = savename + ".png";
	c->SaveAs(pngname.c_str());
      }
    }
  }
}
