void Si16a_FinalPlot_TargetSpectrum_ThickEnergy() {
  
  const int n_runs = 8;
  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743}; // want to do all runs individually
  double scale_factors[n_runs] = {1.035, 0.99, 0.98, 1.01, 1.02, 1.03, 1.04, 1.05};

  const int n_hists = 3;
  std::string histnames[n_hists] = {"Thick_All_TimeSlice-200_200/hRawSpectrum", "Thick_wThin_TimeSlice-200_200/hRawSpectrum", "Thick_wNoThin_TimeSlice-200_200/hRawSpectrum"};
  Int_t colours[n_hists] = {kBlack, kBlue, kRed};
  
  int rebin_factor = 5;
  
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
      
      axislabel.str("");
      axislabel << "Counts / " << hEnergy->GetXaxis()->GetBinWidth(1) << " keV";
      hEnergy->GetYaxis()->SetTitle(axislabel.str().c_str());
      
      hEnergy->Draw("HIST E SAME");
    }

    //    infile->Close();
  }
}
