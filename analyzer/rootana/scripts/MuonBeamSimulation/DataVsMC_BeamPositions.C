void DataVsMC_BeamPositions(std::string outdir = "") {
  /*  
  const int n_runs = 8;
  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743}; // want to do all runs individually
  double scale_factors[n_runs] = {1.035, 0.99, 0.98, 1.01, 1.02, 1.03, 1.04, 1.05};
  */
  /*
  const int n_runs = 7;
  int run_numbers[n_runs] = {10404, 10473, 10477, 9740, 9741, 9742, 9743}; // want to do all runs individually
  std::string dirs[n_runs] = {"BeamPos/", "BeamPos/", "BeamPos/", "Si16a/", "Si16a/", "Si16a/", "Si16a/"};
  */
  const int n_runs = 3;
  int run_numbers[n_runs] = {10404, 10473, 10477};
  std::string dirs[n_runs] = {"BeamPos/", "BeamPos/", "BeamPos/"};
  //
  const int n_settings = 2;
  std::string folders[n_settings] = {"~/data/results/", "~/data/mc/"};
  std::string files[n_settings] = {"positions", "MC_raw_spectra"};
  std::string leglabels[n_settings] = {"Data", "MC"};
  Int_t colours[n_settings] = {kBlack, kRed};
  std::string inhistnames[n_settings] = {"hPosition", "hBeamPosition_SiL"};
  bool do_fit[n_settings] = {false, true};

  std::stringstream run_str, leglabel;
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    TCanvas* c = new TCanvas();

    run_str.str("");
    run_str << std::fixed << std::setw(5) << std::setfill('0') << run_numbers[i_run];
    std::cout << "Run " << run_str.str() << std::endl;
    
    TLegend* leg = new TLegend(0.15,0.65,0.40,0.90);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.03);
    leg->SetFillStyle(0);
    leg->SetFillColor(kWhite);
    
    for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
      std::string infilename = folders[i_setting] + dirs[i_run] + files[i_setting] + run_str.str();
      if (i_setting == 1 && run_numbers[i_run]==10473) {
       	infilename += "_original";
      }
      infilename += ".root";
      TFile* infile = new TFile(infilename.c_str(), "READ");
      if (infile->IsZombie()) {
	std::cout << "ERROR: Can't find file " << infilename << std::endl;
	return;
      }
    
      std::string inhistname = inhistnames[i_setting];
      TH1F* hBeamPos = (TH1F*) infile->Get(inhistname.c_str());
      if (!hBeamPos) {
	std::cout << "ERROR: Can't find histogram " << inhistname << std::endl;
	return;
      }
    
      std::string histtitle;
      histtitle = "Beam Position (Run " + run_str.str() + ")";
      hBeamPos->SetTitle(histtitle.c_str());
      hBeamPos->Sumw2();
      hBeamPos->SetStats(false);
      hBeamPos->SetMarkerColor(colours[i_setting]);
      hBeamPos->SetLineColor(colours[i_setting]);
      hBeamPos->SetLineStyle(1);
      hBeamPos->GetXaxis()->SetTitle("Strip Number");
      hBeamPos->Scale(1.0 / hBeamPos->Integral(1, 14));
      hBeamPos->GetYaxis()->SetTitle("Unit Area (strips 1 - 14)");
      hBeamPos->SetMaximum(0.15);
      hBeamPos->Draw("HIST E SAME");

      std::string fit_fn_name = "gaus";
      hBeamPos->Fit(fit_fn_name.c_str(), "Q", "", 2, 15);
      hBeamPos->GetFunction(fit_fn_name.c_str())->SetLineColor(colours[i_setting]);
      double mean = hBeamPos->GetFunction(fit_fn_name.c_str())->GetParameter(1);
      double mean_error = hBeamPos->GetFunction(fit_fn_name.c_str())->GetParError(1);
      double sigma = hBeamPos->GetFunction(fit_fn_name.c_str())->GetParameter(2);
      double sigma_error = hBeamPos->GetFunction(fit_fn_name.c_str())->GetParError(2);
      double chi_sq = hBeamPos->GetFunction(fit_fn_name.c_str())->GetChisquare();
      int ndf = hBeamPos->GetFunction(fit_fn_name.c_str())->GetNDF();
      std::cout << "Fit: mean = " << mean << " +/- " << mean_error << ", sigma = " << sigma << " +/- " << sigma_error << ", chi^2 / ndf = " << chi_sq << " / " << ndf << " = " << chi_sq / ndf << std::endl;

      hBeamPos->GetFunction(fit_fn_name.c_str())->Draw("LSAME");
      
      leglabel.str("");
      leglabel << leglabels[i_setting] << " Fit: #left(#splitline{mean = " << std::fixed << std::setprecision(2) << mean << " #pm " << mean_error << "}{sigma = " << sigma << " #pm " << sigma_error << "}#right)";
      leg->AddEntry(hBeamPos, leglabel.str().c_str(), "l");

    }

    TLatex* latex = new TLatex();
    latex->SetTextAlign(22);
    std::string text = "Run " + run_str.str();
    latex->DrawLatexNDC(0.65, 0.75, text.c_str());

    leg->Draw();

    if (outdir != "") {
      std::string savename = outdir + "/AlCapDataVsMC_Run" + run_str.str() + "_BeamPosition";
      std::string pdfname = savename + ".pdf";
      c->SaveAs(pdfname.c_str());
    }
  }
}
