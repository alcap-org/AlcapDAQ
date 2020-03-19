void Si16a_FinalPlot_TargetSpectrum_ThickEnergy(std::string outdir = "") {
  
  const int n_runs = 8;
  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743}; // want to do all runs individually
  double scale_factors[n_runs] = {1.035, 0.99, 0.98, 1.01, 1.02, 1.03, 1.04, 1.05};

  // const int n_runs = 1;
  // int run_numbers[n_runs] = {9740};
  // double scale_factors[n_runs] = {1.02};

  const int n_hists = 3;
  std::string histnames[n_hists] = {"Thick_All_TimeSlice-200_200/hRawSpectrum", "Thick_wThin_TimeSlice-200_200/hRawSpectrum", "Thick_wNoThin_TimeSlice-200_200/hRawSpectrum"};
  Int_t colours[n_hists] = {kBlack, kBlue, kRed};
  std::string leglabels[n_hists] = {"all thick hits", "thick hits w/ thin hit (punch-through)", "thick hits w/o thin hit (background)"};
  
  int rebin_factor = 10;
  
  std::stringstream run_str, sf_str, axislabel;
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    TCanvas* c = new TCanvas();
    
    run_str.str("");
    run_str << "0" << run_numbers[i_run];

    sf_str.str("");
    sf_str << scale_factors[i_run];

    std::string infilename = "~/data/results/Si16a/raw_spectra" + run_str.str() + "_newPP.root";    
    TFile* infile = new TFile(infilename.c_str(), "READ");
    if (infile->IsZombie()) {
      std::cout << "ERROR: Can't find file " << infilename << std::endl;
      return;
    }

    TLegend* leg = new TLegend(0.40,0.65,0.75,0.85);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.035);
    leg->SetFillColor(kWhite);

    for (int i_hist = 0; i_hist < n_hists; ++i_hist) {
      std::string histname = histnames[i_hist];
      TH1F* hEnergy = (TH1F*) infile->Get(histname.c_str());
      if (!hEnergy) {
	std::cout << "ERROR: Can't find histogram " << histname << std::endl;
	return;
      }
    
      std::string histtitle = "Si16a Dataset, Run " + run_str.str() + ", SF = " + sf_str.str();
      hEnergy->SetLineColor(colours[i_hist]);
      hEnergy->SetTitle(histtitle.c_str());
      hEnergy->GetXaxis()->SetTitle("Energy [keV]");
      hEnergy->Rebin(rebin_factor);
      hEnergy->GetXaxis()->SetRangeUser(0, 5000);
	    
      axislabel.str("");
      axislabel << "Counts / " << hEnergy->GetXaxis()->GetBinWidth(1) << " keV";
      hEnergy->GetYaxis()->SetTitle(axislabel.str().c_str());
      hEnergy->GetYaxis()->SetTitleOffset(1.3);
      
      hEnergy->Draw("HIST E SAME");

      leg->AddEntry(hEnergy, leglabels[i_hist].c_str(), "l");
    }
    
    leg->Draw();
    //    infile->Close();

    TLatex* latex = new TLatex();
    latex->SetTextAlign(22);
    latex->DrawLatexNDC(0.7, 0.55, "AlCap #bf{#it{Preliminary}}");
    std::string text = "Run " + run_str.str() + ", SF = " + sf_str.str();
    latex->DrawLatexNDC(0.7, 0.5, text.c_str());

    if (outdir != "") {
      std::string savename = outdir + "/AlCapData_Si16aDataset_Run" + run_str.str() + "_TargetSpectrum_ThickEnergy";
      std::string pdfname = savename + ".pdf";
      c->SaveAs(pdfname.c_str());
    }
  }
}
