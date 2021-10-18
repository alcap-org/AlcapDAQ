#include "../../Utils/IntegrateRates.C"

void Si16b_FinalPlot_NormalisedSpectrum_wSiL3NewCorr(std::string savedir = "", std::ostream& numbers_file = std::cout) {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();


  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.17, 0.75, "AlCap #bf{#it{Preliminary}}");

  //////////////////////////////////////
  // SiL3
  //  std::string SiL3_filename = "~/data/results/SiL3/fits_geq2TgtPulse_newPP20us_1_KFactor0-85_test.root";
  std::string SiL3_filename = "~/data/results/SiL3/fits_geq2TgtPulse_newPP20us_2.root";
  TFile* SiL3_file = new TFile(SiL3_filename.c_str(), "READ");

  //  std::string recoil = "allRecoil";
  //  std::string SiL3_dirname = "FinalFit_FitRange1.5_30_100keVBins_alpha__4";
  std::string SiL3_dirname = "FinalFit_FitRange1.5_30_100keVBins_combined__allRecoil";
  //    std::string SiL3_dirname = "FinalFit_FitRange1.5_30_100keVBins_combined__noRecoil";
  //  std::string SiL3_dirname = "FinalFit_FitRange1.5_30_100keVBins_proton__pRecoil";
  //  std::string SiL3_dirname = "FinalFit_FitRange1.5_30_100keVBins_alpha__aRecoil";
  std::string SiL3_histname = SiL3_dirname + "/hInputSpectrum";
  TGraphAsymmErrors* SiL3_raw_spectrum = (TGraphAsymmErrors*) SiL3_file->Get(SiL3_histname.c_str());
  if (!SiL3_raw_spectrum) {
    std::cout << "Error: Problem getting SiL3_raw_spectrum " << SiL3_histname << std::endl;
    return;
  }
  SiL3_raw_spectrum->SetLineColor(kBlack);
  SiL3_raw_spectrum->SetMarkerColor(kBlack);
  SiL3_raw_spectrum->GetFunction("Single")->SetBit(TF1::kNotDraw);
  SiL3_raw_spectrum->GetFunction("Double")->SetBit(TF1::kNotDraw);
  SiL3_raw_spectrum->GetFunction("SingleThresh")->SetBit(TF1::kNotDraw);
  SiL3_raw_spectrum->GetFunction("DoubleThresh")->SetBit(TF1::kNotDraw);

  std::string SiL3_fnname = SiL3_dirname + "/DoubleThresh";
  //  std::string SiL3_fnname = SiL3_dirname + "/SingleThresh";
  TF1* SiL3_fn = (TF1*) SiL3_file->Get(SiL3_fnname.c_str());
  //  SiL3_fn->SetNpx(100);

  double min_E = 1.5;
  double max_E = 30;
  std::string fit_str = "[0]*(1 - [2]/x)^[3]*(TMath::Exp(-x/[1])+[4]*TMath::Exp(-x/[5]))";;//"[0]*(1 - [1]/x)^[2]*(TMath::Exp(-x/[3])+[4]*TMath::Exp(-x/[5]))";
  //  std::string fit_str = "[0]*(1 - [2]/x)^[3]*(TMath::Exp(-x/[1])";
  TF1* expo = new TF1("expo", fit_str.c_str(), min_E, max_E);
  expo->SetParameters(SiL3_fn->GetParameter(0), SiL3_fn->GetParameter(1), SiL3_fn->GetParameter(2), SiL3_fn->GetParameter(3), SiL3_fn->GetParameter(4), SiL3_fn->GetParameter(5));
  std::cout << "par0 = " << expo->GetParameter(0) << std::endl;
  std::cout << "par1 = " << expo->GetParameter(1) << std::endl;
  std::cout << "par2 = " << expo->GetParameter(2) << std::endl;
  std::cout << "par3 = " << expo->GetParameter(3) << std::endl;
  std::cout << "par4 = " << expo->GetParameter(4) << std::endl;
  std::cout << "par5 = " << expo->GetParameter(5) << std::endl;
  expo->SetParError(0, SiL3_fn->GetParError(0));
  expo->SetParError(1, SiL3_fn->GetParError(1));
  expo->SetParError(2, SiL3_fn->GetParError(2));
  expo->SetParError(3, SiL3_fn->GetParError(3));
  expo->SetParError(4, SiL3_fn->GetParError(4));
  expo->SetParError(5, SiL3_fn->GetParError(5));
  expo->SetLineColor(kMagenta);

  //////////////////////
  // Sobottka-Wills
  std::string SW_filename = "~/data/results/SiL3/SW-plots_betterErrors_MeV.root";
  //  std::string SW_filename = "~/data/results/Si16b/SW-plots-above1400keV.root";
  TFile* SW_file = new TFile(SW_filename.c_str(), "READ");
  TGraphErrors* SW_gre = (TGraphErrors*) SW_file->Get("Graph");
  SW_gre->GetFunction("tdr_fit")->SetBit(TF1::kNotDraw);

  // TF1* fit = new TF1("fit", "[0]*(1 - [1]/x)^[2]*TMath::Exp(-x/[3])", 1.4, x_max);
  // fit->SetParameters(0.1, 1.4, 1.3, 4.8);
  // SW_gre->Fit("fit", "RM");
  // SW_gre->GetFunction("fit")->SetLineColor(kRed);

  TF1* fit_dble = new TF1("fit_dble", "[0]*(1 - [1]/x)^[2]*(TMath::Exp(-x/[3]) + [4]*TMath::Exp(-x/[5]))", 1.4, 26);
  fit_dble->SetParameters(0.1, 1.4, 1.3, 4.8, 0.1, 7.0);
  SW_gre->Fit("fit_dble", "RM+");

  //  SW_gre->Fit(SW_gre->GetFunction("tdr_fit"));
  //  SW_gre->GetFunction("tdr_fit")->SetBit(TF1::kNotDraw);
  //  SW_gre->GetFunction("tdr_fit")->SetLineColor(kBlack);

  /////////////////////////////////
  // Si16b
  //  std::string filename = "~/data/results/Si16b/unfold_newPP.root";
  //  std::string filename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_JohnVetoEff_1.root";
  std::string filename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_2.root";
  //    std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_SiL1-2--6.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  std::string i_particle_name = "Sum";
  std::string dirname = "FinalSystPlot_" + i_particle_name + "_TCutG_2sig_layerCoinc500ns_tGT0ns_BinW500keV";
  std::string i_histname = dirname + "/hFinalStatSyst";

  TGraphAsymmErrors* spectrum = (TGraphAsymmErrors*) file->Get(i_histname.c_str());
  if (!spectrum) {
    std::cout << "Error: Problem getting spectrum " << i_histname << std::endl;
    //      return;
  }
  TGraphAsymmErrors* pdta_spectrum = (TGraphAsymmErrors*) spectrum->Clone("pdta_spectrum");

  for (int i_point = 0; i_point < spectrum->GetN(); ++i_point) {
    if (*(spectrum->GetX()+i_point) < 6
	|| *(spectrum->GetX()+i_point) > 17) {
      spectrum->RemovePoint(i_point);
      --i_point;
    }
  }

  for (int i_point = 0; i_point < pdta_spectrum->GetN(); ++i_point) {
      if (*(pdta_spectrum->GetX()+i_point) < 15
	|| *(pdta_spectrum->GetX()+i_point) > 17) {
      pdta_spectrum->RemovePoint(i_point);
      --i_point;
    }
  }

  
  //    spectrum->SetStats(false);
  spectrum->SetLineColor(kRed);
  spectrum->SetFillStyle(0);
  spectrum->SetFillColor(0);
  spectrum->SetLineWidth(2);
  spectrum->SetMarkerStyle(kFullCircle);
  spectrum->SetMarkerColor(kRed);
  //    spectrum->SetLineColor(kBlack);
  //    spectrum->SetFillColor(i_colour);
  //  spectrum->GetXaxis()->SetRangeUser(0, 30);
  //    spectrum->GetYaxis()->SetRangeUser(5e-4, 5e-2);

  TCanvas* c_lit_comp = new TCanvas("c_lit_comp", "c_lit_comp");
  c_lit_comp->SetLogy();
  SiL3_raw_spectrum->SetLineColor(kRed);
  SiL3_raw_spectrum->SetLineWidth(1);
  SiL3_raw_spectrum->SetMarkerColor(kRed);
  //  SiL3_raw_spectrum->Draw("APE");
  //  SiL3_raw_spectrum->GetXaxis()->SetRangeUser(0, 30);
  //  SiL3_raw_spectrum->GetYaxis()->SetRangeUser(5e-5, 5e-1);
  //  SW_gre->SetMarkerStyle(kOpenCircle);
  SW_gre->SetLineColor(kBlack);
  SW_gre->SetMarkerColor(kBlack);
  //  SW_gre->Draw("PE SAME");
  SW_gre->Draw("APE");
  //  SW_gre->GetXaxis()->SetLimits(0,30);
  SW_gre->GetYaxis()->SetRangeUser(5e-5, 5e-1);
  SW_gre->GetYaxis()->SetTitle("Charged Particle Yield / Capture / MeV");
  //  SiL3_raw_spectrum->GetFunction("DoubleThresh")->SetBit(TF1::kNotDraw);
  SW_gre->GetYaxis()->SetTitleSize(0.05);
  SW_gre->GetXaxis()->SetTitleSize(0.05);
  SW_gre->GetYaxis()->SetTitleOffset(1.0);
  SW_gre->GetYaxis()->SetLabelSize(0.05);
  SW_gre->GetXaxis()->SetLabelSize(0.05);
  //  SiL3_raw_spectrum->Draw("PE SAME");
  //  SiL3_fn->SetLineColor(kSpring);
  //  SiL3_fn->Draw("LSAME");
  //  SW_gre->GetFunction("fit_dble")->SetLineColor(kMagenta);
  //  SW_gre->GetFunction("fit_dble")->SetLineWidth(2);
  //  SW_gre->GetFunction("fit_dble")->Draw("LSAME");
  SW_gre->GetFunction("fit_dble")->SetBit(TF1::kNotDraw);
  expo->SetLineColor(kBlue);
  expo->SetLineStyle(kSolid);
  expo->Draw("LSAME");

  //  spectrum->Draw("PE SAME");
  
  TLegend* leg = new TLegend(0.25,0.65,0.80,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.04);
  leg->SetFillColor(kWhite);
  leg->SetHeader("AlCap Si");
  //  leg->AddEntry(SiL3_raw_spectrum, "AlCap active target (data)", "lp");
  //  leg->AddEntry(SiL3_fn, "AlCap active target (fit)", "lp");
  leg->AddEntry(expo, "AlCap active target (escape-corrected fit)", "lp");
  leg->AddEntry(SW_gre, "Sobottka-Wills (data)", "pl");
  //  leg->AddEntry(SW_gre->GetFunction("fit_dble"), "Sobottka-Wills (fit)", "l");
  //  leg->AddEntry(spectrum, "passive Si target (p+d+t+#alpha)", "lp");
  leg->Draw();

  TCanvas* c_self_comp = new TCanvas("c_self_comp", "c_self_comp");
  c_self_comp->SetLogy();
  SiL3_raw_spectrum->SetLineColor(kRed);
  SiL3_raw_spectrum->SetLineWidth(1);
  SiL3_raw_spectrum->SetMarkerColor(kRed);
  //  SiL3_raw_spectrum->Draw("APE");
  //  SiL3_raw_spectrum->GetXaxis()->SetRangeUser(0, 30);
  //  SiL3_raw_spectrum->GetYaxis()->SetRangeUser(5e-5, 5e-1);
  //  SiL3_raw_spectrum->Draw("PE SAME");
  SiL3_fn->SetLineColor(kSpring);
  //  SiL3_fn->Draw("LSAME");
  expo->SetLineStyle(kSolid);
  //  expo->Draw("LSAME");
  double start = 2;
  double end = 30;//1500;
  double point_step = 0.1;
  int n_interval_points = (end - start)/point_step;
  TGraphErrors* fit_interval = new TGraphErrors(n_interval_points);
  std::string intervalname = "interval";
  fit_interval->SetName(intervalname.c_str());
  for (int i_point = 0; i_point < n_interval_points; ++i_point) {
    fit_interval->SetPoint(i_point, start + i_point*point_step, 0);
  }
  TFitter* expo_fitter = new TFitter();
  expo_fitter->SetUserFunc(expo);
  expo_fitter->SetParameter(0, "N0", expo->GetParameter(0), expo->GetParError(0), 0, 1);
  expo_fitter->SetParameter(1, "N0", expo->GetParameter(1), expo->GetParError(1), 0, 1);
  expo_fitter->SetParameter(2, "N0", expo->GetParameter(2), expo->GetParError(2), 0, 1);
  expo_fitter->SetParameter(3, "N0", expo->GetParameter(3), expo->GetParError(3), 0, 1);
  expo_fitter->SetParameter(4, "N0", expo->GetParameter(4), expo->GetParError(4), 0, 1);
  expo_fitter->SetParameter(5, "N0", expo->GetParameter(5), expo->GetParError(5), 0, 1);
  //  expo_fitter->SetCovarianceMatrix
  //  expo_fitter->GetConfidenceIntervals(fit_interval);
  fit_interval->SetFillColor(kBlue);
  fit_interval->SetFillStyle(3001);
  fit_interval->Draw("4 same");

  spectrum->SetLineColor(kGray);
  spectrum->SetMarkerColor(kGray);
  TF1* expo_clone = (TF1*) expo->Clone("expo_clone");
  expo_clone->SetRange(1, 19);
  expo_clone->SetMaximum(5e-1);
  expo_clone->SetMinimum(5e-5);
  expo_clone->Draw();
  expo_clone->GetHistogram()->GetXaxis()->SetTitle("Energy [MeV]");
  expo_clone->GetHistogram()->GetYaxis()->SetTitle("Charged Particle Yield / Capture / MeV");
  spectrum->Draw("PE SAME");
  pdta_spectrum->SetMarkerStyle(kFullCircle);
  pdta_spectrum->SetLineColor(kBlack);
  pdta_spectrum->SetMarkerColor(kBlack);
  pdta_spectrum->Draw("PE SAME");
  //  expo->Draw("LSAME");

  spectrum->GetYaxis()->SetTitle("Charged Particle Yield / Capture / MeV");
  spectrum->GetXaxis()->SetTitle("Energy [MeV]");
  spectrum->GetYaxis()->SetRangeUser(5e-5,5e-1);
  //  spectrum->GetXaxis()->SetLimits(0, 30);
  
  TLegend* self_leg = new TLegend(0.35,0.65,0.80,0.85);
  self_leg->SetBorderSize(0);
  self_leg->SetTextSize(0.04);
  self_leg->SetFillColor(kWhite);
  self_leg->SetHeader("AlCap Si");
  //  self_leg->AddEntry(SiL3_raw_spectrum, "AlCap active target (data)", "lp");
  //  self_leg->AddEntry(SiL3_fn, "AlCap active target (fit)", "lp");
  self_leg->AddEntry(expo_clone, "active target", "lp");
  self_leg->AddEntry(spectrum, "passive target (p+d+t)", "lp");
  self_leg->AddEntry(pdta_spectrum, "passive target (p+d+t+#alpha)", "lp");
  self_leg->Draw();

  TCanvas* c_explanation = new TCanvas("c_explanation", "c_explanation");
  c_explanation->SetLogy();
  SiL3_raw_spectrum->SetLineColor(kRed);
  SiL3_raw_spectrum->SetLineWidth(1);
  SiL3_raw_spectrum->SetMarkerColor(kRed);
  SiL3_raw_spectrum->Draw("APE");
  SiL3_raw_spectrum->GetXaxis()->SetRangeUser(0, 30);
  SiL3_raw_spectrum->GetYaxis()->SetRangeUser(5e-5, 5e-1);
  SiL3_raw_spectrum->GetYaxis()->SetTitle("Charged Particle Yield / Capture / MeV");
  SiL3_raw_spectrum->GetXaxis()->SetTitle("Energy [MeV]");
  SiL3_raw_spectrum->Draw("PE SAME");
  SiL3_fn->SetLineColor(kSpring);
  SiL3_fn->Draw("LSAME");
  expo->SetLineStyle(kSolid);
  expo->Draw("LSAME");
  //  expo->Draw("");

  TLegend* leg_explanation = new TLegend(0.25,0.65,0.75,0.85);
  leg_explanation->SetBorderSize(0);
  leg_explanation->SetTextSize(0.04);
  leg_explanation->SetFillColor(kWhite);
  leg_explanation->SetHeader("AlCap Si (active target)");
  leg_explanation->AddEntry(SiL3_raw_spectrum, "data", "lp");
    leg_explanation->AddEntry(expo, "initial energy distribution", "lp");
  leg_explanation->AddEntry(SiL3_fn, "initial energy distribution #times detector response", "lp");
  leg_explanation->Draw();


  TCanvas* c_alpha = new TCanvas();
  c_alpha->SetLogy();
  int n_points = spectrum->GetN();
  double energy[n_points];
  double bin_width[n_points];
  double alphas[n_points];
  for (int i_point = 0; i_point < n_points; ++i_point) {
    energy[i_point] = *(spectrum->GetX()+i_point);
    bin_width[i_point] = spectrum->GetErrorX(i_point);
    alphas[i_point] = expo->Eval(energy[i_point]) - *(spectrum->GetY()+i_point);
  }
  TGraphAsymmErrors* grAlphas = new TGraphAsymmErrors(n_points, energy, alphas, bin_width, bin_width);
  dirname = "FinalSystPlot_proton_TCutG_2sig_layerCoinc500ns_tGT0ns_BinW500keV";
  i_histname = dirname + "/hFinalStatSyst";
  TGraphAsymmErrors* realProtons = (TGraphAsymmErrors*) file->Get(i_histname.c_str());
  realProtons->SetLineColor(kRed);
  realProtons->Draw("APE");
  realProtons->GetXaxis()->SetRangeUser(0,17);
  realProtons->GetYaxis()->SetRangeUser(5e-6, 1);
  dirname = "FinalSystPlot_deuteron_TCutG_2sig_layerCoinc500ns_tGT0ns_BinW500keV";
  i_histname = dirname + "/hFinalStatSyst";
  TGraphAsymmErrors* realDeuterons = (TGraphAsymmErrors*) file->Get(i_histname.c_str());
  realDeuterons->SetLineColor(kBlue);
  realDeuterons->Draw("PE SAME");
  dirname = "FinalSystPlot_triton_TCutG_2sig_layerCoinc500ns_tGT0ns_BinW500keV";
  i_histname = dirname + "/hFinalStatSyst";
  TGraphAsymmErrors* realTritons = (TGraphAsymmErrors*) file->Get(i_histname.c_str());
  realTritons->SetLineColor(kSpring);
  realTritons->Draw("PE SAME");
  dirname = "FinalSystPlot_alpha_TCutG_2sig_layerCoinc500ns_tGT0ns_BinW500keV";
  i_histname = dirname + "/hFinalStatSyst";
  TGraphAsymmErrors* realAlphas = (TGraphAsymmErrors*) file->Get(i_histname.c_str());
  realAlphas->SetLineColor(kMagenta);
  realAlphas->Draw("PE SAME");
  grAlphas->SetLineWidth(2);
  grAlphas->Draw("PE SAME");
  TLegend* leg2 = new TLegend(0.35,0.65,0.80,0.85);
  leg2->SetBorderSize(0);
  leg2->SetTextSize(0.04);
  leg2->SetFillColor(kWhite);
  leg2->AddEntry(realProtons, "protons (Si16b)", "pl");
  leg2->AddEntry(realDeuterons, "deuterons (Si16b)", "pl");
  leg2->AddEntry(realTritons, "tritons (Si16b)", "pl");
  leg2->AddEntry(realAlphas, "alphas (Si16b)", "pl");
  leg2->AddEntry(grAlphas, "SiL3 (true model) - Si16b (p+d+t+#alpha) (no errors)", "pl");
  leg2->Draw();
  double inferRate, statErr, hiSystErr, loSystErr = 0;
  IntegrateRates_wStatAndSystSpectra(grAlphas, grAlphas, 5, 14, inferRate, statErr, hiSystErr, loSystErr);
  std::cout << "Inferred Alphas (5 -- 14 MeV) = " << inferRate << std::endl;
  
  if (savedir != "") {
    std::string savename = savedir + "AlCapData_Si16bDataset_NormalisedSpectra_wSiL3NewCorr";
    std::string pdfname = savename + "_SelfComp.pdf";
    c_self_comp->SaveAs(pdfname.c_str());
    std::string pngname = savename + "_SelfComp.png";
    c_self_comp->SaveAs(pngname.c_str());

    pdfname = savename + "_LitComp.pdf";
    c_lit_comp->SaveAs(pdfname.c_str());
    pngname = savename + "_LitComp.png";
    c_lit_comp->SaveAs(pngname.c_str());

    pdfname = savename + "_Explanation.pdf";
    c_explanation->SaveAs(pdfname.c_str());
    pngname = savename + "_Explanation.png";
    c_explanation->SaveAs(pngname.c_str());

    // savename = savedir + "AlCapData_NormalisedSpectraComparison_Unstacked";    
    // pdfname = savename + ".pdf";
    // //    c_unstacked->SaveAs(pdfname.c_str());
    // pngname = savename + ".png";
    // //    c_unstacked->SaveAs(pngname.c_str());
  }

  double min_energy = 1.4;
  double max_energy = 26;
  TFitResult* result = (TFitResult*) SiL3_file->Get((SiL3_dirname+"/TFitResult-hInputSpectrum-DoubleThresh").c_str());
  double rate = expo->Integral(min_energy, max_energy);
  std::cout << "Cov(0,0) = " << (result->GetCovarianceMatrix())(0,0) << std::endl;
  double error = expo->IntegralError(min_energy, max_energy, result->GetParams(), result->GetCovarianceMatrix().GetMatrixArray());
  std::cout << "SiL3 Rate = " << rate << " +/- " << error << std::endl;
  numbers_file << "% Si16b_FinalPlot_NormalisedSpectrum_wSiL3NewCorr.C" << std::endl;
  numbers_file << std::fixed << std::setprecision(1);
  numbers_file << "\\newcommand\\SiLActiveRange{" << min_energy << " -- " << std::setprecision(0) << max_energy << "}" << std::endl;
  numbers_file << std::fixed << std::setprecision(15);
  numbers_file << "\\newcommand\\SiLActiveRate{\\num[round-precision=3, round-mode=figures]{" << rate*1e3 << "}(\\num[round-precision=1, round-mode=figures]{" << error*1e3 << "})}" << std::endl;
  //  numbers_file << "\\newcommand\\SiLActiveRateBare{" << "" << rates[i_range] << "}" << std::endl;
  // numbers_file << "\\newcommand\\SiLActiveStatErrBare{" << total_stat_errors[i_range] << "}" << std::endl;
  // numbers_file << "\\newcommand\\SiLActiveHighSystErrBare{" << total_high_syst_errors[i_range] << "}" << std::endl;
  // numbers_file << "\\newcommand\\SiLActiveLowSystErrBare{" << total_low_syst_errors[i_range] << "}" << std::endl;
  // numbers_file << std::fixed << std::setprecision(2);
  // numbers_file << "\\newcommand\\SiLActiveSystFracUncertainty{$^{+" << (total_high_syst_errors[i_range]/rate)*100 << "\\%}_{-" << (total_low_syst_errors[i_range]/rate)*100 << "\\%}$}" << std::endl;
  // numbers_file << "\\newcommand\\SiLActiveRate{$\\SiLActiveRateBare \\pm \\SiLActiveStatErrBare ~\\text{(stat.)} ^{+\\SiLActiveHighSystErrBare}_{-\\SiLActiveLowSystErrBare}~\\text{(syst.)}$}" << std::endl;
  numbers_file << std::endl;
}
