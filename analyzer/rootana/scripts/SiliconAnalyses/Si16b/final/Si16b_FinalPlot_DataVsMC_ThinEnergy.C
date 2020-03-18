void Si16b_FinalPlot_DataVsMC_ThinEnergy(std::string savedir = "") {
  /*  
  const int n_runs = 8;
  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743}; // want to do all runs individually
  double scale_factors[n_runs] = {1.035, 0.99, 0.98, 1.01, 1.02, 1.03, 1.04, 1.05};
  */
  const int n_runs = 1;
  int run_numbers[n_runs] = {10404}; // want to do all runs individually
  double scale_factors[n_runs] = {1.02};

  const int n_settings = 4;
  //  std::string histnames[n_settings] = {"Thin_wNoThick_TimeSlice-200_200/hRawSpectrum", "muonbeam/hRawSpectrum"};
  //  std::string histnames[n_settings] = {"muonbeam_thickStop/hRawSpectrum", "muonbeam_thinStop/hRawSpectrum"};
  std::string filenames[n_settings] = {"~/data/results/Si16b/raw_spectra_newPP_geq1TgtPulse.root",
				       "~/data/mc/Si16b/MC_Si16b_BeamEDep.root",
				       "~/data/mc/Si16b/MC_Si16b_BeamEDep_higherE.root",
				       "~/data/mc/Si16b/MC_Si16b_BeamEDep_lowerE.root"};
  //				       "~/data/mc/BeamPos/MC_raw_spectra10404.root"};
  std::string leglabels[n_settings] = {"Data", "MC (nominal)", "MC (higher E)", "MC (lower E)"};
  Int_t colours[n_settings] = {kBlack, kRed, kBlue, kMagenta};
  //  int rebin_factors[n_settings] = {5, 5};
  //  std::string inhistnames[n_settings] = { "Thin_All_TimeSlice-200_200/hRawSpectrum", "hThinEnergy_noVeto_SiL"};
  int rebin_factors[n_settings] = {1, 1, 1, 1};
  std::string inhistnames[n_settings] = { "TargetMuons/hRawSpectrum", "hEDep_beam", "hEDep_beam", "hEDep_beam"};

  TLegend* leg = new TLegend(0.40,0.45,0.80,0.75);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);

  TCanvas* c = new TCanvas();
  
  std::stringstream run_str, sf_str, axislabel, leglabel;
  for (int i_run = 0; i_run < n_runs; ++i_run) {
  //    TCanvas* c = new TCanvas();
    run_str.str("");
    run_str << "0" << run_numbers[i_run];

    sf_str.str("");
    sf_str << scale_factors[i_run];

    for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
      std::string infilename = filenames[i_setting];
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
      histtitle = "Si16b Dataset, SF = " + sf_str.str();
      hEnergy->SetTitle(histtitle.c_str());
      hEnergy->Sumw2();
      hEnergy->SetStats(false);
      hEnergy->SetMarkerColor(colours[i_setting]);
      hEnergy->SetLineColor(colours[i_setting]);
      hEnergy->SetLineStyle(1);
      hEnergy->SetLineWidth(2);
      hEnergy->GetXaxis()->SetTitle("Energy [keV]");
      hEnergy->Rebin(rebin_factors[i_setting]);
      //      hEnergy->Scale(1.0 / hEnergy->Integral());
      hEnergy->Scale(1.0 / hEnergy->GetMaximum());
      hEnergy->GetXaxis()->SetRangeUser(0,5000);
      hEnergy->GetXaxis()->SetTitleOffset(0.9);
      
      axislabel.str("");
      axislabel << "Peak Normalised / " << hEnergy->GetXaxis()->GetBinWidth(1) << " keV";
      hEnergy->GetYaxis()->SetTitle(axislabel.str().c_str());
      hEnergy->GetYaxis()->SetTitleOffset(0.9);

      hEnergy->Draw("HIST E SAME");

      alcaphistogram(hEnergy);
      if (i_setting == 0) {
	hEnergy->SetDrawOption("HIST E1");
	alcapPreliminary(hEnergy);
      }
      else {
	hEnergy->SetDrawOption("HIST E1 SAME");
      }

      leglabel.str("");
      leglabel << leglabels[i_setting];
      leg->AddEntry(hEnergy, leglabel.str().c_str(), "l");
    }
    leg->Draw();
  }

  if (savedir != "") {
    std::string savename = savedir + "AlCapDataVsMC_Si16bDataset_ThinEnergy_MuonStopSystematic";
    std::string pdfname = savename + ".pdf";
    c->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c->SaveAs(pngname.c_str());
  }
}
