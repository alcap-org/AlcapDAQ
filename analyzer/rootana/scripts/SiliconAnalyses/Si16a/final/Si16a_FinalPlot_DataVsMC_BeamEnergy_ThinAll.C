void Si16a_FinalPlot_DataVsMC_BeamEnergy_ThinAll(std::string outdir = "") {
  /*  
  const int n_runs = 8;
  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743}; // want to do all runs individually
  double scale_factors[n_runs] = {1.035, 0.99, 0.98, 1.01, 1.02, 1.03, 1.04, 1.05};
  */
  
  const int n_runs = 3;
  int run_numbers[n_runs] = {9740, 9741, 9743}; // want to do all runs individually
  double scale_factors[n_runs] = {1.02, 1.03, 1.05};
  double input_beam_energy[n_runs] = {1.37, 1.46, 1.67};
  
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
  Int_t fill_colours[n_settings] = {kWhite, kRed};
  Int_t marker_styles[n_settings] = {kFullDotLarge, kDot};
  int rebin_factors[n_settings] = {5, 5};
  std::string inhistnames[n_settings] = {"Thin_All_TimeSlice-200_200/hRawSpectrum", "hThinEnergy_noVeto_SiL"};

  std::stringstream run_str, sf_str, axislabel, leglabel, input_beam_str;
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    TLegend* leg = new TLegend(0.55,0.40,0.95,0.60);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.07);
    leg->SetFillStyle(0);
    leg->SetFillColor(kWhite);

    TCanvas* c = new TCanvas();
    run_str.str("");
    run_str << "0" << run_numbers[i_run];

    sf_str.str("");
    //    sf_str << scale_factors[i_run];
    sf_str << std::fixed << std::setprecision(1) << scale_factors[i_run]*25.4;
    //    sf_str << std::fixed << std::setprecision(4) << scale_factors[i_run]*25.4/1000.0;

    input_beam_str.str("");
    input_beam_str << input_beam_energy[i_run];
    
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
      hEnergy->SetMarkerStyle(marker_styles[i_setting]);
      hEnergy->SetMarkerColor(colours[i_setting]);
      hEnergy->SetLineColor(colours[i_setting]);
      hEnergy->SetLineStyle(1);
      hEnergy->SetLineWidth(2);
      //      hEnergy->SetFillStyle(3004);
      //      hEnergy->SetFillColor(fill_colours[i_setting]);
      hEnergy->GetXaxis()->SetTitle("Energy [MeV]");
      hEnergy->Rebin(rebin_factors[i_setting]);
      hEnergy->Scale(1.0 / hEnergy->GetMaximum());
      hEnergy->GetXaxis()->SetRangeUser(0,2500);
      //      hEnergy->GetXaxis()->SetRangeUser(0,1500);

      hEnergy->GetXaxis()->SetTitleSize(0.05);
      hEnergy->GetXaxis()->SetLabelSize(0.05);
      hEnergy->GetYaxis()->SetTitleSize(0.05);
      hEnergy->GetYaxis()->SetLabelSize(0.05);
      hEnergy->GetXaxis()->SetTitleOffset(0.95);
      hEnergy->GetYaxis()->SetTitleOffset(1.0);
      
      axislabel.str("");
      //      axislabel << "Counts / " << hEnergy->GetXaxis()->GetBinWidth(1) << " keV";
      axislabel << "Peak Normalized / " << hEnergy->GetXaxis()->GetBinWidth(1) / 1e3 << " MeV";
      hEnergy->GetYaxis()->SetTitle(axislabel.str().c_str());

      // Change labels for x-axis
      int n_labels = 6;
      std::stringstream new_label;
      for (int i_label = 1; i_label <= n_labels; ++i_label) {
	new_label.str("");
	if (i_label %2 == 0) {
	  new_label << std::fixed << std::setprecision(1);
	}
	new_label << (i_label-1)/2.0; // convert to MeV labels
	hEnergy->GetXaxis()->ChangeLabel(i_label, -1, -1, -1, -1, -1, new_label.str().c_str());
      }

      
      hEnergy->Draw("HIST E SAME");

      leglabel.str("");
      leglabel << leglabels[i_setting];
      leg->AddEntry(hEnergy, leglabel.str().c_str(), "lp");

      double maximum = hEnergy->GetMaximum();
      double x1 = hEnergy->GetXaxis()->GetBinCenter(hEnergy->FindFirstBinAbove(maximum/2));
      double x2 = hEnergy->GetXaxis()->GetBinCenter(hEnergy->FindLastBinAbove(maximum/2));
      double fwhm = x2 - x1;
      double peak_pos = hEnergy->GetXaxis()->GetBinCenter(hEnergy->GetMaximumBin());
      std::cout << run_str.str() << " " << leglabel.str() << ": FWHM = " << fwhm << std::endl;
      std::cout << run_str.str() << " " << leglabel.str() << ": Mean = " << peak_pos << std::endl;

      //      hEnergy->Fit("gaus", "", "", x1, x2);
      //      hEnergy->GetFunction("gaus")->SetLineColor(colours[i_setting]);
      //      hEnergy->GetFunction("gaus")->Draw("LSAME");
    }
    leg->Draw();

    TLatex* latex = new TLatex();
    latex->SetTextSize(0.07);
    latex->SetTextAlign(22);
    //    latex->DrawLatexNDC(0.7, 0.55, "AlCap #bf{#it{Preliminary}}");
    //    std::string text = "Run " + run_str.str() + ", SF = " + sf_str.str();
    std::string text = "p_{beam} = " + sf_str.str() + " MeV/c";
    latex->DrawLatexNDC(0.65, 0.78, text.c_str());

    text = "E_{MC} = " + input_beam_str.str() + " MeV";
    latex->DrawLatexNDC(0.65, 0.68, text.c_str());

    if (outdir != "") {
      std::string savename = outdir + "/AlCapDataVsMC_Si16aDataset_Run" + run_str.str() + "_BeamEnergy_ThinAll";
      std::string pdfname = savename + ".pdf";
      c->SaveAs(pdfname.c_str());
    }
  }
}
