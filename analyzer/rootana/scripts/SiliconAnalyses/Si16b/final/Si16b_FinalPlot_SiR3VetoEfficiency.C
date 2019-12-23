void Si16b_FinalPlot_SiR3VetoEfficiency(std::string savedir = "") {
  std::string filename = "~/data/results/Si16b/plots_newPP_geq1TgtPulse.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_hists = 4;
  std::string dirnames[n_hists] = {"all_SiR_timecut0_10000ns_layerCoinc", "all_SiR_timecut0_10000ns_layerCoinc", "proton_SiR_timecut0_10000ns_layerCoinc", "all_SiR_timecut0_10000ns_layerCoinc"};
  std::string histnames[n_hists] = {"hEvdE_TwoLayer_12", "hEvdE_TwoLayer_123", "hEvdE_TwoLayer_123", "hEvdE_ThreeLayer_123"};
  std::string histtitles[n_hists] = {"Si16b Dataset, Right Arm, No SiR3 Cut", "Si16b Dataset, Right Arm, With SiR3 Hit", "Si16b Dataset, Right Arm, With SiR3 Hit (protons)", "Si16b Dataset, Right Arm, With SiR3 Hit"};
  std::string suffixes[n_hists] = {"noCut", "withHit", "withHitProtons", "threeLayer"};
  int rebin_factors[n_hists] = {1, 1, 10, 10};
  double max_x_axis[n_hists] = {20000, 20000, 20000, 25000};
  double max_y_axis[n_hists] = {3000, 3000, 3000, 20000};
  //  double box_min_x[n_hists] = {6000, 6000, 12000};
  //  double box_max_x[n_hists] = {8000, 8000, 14000};
  //  double box_min_y[n_hists] = {0, 0, 12000};
  //  double box_max_y[n_hists] = {300, 300, 14000};
  double e1 = 5000; double e2 = 8000;
  double box_min_x[n_hists] = {e1, e1, 0, 0};
  double box_max_x[n_hists] = {e2, e2, 0, 25000};
  double box_min_y[n_hists] = {0, 0, 0, e1};
  double box_max_y[n_hists] = {300, 300, 0, e2};
  double integrals[n_hists] = {0};
  

  const int n_use_hists = 3;
  for (int i_hist = 0; i_hist < n_use_hists; ++i_hist) {
    std::string dirname = dirnames[i_hist];
    std::string i_histname = dirname + "/" + histnames[i_hist];
    std::string canvasname = "c_" + histnames[i_hist];
    TCanvas* c = new TCanvas(canvasname.c_str());
    
    TH2F* hEvdE = (TH2F*) file->Get(i_histname.c_str());
    if (!hEvdE) {
      std::cout << "Can't find histogram " << i_histname << std::endl;
      continue;
    }
    hEvdE->Rebin2D(rebin_factors[i_hist], rebin_factors[i_hist]);
    hEvdE->GetXaxis()->SetRangeUser(0, max_x_axis[i_hist]);
    hEvdE->GetYaxis()->SetRangeUser(0, max_y_axis[i_hist]);
    hEvdE->SetTitle(histtitles[i_hist].c_str());
    hEvdE->SetStats(false);
    hEvdE->Draw("COLZ");

    double integral_bin_x_low = hEvdE->GetXaxis()->FindBin(box_min_x[i_hist]);
    double integral_bin_x_high = hEvdE->GetXaxis()->FindBin(box_max_x[i_hist]);
    double integral_bin_y_low = hEvdE->GetYaxis()->FindBin(box_min_y[i_hist]);
    double integral_bin_y_high = hEvdE->GetYaxis()->FindBin(box_max_y[i_hist]);
    integrals[i_hist] = hEvdE->Integral(integral_bin_x_low, integral_bin_x_high, integral_bin_y_low, integral_bin_y_high);
    std::cout << i_histname << ": Integral ("
	      << box_min_x[i_hist] << " < " << hEvdE->GetXaxis()->GetTitle() << " < " << box_max_x[i_hist] << ", "
      	      << box_min_y[i_hist] << " < " << hEvdE->GetYaxis()->GetTitle() << " < " << box_max_y[i_hist] << ", "
	      << ") = " << integrals[i_hist] << std::endl;

    if (integrals[i_hist] > 0) {
      TBox* box = new TBox(box_min_x[i_hist], box_min_y[i_hist], box_max_x[i_hist], box_max_y[i_hist]);
      box->SetLineColor(kRed);
      box->SetFillStyle(0);
      box->Draw("LSAME");
      
      std::stringstream text;
      text << integrals[i_hist] << " entries";
    
      TLatex* latex = new TLatex();
      latex->SetTextAlign(22);
      latex->SetTextColor(kRed);
      double latex_x = box_min_x[i_hist] + (box_max_x[i_hist] - box_min_x[i_hist])/2;
      double latex_y = box_max_y[i_hist] + 100;
      latex->DrawLatex(latex_x, latex_y, text.str().c_str());
    }
    
    alcaphistogram(hEvdE);
    alcapPreliminary(hEvdE);
    hEvdE->SetDrawOption("COLZ");
    
    if (savedir != "") {
      std::string savename = savedir + "AlCapData_Si16bDataset_SiR3VetoEfficiency_" + suffixes[i_hist];

      std::string pdfname = savename + ".pdf";
      c->SaveAs(pdfname.c_str());
      std::string pngname = savename + ".png";
      c->SaveAs(pngname.c_str());
    }
  }

  double ratio = integrals[1] / integrals[0];
  double ratio_error = std::sqrt( (ratio*(1-ratio)) / integrals[0] );
  std::cout << "Ratio = " << ratio << " +/- " << ratio_error << std::endl;
}
