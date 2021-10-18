void SiL3_FinalPlot_ResponseMatrixFit_ParticleComposition(std::string savedir = "") {

  //  std::string filename = "~/data/results/SiL3/unfold_geq2TgtPulse_newPP20us_1_test.root";
  //  std::string filename = "~/data/results/SiL3/fits_geq2TgtPulse_newPP20us_1_newTimeSlice_test_fitScan.root";
  std::string filename = "~/data/results/SiL3/fits_geq2TgtPulse_newPP20us_2.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_settings = 3;
  std::string settings[n_settings] = {"combined__allRecoil", "proton__pRecoil", "syst__systRecoil"};//"alpha__aRecoil"};
  std::string fn_names[n_settings] = {"nominal (50% p, 30% d, 4% t, 15% a)", "100% proton", "syst (75% p, 21% d, 5% t)"};//"100% alpha"};
  Int_t colours[n_settings] = {kBlack, kRed, kGreen+3};

  bool draw_plot = true;
  double min_E = 1.5;
  double max_E = 30;
  double E_step = 30;
  int i_fit_range = -1;
  double fit_range_min = min_E;
  double fit_range_max = max_E;
  std::stringstream foldername;

  TLegend* leg = new TLegend(0.30,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);

  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = settings[i_setting];
    foldername.str("");
    foldername << "FinalFit_FitRange" << fit_range_min << "_" << fit_range_max << "_100keVBins_" << setting;
    std::cout << foldername.str() << std::endl;

    std::stringstream axislabel;
    //    axislabel << "Counts / " << raw_spectrum->GetBinWidth(1) << " MeV";
    axislabel << "Counts / 0.1 MeV";
    //    raw_spectrum->SetYTitle(axislabel.str().c_str());

    std::string fit_model = "DoubleThresh";
    std::string fnname = foldername.str() + "/" + fit_model;
    TF1* fn = (TF1*) file->Get(fnname.c_str());
    //    fn->SetNpx(100);
    fn->SetLineColor(colours[i_setting]);
    //    fn->SetParameter(0, fn->GetParameter(0)*rebin_factor);
    std::cout << "Chi2 / ndf = " << fn->GetChisquare() << " / " << fn->GetNDF() << std::endl;
    std::cout << "T0 = " << fn->GetParameter(1) << std::endl;
    //	std::cout << "(x, y) = " << x_vals[i_fit_range] << ", " << chi2_ndfs[i_setting][i_fit_range] << std::endl;
    //    std::cout << "AE: N0 = " << fn->GetParameter(0) << " +/- " << fn->GetParError(0) << std::endl;
    //    std::cout << "AE: T0 = " << fn->GetParameter(1) << " +/- " << fn->GetParError(1) << std::endl;
    TF1* expo = 0;
    std::string fit_str = "";
    if (fit_model == "Single") {
      fit_str = "[0]*TMath::Exp(-x/[1])";
      expo = new TF1("expo", fit_str.c_str(), fit_range_min, fit_range_max);//raw_spectrum->GetXaxis()->GetXmax());
      expo->SetParameters(fn->GetParameter(0), fn->GetParameter(1));
    }
    else if (fit_model == "Double") {
      //      fit_str = "[0]*(TMath::Exp(-x/[1]) + [2]*TMath::Exp(-x/[3]))";
      fit_str = "[0]*(TMath::Exp(-x/[1]) + [2]*TMath::Exp(-x/[3]))";
      expo = new TF1("expo", fit_str.c_str(), fit_range_min,fit_range_max); // raw_spectrum->GetXaxis()->GetXmax());
      expo->SetParameters(fn->GetParameter(0), fn->GetParameter(1), fn->GetParameter(2), fn->GetParameter(3));
    }
    else if (fit_model == "SingleThresh") {
      fit_str = "[0]*(1 - [1]/x)^[2]*TMath::Exp(-x/[3])";
      //	  fit_range_min = fn->GetParameter(2);
      expo = new TF1("expo", fit_str.c_str(), fit_range_min, fit_range_max); //raw_spectrum->GetXaxis()->GetXmax());
      //      expo->SetParameters(5e4, 1.4, 1.3, 4.8);
      expo->SetParameters(fn->GetParameter(0), fn->GetParameter(2), fn->GetParameter(3), fn->GetParameter(1));
    }
    else if (fit_model == "DoubleThresh") {
      fit_str = "[0]*(1 - [2]/x)^[3]*(TMath::Exp(-x/[1])+[4]*TMath::Exp(-x/[5]))";
      //	  fit_range_min = fn->GetParameter(2);
      expo = new TF1("expo", fit_str.c_str(), fit_range_min, fit_range_max); //raw_spectrum->GetXaxis()->GetXmax());
      //      expo->SetParameters(5e4, 1.4, 1.3, 4.8);
      expo->SetParameters(fn->GetParameter(0), fn->GetParameter(1), fn->GetParameter(2), fn->GetParameter(3), fn->GetParameter(4), fn->GetParameter(5));
    }
    std::cout << "AE: " << fit_range_min << " " << fit_range_max << " " << fit_model << std::endl;
    expo->SetLineColor(colours[i_setting]);
    //      fn->Draw("LSAME");
    if (i_setting == 0) {
      expo->Draw();
    }
    else {
      expo->Draw("LSAME");
    }
    TF1* expo_extrap = (TF1*) expo->Clone("expo_extrap");
    expo_extrap->SetLineStyle(kDashed);
    expo_extrap->SetRange(expo->GetParameter(2), 50);
    expo_extrap->Draw("LSAME");
      
    leg->AddEntry(expo, fn_names[i_setting].c_str(), "l");

  }
  leg->Draw();
      
}
