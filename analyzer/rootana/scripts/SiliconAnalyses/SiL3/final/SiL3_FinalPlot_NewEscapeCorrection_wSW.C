void SiL3_FinalPlot_NewEscapeCorrection_wSW(std::string savedir = "") {

    std::string SW_filename = "~/data/results/SiL3/SW-plots_betterErrors_MeV.root";
  TFile* SW_file = new TFile(SW_filename.c_str(), "READ");
  TGraphErrors* SW_gre = (TGraphErrors*) SW_file->Get("Graph");
  //  SW_gre->SetTitle("Charged Particle Emission (after decay electron correction)");
  //  SW_gre->SetTitle("Charged Particle Emission (after proton escape correction)");
  //  SW_gre->SetTitle("Charged Particle Emission (after deuteron escape correction)");
  SW_gre->SetTitle("Charged Particle Emission");
  SW_gre->GetXaxis()->SetRangeUser(0,26000);
  SW_gre->GetXaxis()->SetTitle("Energy [keV]");
  SW_gre->GetYaxis()->SetTitle("Particles / muon capture / keV");
  SW_gre->GetXaxis()->SetTitleOffset(0.9);
  SW_gre->GetYaxis()->SetTitleOffset(0.9);
  SW_gre->SetMarkerColor(kRed);
  SW_gre->SetLineColor(kRed);
  //  SW_gre->GetFunction("tdr_fit")->SetLineColor(kBlack);
  SW_gre->GetFunction("tdr_fit")->SetBit(TF1::kNotDraw);
  //  SW_gre->Draw("APE");
  //  leg->AddEntry(SW_gre->GetFunction("tdr_fit"), "S-W Fit", "l");


  std::string filename = "~/data/results/SiL3/unfold_geq2TgtPulse_newPP20us_1_test.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_settings = 2;
  std::string settings[n_settings] = {"SingleExp", "DoubleExp"};//, "TripleExp"};
  Int_t colours[n_settings] = {kRed, kBlue};//, kGreen};

  // TCanvas* c1 = new TCanvas("c1", "c1");
  // c1->SetLogy();
  // c1->SetGridx();
  // c1->SetGridy();

  double n_cap_muon = 1.5697e+07/10;
  
  TCanvas* c_residual = new TCanvas();
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {

    std::string setting = settings[i_setting];
    
    std::string foldername = "NewCombinedEscape_TimeSlice2000_4000_allRecoil_" + setting;
    std::string histname = foldername + "/hInputSpectrum";
    TH1F* raw_spectrum = (TH1F*) file->Get(histname.c_str());
    if (!raw_spectrum) {
      std::cout << "Error: Problem getting raw_spectrum " << histname << std::endl;
      return;
    }
    histname = foldername + "/hCorrectedSpectrum";

    int rebin_factor = 1;
    raw_spectrum->Rebin(rebin_factor);

    raw_spectrum->SetTitle("SiL3 Dataset, Active Target Analysis, New Escape Correction");
    raw_spectrum->SetStats(false);
    raw_spectrum->GetXaxis()->SetRangeUser(0,35);
    //    raw_spectrum->GetYaxis()->SetRangeUser(1e-3, 1e7);
    raw_spectrum->SetLineColor(kBlack);
    raw_spectrum->GetXaxis()->SetTitleOffset(0.9);
    raw_spectrum->GetYaxis()->SetTitleOffset(0.9);

    std::stringstream axislabel;
    axislabel << "Counts / " << raw_spectrum->GetBinWidth(1) << " MeV";
    raw_spectrum->SetYTitle(axislabel.str().c_str());

    raw_spectrum->Scale(1.0 / n_cap_muon);
    
    TCanvas* c_spec = new TCanvas();
    c_spec->SetLogy();
    c_spec->SetGridx();
    c_spec->SetGridy();
    alcaphistogram(raw_spectrum);
    raw_spectrum->Draw("HIST E");
    alcapPreliminary(raw_spectrum);

    std::string fnname = foldername + "/fit_fn";
    TF1* fn = (TF1*) file->Get(fnname.c_str());
    //    fn->SetNpx(100);
    fn->SetLineColor(kRed);
    fn->Draw("LSAME");
    fn->SetParameter(0, fn->GetParameter(0) / n_cap_muon);
    std::cout << "AE: N0 = " << fn->GetParameter(0) << " +/- " << fn->GetParError(0) << std::endl;
    std::cout << "AE: T0 = " << fn->GetParameter(1) << " +/- " << fn->GetParError(1) << std::endl;
    TF1* expo = 0;
    double min_E = 3.5;
    if (setting == "SingleExp") {
      expo = new TF1("expo", "[0]*TMath::Exp(-x/[1])", min_E, raw_spectrum->GetXaxis()->GetXmax());
      expo->SetParameters(fn->GetParameter(0), fn->GetParameter(1));
    }
    else if (setting == "DoubleExp") {
      expo = new TF1("expo", "[0]*(TMath::Exp(-x/[1]) + [2]*TMath::Exp(-x/[3]))", min_E, raw_spectrum->GetXaxis()->GetXmax());
      expo->SetParameters(fn->GetParameter(0), fn->GetParameter(1), fn->GetParameter(2), fn->GetParameter(3));
    }
    expo->SetLineColor(kMagenta);
    expo->Draw("LSAME");

    SW_gre->Draw("PE SAME");
    
    TLegend* leg = new TLegend(0.30,0.55,0.90,0.85);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.03);
    leg->SetFillStyle(0);
    leg->SetFillColor(kWhite);
    leg->AddEntry(raw_spectrum, "Spectrum (w/ decay electron correction)", "l");
    leg->AddEntry(expo, "True Model", "l");
    leg->AddEntry(fn, "Folded True Model", "l");
    leg->AddEntry(SW_gre, "Sobottka-Wills", "pl");
    leg->Draw();
  
    double latex_x = 0.15;
    TLatex* latex = new TLatex();
    latex->SetTextSize(0.04);
    std::stringstream leglabel;
    leglabel.str("");
    leglabel << "#chi^2 / ndf = " << fn->GetChisquare() << " / " << fn->GetNDF();
    latex->DrawLatexNDC(latex_x, 0.45, leglabel.str().c_str());

    leglabel.str("");
    leglabel << "N0 = " << fn->GetParameter(0) << " #pm " << fn->GetParError(0);
    latex->DrawLatexNDC(latex_x, 0.40, leglabel.str().c_str());

    leglabel.str("");
    leglabel << "T0 = " << fn->GetParameter(1) << " #pm " << fn->GetParError(1);
    latex->DrawLatexNDC(latex_x, 0.35, leglabel.str().c_str());

    // leglabel.str("");
    // leglabel << "E0 = " << fn->GetParameter(2) << " #pm " << fn->GetParError(2);
    // latex->DrawLatexNDC(latex_x, 0.30, leglabel.str().c_str());

    if (setting == "DoubleExp") {
      leglabel.str("");
      leglabel << "r = " << fn->GetParameter(2) << " #pm " << fn->GetParError(2);
      latex->DrawLatexNDC(latex_x, 0.25, leglabel.str().c_str());

      leglabel.str("");
      leglabel << "T1 = " << fn->GetParameter(3) << " #pm " << fn->GetParError(3);
      latex->DrawLatexNDC(latex_x, 0.20, leglabel.str().c_str());
    }
    std::cout << "AE: integral = " << expo->Integral(min_E, 50) << std::endl;
    std::cout << "AE: Rate = " << (expo->Integral(min_E, 50)*10) / 1.5697e+07 << std::endl;

    c_residual->cd();
    TH1F* hResidual = (TH1F*) raw_spectrum->Clone("hResidual");
    hResidual->SetLineColor(colours[i_setting]);
    hResidual->Reset();
    for (int i_bin = 1; i_bin <= hResidual->GetXaxis()->GetNbins(); ++i_bin) {
      double bin_center = hResidual->GetBinCenter(i_bin);
      double residual = raw_spectrum->GetBinContent(i_bin) - fn->Eval(bin_center);
      hResidual->SetBinContent(i_bin, residual);
    }
    hResidual->GetXaxis()->SetRangeUser(3, 26);
    hResidual->Draw("HIST SAME");
  }

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_SiL3Dataset_ActiveTarget_EscapeCorrection";
    
    std::string pdfname = savename + ".pdf";
    //    c1->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    //    c1->SaveAs(pngname.c_str());
  }
}
