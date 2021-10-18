void SiL3_FinalPlot_NewEscapeCorrection(std::string savedir = "") {

  //  std::string filename = "~/data/results/SiL3/unfold_geq2TgtPulse_newPP20us_1_test.root";
  //  std::string filename = "~/data/results/SiL3/fits_geq2TgtPulse_newPP20us_1_newTimeSlice_test_fitScan.root";
  std::string filename = "~/data/results/SiL3/fits_geq2TgtPulse_newPP20us_2.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_settings = 4;
  std::string settings[n_settings] = {"Single", "Double", "SingleThresh", "DoubleThresh"};//, "TripleExp"};
  std::string fn_names[n_settings] = {"single-exponential", "double-exponential", "threshold + single-exponential", "threshold + double-exponential"};
  Int_t colours[n_settings] = {kRed, kBlue, kBlack, kMagenta};//, kBlack};
  Int_t marker_colours[n_settings] = {kRed, kBlue, kBlack, kMagenta};//, kBlack};
  Int_t marker_styles[n_settings] = {kFullTriangleUp, kOpenTriangleDown, kFullCircle, kOpenSquare};

  int n_fit_ranges = 100;
  float x_vals[n_fit_ranges];
  float chi2_ndfs[n_settings][n_fit_ranges];
  float T0s[n_settings][n_fit_ranges];
  float T0Errs[n_settings][n_fit_ranges];
  float T1s[n_settings][n_fit_ranges];
  float T1Errs[n_settings][n_fit_ranges];
  float Tths[n_settings][n_fit_ranges];
  float TthErrs[n_settings][n_fit_ranges];
  
  TLegend* leg_scan = new TLegend(0.30,0.55,0.90,0.85);
  leg_scan->SetBorderSize(0);
  leg_scan->SetTextSize(0.05);
  leg_scan->SetFillStyle(0);
  leg_scan->SetFillColor(kWhite);

  // TCanvas* c1 = new TCanvas("c1", "c1");
  // c1->SetLogy();
  // c1->SetGridx();
  // c1->SetGridy();

  //  TCanvas* c_residual = new TCanvas();

  bool draw_plot = true;
  double min_E = 1.5;
  double max_E = 30;
  double E_step = 30;
  int i_fit_range = -1;
  for (double i_min_E = min_E; i_min_E < max_E; i_min_E += E_step) {
    //    for (double i_max_E = max_E; i_max_E > i_min_E; i_max_E -= E_step) {
    for (double i_max_E = max_E; i_max_E > 29; i_max_E -= E_step) {

      double fit_range_min = i_min_E;
      double fit_range_max = i_max_E;
      std::stringstream foldername;
      ++i_fit_range;
      x_vals[i_fit_range] = fit_range_min;
      //    std::string foldername = "NewProtonEscape_TimeSlice2000_4000_allRecoil_" + setting;
      foldername.str("");
      //    foldername << "NewCombinedEscape_TimeSlice2000_4000_allRecoil_FitRange" << fit_range_min << "_30_" << setting;
      foldername << "FinalFit_FitRange" << fit_range_min << "_" << fit_range_max << "_100keVBins_combined_";
      std::cout << foldername.str() << std::endl;
      std::string histname = foldername.str() + std::string("/hInputSpectrum");
      TGraphAsymmErrors* raw_spectrum = (TGraphAsymmErrors*) file->Get(histname.c_str());
      if (!raw_spectrum) {
	std::cout << "Error: Problem getting raw_spectrum " << histname << std::endl;
	return;
      }
      histname = foldername.str() + std::string("/hCorrectedSpectrum");

      raw_spectrum->SetTitle("SiL3 Dataset, Active Target Analysis, New Escape Correction");
      raw_spectrum->GetXaxis()->SetRangeUser(0,30);
      raw_spectrum->GetYaxis()->SetRangeUser(1e-7, 1e1);
      raw_spectrum->SetLineColor(kBlack);
      raw_spectrum->GetXaxis()->SetTitleOffset(0.9);
      raw_spectrum->GetYaxis()->SetTitleOffset(0.9);
      //    raw_spectrum->Scale(1.0/rebin_factor);

      std::stringstream axislabel;
      //    axislabel << "Counts / " << raw_spectrum->GetBinWidth(1) << " MeV";
      axislabel << "Counts / 0.1 MeV";
      //    raw_spectrum->SetYTitle(axislabel.str().c_str());

      if (fit_range_min == 1.5 && fit_range_max == 30) {
	TCanvas* c_spec = new TCanvas();
	c_spec->SetLogy();
	c_spec->SetGridx();
	c_spec->SetGridy();
	raw_spectrum->Draw("APE");
      }

      for (int i_setting = 0; i_setting < n_settings; ++i_setting) {

	std::string setting = settings[i_setting];

	std::string fnname = foldername.str() + "/" + setting;
	TF1* fn = (TF1*) file->Get(fnname.c_str());
	//    fn->SetNpx(100);
	fn->SetLineColor(kRed);
	//    fn->SetParameter(0, fn->GetParameter(0)*rebin_factor);
	std::cout << "Chi2 / ndf = " << fn->GetChisquare() << " / " << fn->GetNDF() << std::endl;
	std::cout << "T0 = " << fn->GetParameter(1) << std::endl;
	chi2_ndfs[i_setting][i_fit_range] = fn->GetChisquare() / fn->GetNDF();
	T0s[i_setting][i_fit_range] = fn->GetParameter(1);
	T0Errs[i_setting][i_fit_range] = fn->GetParError(1);
	if (setting == "Double") {
	  T1s[i_setting][i_fit_range] = fn->GetParameter(3);
	  T1Errs[i_setting][i_fit_range] = fn->GetParError(3);
	}
	else if (setting == "SingleThresh") {
	  Tths[i_setting][i_fit_range] = fn->GetParameter(2);
	  TthErrs[i_setting][i_fit_range] = fn->GetParError(2);
	}
	else if (setting == "DoubleThresh") {
	  T1s[i_setting][i_fit_range] = fn->GetParameter(5);
	  T1Errs[i_setting][i_fit_range] = fn->GetParError(5);
	  Tths[i_setting][i_fit_range] = fn->GetParameter(2);
	  TthErrs[i_setting][i_fit_range] = fn->GetParError(2);
	}
	//	std::cout << "(x, y) = " << x_vals[i_fit_range] << ", " << chi2_ndfs[i_setting][i_fit_range] << std::endl;
	//    std::cout << "AE: N0 = " << fn->GetParameter(0) << " +/- " << fn->GetParError(0) << std::endl;
	//    std::cout << "AE: T0 = " << fn->GetParameter(1) << " +/- " << fn->GetParError(1) << std::endl;
	TF1* expo = 0;
	std::string fit_str = "";
	if (setting == "Single") {
	  fit_str = "[0]*TMath::Exp(-x/[1])";
	  expo = new TF1("expo", fit_str.c_str(), fit_range_min, raw_spectrum->GetXaxis()->GetXmax());
	  expo->SetParameters(fn->GetParameter(0), fn->GetParameter(1));
	}
	else if (setting == "Double") {
	  //      fit_str = "[0]*(TMath::Exp(-x/[1]) + [2]*TMath::Exp(-x/[3]))";
	  fit_str = "[0]*(TMath::Exp(-x/[1]) + [2]*TMath::Exp(-x/[3]))";
	  expo = new TF1("expo", fit_str.c_str(), fit_range_min, raw_spectrum->GetXaxis()->GetXmax());
	  expo->SetParameters(fn->GetParameter(0), fn->GetParameter(1), fn->GetParameter(2), fn->GetParameter(3));
	}
	else if (setting == "SingleThresh") {
	  fit_str = "[0]*(1 - [1]/x)^[2]*TMath::Exp(-x/[3])";
	  //	  fit_range_min = fn->GetParameter(2);
	  expo = new TF1("expo", fit_str.c_str(), fit_range_min, raw_spectrum->GetXaxis()->GetXmax());
	  //      expo->SetParameters(5e4, 1.4, 1.3, 4.8);
	  expo->SetParameters(fn->GetParameter(0), fn->GetParameter(2), fn->GetParameter(3), fn->GetParameter(1));
	}
	else if (setting == "DoubleThresh") {
	  fit_str = "[0]*(1 - [2]/x)^[3]*(TMath::Exp(-x/[1])+[4]*TMath::Exp(-x/[5]))";
	  //	  fit_range_min = fn->GetParameter(2);
	  expo = new TF1("expo", fit_str.c_str(), fit_range_min, raw_spectrum->GetXaxis()->GetXmax());
	  //      expo->SetParameters(5e4, 1.4, 1.3, 4.8);
	  expo->SetParameters(fn->GetParameter(0), fn->GetParameter(1), fn->GetParameter(2), fn->GetParameter(3), fn->GetParameter(4), fn->GetParameter(5));
	}
	std::cout << "AE: " << fit_range_min << " " << fit_range_max << " " << setting << std::endl;
	if (fit_range_min == 1.5 && fit_range_max == 30 && setting == "DoubleThresh") {
	  expo->SetLineColor(kMagenta);
	  fn->Draw("LSAME");
	  expo->Draw("LSAME");
	  TF1* expo_extrap = (TF1*) expo->Clone("expo_extrap");
	  expo_extrap->SetLineStyle(kDashed);
	  expo_extrap->SetRange(expo->GetParameter(2), 50);
	  expo_extrap->Draw("LSAME");

	  TLegend* leg = new TLegend(0.30,0.55,0.90,0.85);
	  leg->SetBorderSize(0);
	  leg->SetTextSize(0.03);
	  leg->SetFillStyle(0);
	  leg->SetFillColor(kWhite);
	  leg->AddEntry(raw_spectrum, "Spectrum (w/ decay electron correction)", "l");
	  leg->AddEntry(expo, ("True Model (" + fn_names[i_setting] + ")").c_str(), "l");
	  leg->AddEntry(fn, "Folded True Model", "l");
	  leg->Draw();

	  double latex_x = 0.15;
	  TLatex* latex = new TLatex();
	  latex->SetTextSize(0.04);

	  latex->DrawLatexNDC(0.2, 0.85, fit_str.c_str());
	  
	  std::stringstream leglabel;
	  leglabel.str("");
	  leglabel << "Fit Range: " << fit_range_min << " - " << fit_range_max << " MeV";
	  latex->DrawLatexNDC(latex_x, 0.45, leglabel.str().c_str());
	  
	  leglabel.str("");
	  leglabel << "#chi^2 / ndf = " << fn->GetChisquare() << " / " << fn->GetNDF();
	  latex->DrawLatexNDC(latex_x, 0.40, leglabel.str().c_str());

	  leglabel.str("");
	  leglabel << "N0 = " << fn->GetParameter(0) << " #pm " << fn->GetParError(0);
	  latex->DrawLatexNDC(latex_x, 0.35, leglabel.str().c_str());

	  leglabel.str("");
	  leglabel << "T0 = " << fn->GetParameter(1) << " #pm " << fn->GetParError(1);
	  latex->DrawLatexNDC(latex_x, 0.30, leglabel.str().c_str());

	  // leglabel.str("");
	  // leglabel << "E0 = " << fn->GetParameter(2) << " #pm " << fn->GetParError(2);
	  // latex->DrawLatexNDC(latex_x, 0.30, leglabel.str().c_str());

	  if (setting == "Double") {
	    leglabel.str("");
	    leglabel << "r = " << fn->GetParameter(2) << " #pm " << fn->GetParError(2);
	    latex->DrawLatexNDC(latex_x, 0.25, leglabel.str().c_str());

	    leglabel.str("");
	    leglabel << "T1 = " << fn->GetParameter(3) << " #pm " << fn->GetParError(3);
	    latex->DrawLatexNDC(latex_x, 0.20, leglabel.str().c_str());
	  }

	  if (setting == "SingleThresh") {
	    leglabel.str("");
	    leglabel << "T_{th} = " << fn->GetParameter(2) << " #pm " << fn->GetParError(2);
	    latex->DrawLatexNDC(latex_x, 0.25, leglabel.str().c_str());

	    leglabel.str("");
	    leglabel << "#alpha = " << fn->GetParameter(3) << " #pm " << fn->GetParError(3);
	    latex->DrawLatexNDC(latex_x, 0.20, leglabel.str().c_str());
	  }

	  if (setting == "DoubleThresh") {
	    leglabel.str("");
	    leglabel << "T_{th} = " << fn->GetParameter(2) << " #pm " << fn->GetParError(2);
	    latex->DrawLatexNDC(latex_x, 0.25, leglabel.str().c_str());

	    leglabel.str("");
	    leglabel << "#alpha = " << fn->GetParameter(3) << " #pm " << fn->GetParError(3);
	    latex->DrawLatexNDC(latex_x, 0.20, leglabel.str().c_str());

	    leglabel.str("");
	    leglabel << "r = " << fn->GetParameter(4) << " #pm " << fn->GetParError(4);
	    latex->DrawLatexNDC(latex_x, 0.15, leglabel.str().c_str());

	    leglabel.str("");
	    leglabel << "T1 = " << fn->GetParameter(5) << " #pm " << fn->GetParError(5);
	    latex->DrawLatexNDC(latex_x, 0.10, leglabel.str().c_str());
	  }

	  std::string fitresult_name = foldername.str() + "/TFitResult-hInputSpectrum-"+setting;
	  TFitResult* result = (TFitResult*) file->Get(fitresult_name.c_str());
	  if (!result) {
	    std::cout << "ERROR: Couldn't get TFitResult " << fitresult_name << std::endl;
	  }
	  double rate = expo->Integral(fit_range_min, 50);
	  double error = expo->IntegralError(fit_range_min, 50, result->GetParams(), result->GetCovarianceMatrix().GetMatrixArray());
	  std::cout << "AE: integral = " << rate << " +/- " << error << std::endl;
	  //    std::cout << "AE: Rate = " << (expo->Integral(fit_range_min, 50)*10) / 1.5697e+07 << std::endl;
	}
      }
    }
  }

  TCanvas* c_scan_chi2 = new TCanvas();
  TCanvas* c_scan_T0 = new TCanvas();
  TCanvas* c_scan_T1 = new TCanvas();
  TCanvas* c_scan_Tth = new TCanvas();
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = settings[i_setting];
    
    c_scan_chi2->cd();
    TGraphErrors* grChi2Ndf = new TGraphErrors(i_fit_range+1, x_vals, chi2_ndfs[i_setting], 0, 0);
    grChi2Ndf->SetMarkerStyle(marker_styles[i_setting]);
    grChi2Ndf->SetMarkerColor(marker_colours[i_setting]);
    if (i_setting == 0) {
      grChi2Ndf->Draw("APE");
      grChi2Ndf->GetXaxis()->SetTitle("Fit Start Energy [MeV]");
      grChi2Ndf->GetYaxis()->SetTitle("#chi^2 / ndf");
    }
    else {
      grChi2Ndf->Draw("PE SAME");
    }

    c_scan_T0->cd();
    TGraphErrors* grT0 = new TGraphErrors(i_fit_range+1, x_vals, T0s[i_setting], 0, T0Errs[i_setting]);
    // if (setting == "DoubleThresh") {
    //   for (int i_point = 0; i_point < grT0->GetN(); ++i_point) {
    // 	std::cout << "Debug: T0 = " << *(grT0->GetY()+i_point) << " +/- " << *(grT0->GetEY()+i_point) << std::endl;
    //   }
    // }

    grT0->SetMarkerStyle(marker_styles[i_setting]);
    grT0->SetMarkerColor(marker_colours[i_setting]);
    grT0->SetLineColor(marker_colours[i_setting]);
    if (i_setting == 0) {
      grT0->Draw("APE");
      grT0->GetXaxis()->SetTitle("Fit Start Energy [MeV]");
      grT0->GetYaxis()->SetTitle("T0 [MeV]");
      grT0->GetXaxis()->SetRangeUser(0,5);
      grT0->GetYaxis()->SetRangeUser(0,10);
    }
    else {
      grT0->Draw("PE SAME");
    }

    c_scan_T1->cd();
    TGraphErrors* grT1 = new TGraphErrors(i_fit_range+1, x_vals, T1s[i_setting], 0, T1Errs[i_setting]);
    grT1->SetMarkerStyle(marker_styles[i_setting]);
    grT1->SetMarkerColor(marker_colours[i_setting]);
    grT1->SetLineColor(marker_colours[i_setting]);
    if (setting == "Double") {
      grT1->Draw("APE");
      grT1->GetXaxis()->SetTitle("Fit Start Energy [MeV]");
      grT1->GetYaxis()->SetTitle("T1 [MeV]");
      grT1->GetXaxis()->SetRangeUser(0,5);
      grT1->GetYaxis()->SetRangeUser(0,20);
    }
    else if (setting == "DoubleThresh") {
      grT1->Draw("PE SAME");
    }

    c_scan_Tth->cd();
    TGraphErrors* grTth = new TGraphErrors(i_fit_range+1, x_vals, Tths[i_setting], 0, TthErrs[i_setting]);
    grTth->SetMarkerStyle(marker_styles[i_setting]);
    grTth->SetMarkerColor(marker_colours[i_setting]);
    grTth->SetLineColor(marker_colours[i_setting]);
    if (setting == "SingleThresh") {
      grTth->Draw("APE");
      grTth->GetXaxis()->SetTitle("Fit Start Energy [MeV]");
      grTth->GetYaxis()->SetTitle("Tth [MeV]");
      grTth->GetXaxis()->SetRangeUser(0,5);
      grTth->GetYaxis()->SetRangeUser(-5,5);
    }
    else if (setting == "DoubleThresh") {
      grTth->Draw("PE SAME");
    }
    
    leg_scan->AddEntry(grChi2Ndf, fn_names[i_setting].c_str(), "p");
  }
  c_scan_chi2->cd();
  leg_scan->Draw();

  //   c_residual->cd();
  //   c_residual->SetGridy();
  //   TH1F* hResidual = (TH1F*) raw_spectrum->Clone("hResidual");
  //   hResidual->SetLineColor(colours[i_setting]);
  //   hResidual->Reset();
  //   for (int i_bin = 1; i_bin <= hResidual->GetXaxis()->GetNbins(); ++i_bin) {
  //     double bin_center = hResidual->GetBinCenter(i_bin);
  //     double residual = (raw_spectrum->GetBinContent(i_bin) - fn->Eval(bin_center)) / (raw_spectrum->GetBinError(i_bin));
  //     hResidual->SetBinContent(i_bin, residual);
  //   }
  //   hResidual->GetXaxis()->SetRangeUser(fit_range_min, 30);
  //   hResidual->Draw("HIST SAME");
  // }

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_SiL3Dataset_ActiveTarget_EscapeCorrection";
    
    std::string pdfname = savename + ".pdf";
    //    c1->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    //    c1->SaveAs(pngname.c_str());
  }
}
