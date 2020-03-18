void Si16b_FinalPlot_TwoLayer_TDiff(std::string savedir = "", std::ostream& numbers_file = std::cout) {
  
  std::string infilename = "~/data/results/Si16b/plots_newPP_geq1TgtPulse.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");
  //  std::string histname = "SiR_LayerTDiff/hLayerTDiff";

  const int n_particles = 6;
  std::string particles[n_particles] = {"all", "proton", "deuteron", "triton", "alpha", "total"};
  std::string Particles[n_particles] = {"All", "Proton", "Deuteron", "Triton", "Alpha", "All Selected"};

  /*
  const int n_fit_fns = 2;
  TF1* fit_fns[n_fit_fns] = {0, 0};
  std::string fit_names[n_fit_fns] = {"Double Gaussian Fit", "Single Gaussian Fit"};
  */
  const int n_fit_fns = 1;
  TF1* fit_fns[n_fit_fns] = {0};
  std::string fit_names[n_fit_fns] = {"Single Gaussian Fit"};
  
  TH1F* hTotal = 0;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {

    std::string particle = particles[i_particle];
    std::string Particle = Particles[i_particle];
    
    std::string histname = particle + "_SiR_timecut0_10000ns_noLayerCoinc";
    if (particle != "all") {
      histname += "_PSel";
    }
    histname += "/hTDiff_TwoLayer_12";
    //    histname += "/hTDiff_ThreeLayer_123";
    std::string histtitle = Particle + " Hits (no layer coincidence cuts)";

    double layer_coinc_cut = 100;

    //  gStyle->SetOptFit(1);

    std::string canvasname = "c_tdiff_" + particle;
    TCanvas* c_tdiff = new TCanvas(canvasname.c_str(), canvasname.c_str());
    c_tdiff->SetLogy();
  
    TH1F* hLayerTDiff = NULL;
    if (particle != "total") {
      hLayerTDiff = (TH1F*) infile->Get(histname.c_str());
    }
    else {
      hLayerTDiff = hTotal;
    }
    if(!hLayerTDiff){
      std::cout << "Can't find histogram " << histname << std::endl;
      continue;
    }
    
    if (particle != "all") {
      if (!hTotal) {
	hTotal = (TH1F*) hLayerTDiff->Clone("hTotal");
      }
      else {
	hTotal->Add(hLayerTDiff);
      }
    }

    std::string fitname = "single_gaus_" + particle;
    TF1* single_gaus = new TF1(fitname.c_str(), "[0]*TMath::Gaus(x,[1],[2])", -225,225);
    single_gaus->SetParName(0, "Norm");
    single_gaus->SetParName(1, "Mean");
    single_gaus->SetParName(2, "Sigma");
    single_gaus->SetParameters(100,0,100);
    single_gaus->SetLineColor(kRed);
    single_gaus->SetLineStyle(kDashed);

    fitname = "double_gaus_" + particle;
    TF1* double_gaus = new TF1(fitname.c_str(), "[0]*TMath::Gaus(x,[1],[2]) + [3]*TMath::Gaus(x,[4],[5])", -225,225);
    double_gaus->SetParName(0, "Gaus 1 Norm");
    double_gaus->SetParName(1, "Gaus 1 Mean");
    double_gaus->SetParName(2, "Gaus 1 Sigma");
    double_gaus->SetParName(3, "Gaus 2 Norm");
    double_gaus->SetParName(4, "Gaus 2 Mean");
    double_gaus->SetParName(5, "Gaus 2 Sigma");
    double_gaus->SetParameters(100,0,100, 10,0,200);
    double_gaus->SetLineColor(kBlue);
    
    std::stringstream text;
    TLatex* latex = new TLatex();
    latex->SetTextAlign(22);
    
    std::stringstream leglabel;
    TLegend* leg = new TLegend(0.55,0.60,0.8,0.89);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.03);
    leg->SetFillColor(kWhite);
    
    hLayerTDiff->SetTitle(histtitle.c_str());
    hLayerTDiff->SetStats(false);
    hLayerTDiff->GetYaxis()->SetTitleOffset(0.9);
    hLayerTDiff->GetXaxis()->SetTitleOffset(0.9);
    hLayerTDiff->Draw("HIST E");

    alcaphistogram(hLayerTDiff);
    alcapPreliminary(hLayerTDiff);
    hLayerTDiff->SetDrawOption("HIST E");
    
    //    fit_fns[0] = double_gaus; fit_fns[1] = single_gaus;
    fit_fns[0] = single_gaus;
    for (int i_fit_fn = 0; i_fit_fn < n_fit_fns; ++i_fit_fn) {
      TF1* fit_fn = fit_fns[i_fit_fn];
      
      TFitResultPtr result = hLayerTDiff->Fit(fit_fn, "R0S");
      if (!result) {
	std::cout << "Fit failed" << std::endl;
	//      std::cout << "AE: " << result->Status() << std::endl;
	//      result->Print();
	if (result->Status() != 0) {
	  std::cout << "Failed fit" << std::endl;
	  continue;
	}
      }
      
      double total_integral = fit_fn->Integral(-2000,2000);
      double integral_in_cut = fit_fn->Integral(-layer_coinc_cut, layer_coinc_cut);
      double fraction_from_fit = integral_in_cut / total_integral;
      double fraction_from_fit_err = std::sqrt( (fraction_from_fit*(1-fraction_from_fit)) / total_integral );

      double total_integral_hist = hLayerTDiff->Integral();
      int min_tdiff_bin = hLayerTDiff->GetXaxis()->FindBin(-layer_coinc_cut);
      int max_tdiff_bin = hLayerTDiff->GetXaxis()->FindBin(layer_coinc_cut);
      double integral_in_cut_hist = hLayerTDiff->Integral(min_tdiff_bin, max_tdiff_bin);
      double fraction_from_hist = integral_in_cut_hist / total_integral_hist;
      double fraction_from_hist_err = std::sqrt( (fraction_from_hist*(1-fraction_from_hist)) / total_integral );      
      
      std::cout << "AE: Fraction = " << integral_in_cut << " / " << total_integral << " = " << fraction_from_fit << " +/- " << fraction_from_fit_err << std::endl;
      
      fit_fn->Draw("LSAME");
      
      leglabel.str("");
      //      leglabel << "#splitline{" << fit_names[i_fit_fn] << std::fixed << std::setprecision(0) << "}{#splitline{#chi^2 / ndf = " << fit_fn->GetChisquare() << " / " << fit_fn->GetNDF() << ", #sigma = " << fit_fn->GetParameter(2) << " ns}{#splitline{Eff (from fit) = " << std::setprecision(4) << fraction_from_fit << " #pm " << fraction_from_fit_err << "}{Eff (from hist) = " << std::setprecision(4) << fraction_from_hist << " #pm " << fraction_from_hist_err << "}}}";
      leglabel << "#splitline{" << fit_names[i_fit_fn] << std::fixed << std::setprecision(0) << "}{#splitline{#chi^2 / ndf = " << fit_fn->GetChisquare() << " / " << fit_fn->GetNDF() << ", #sigma = " << fit_fn->GetParameter(2) << " ns}{Efficiency = " << std::setprecision(4) << fraction_from_fit << " #pm " << fraction_from_fit_err << "}}";
      leg->AddEntry(fit_fn, leglabel.str().c_str(), "l");

      if (particle == "total") {
	numbers_file << "% From Si16b_FinalPlot_TwoLayer_TDiff.C" << std::endl;
	numbers_file << "\\newcommand\\SibLayerCoincEffFit{$" << std::setprecision(4) << fraction_from_fit << " \\pm " << fraction_from_fit_err << "$}" << std::endl;
	numbers_file << "\\newcommand\\SibLayerCoincEffHist{$" << std::setprecision(4) << fraction_from_hist << " \\pm " << fraction_from_hist_err << "$}" << std::endl;
	numbers_file << std::endl;
      }
    }
    leg->Draw();
    
    TLine* lower_cut = new TLine(-layer_coinc_cut, 0, -layer_coinc_cut, hLayerTDiff->GetMaximum());
    lower_cut->SetLineWidth(2);
    lower_cut->SetLineColor(kBlack);
    lower_cut->Draw("LSAME");
    TLine* upper_cut = new TLine(layer_coinc_cut, 0, layer_coinc_cut, hLayerTDiff->GetMaximum());
    upper_cut->SetLineWidth(2);
    upper_cut->SetLineColor(kBlack);
    upper_cut->Draw("LSAME");
    
    text.str("");
    text << std::fixed << std::setprecision(0) << -layer_coinc_cut << " ns < t_{diff} < " << layer_coinc_cut << " ns";
    latex->DrawLatexNDC(0.275, 0.8, text.str().c_str());

    if (savedir != "" && particle=="total") {
      std::string plotname = savedir + "AlCapData_Si16bDataset_TwoLayer_LayerTDiff_" + particle;
      std::string pdfname = plotname + ".pdf";
      std::string pngname = plotname + ".png";
      c_tdiff->SaveAs(pdfname.c_str());
      c_tdiff->SaveAs(pngname.c_str());
    }
  }
}
