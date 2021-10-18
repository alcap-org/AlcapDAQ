//#include "../../Utils/IntegrateRates.C"

void Si16b_FinalPlot_SpectrumFit(std::string savedir = "", std::ostream& numbers_file = std::cout) {
    
  //  std::string filename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_1.root";
  std::string filename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_JohnVetoEff_1.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  double x_max = 25;
  
  const int n_ranges = 1;
  double min_energies[n_ranges] = {15};
  double max_energies[n_ranges] = {16};
  
  const int n_settings = 5;
  TCanvas* particle_canvases[n_settings] = {0};
  std::string particle_names[n_settings] = {"proton", "deuteron", "triton", "alpha", "Sum"};
  std::string Particle_names[n_settings] = {"Proton", "Deuteron", "Triton", "Alpha", "Sum"};
  Int_t colours[n_settings] = {kRed, kCyan, kMagenta, kSpring, kBlue};
  std::string leglabels[n_settings] = {"protons", "deuterons", "tritons", "alphas", "sum"};
  double min_fit_ranges[n_settings] = {3, 5, 6, 15, 3};
  double max_fit_ranges[n_settings] = {17, 16, 17, 20, 17};
  TGraph* grIntegrals[n_settings] = {0};
  
  TLegend* leg_for_all = new TLegend(0.60,0.65,0.85,0.85);
  leg_for_all->SetBorderSize(0);
  leg_for_all->SetTextSize(0.035);
  leg_for_all->SetFillColor(kWhite);

  std::stringstream leglabel;
  TCanvas* c_all_log = new TCanvas();
  c_all_log->SetLogy();

  TCanvas* c_fits_only = new TCanvas("c_fits_only", "c_fits_only");
  
  numbers_file << "% Si16b_FinalPlot_SpectrumFit.C" << std::endl;
  int min_setting = 0;
  int max_setting = n_settings;
  for (int i_setting = min_setting; i_setting < max_setting; ++i_setting) {
    particle_canvases[i_setting] = new TCanvas();
    particle_canvases[i_setting]->SetLogy();
    
    std::string particle = particle_names[i_setting];
    std::string Particle = Particle_names[i_setting];
    Int_t i_colour = colours[i_setting];

    std::string i_dirname = "FinalSystPlot_" + particle + "_TCutG_2sig_layerCoinc500ns_tGT0ns";
    std::string i_statsystname = i_dirname + "/hFinalStatSyst";
    TGraphAsymmErrors* statsyst_spectrum = (TGraphAsymmErrors*) file->Get(i_statsystname.c_str());
    if (!statsyst_spectrum) {
      std::cout << "Error: Problem getting statsyst_spectrum " << i_statsystname << std::endl;
      return;
    }
    statsyst_spectrum->Draw("APE");
    statsyst_spectrum->GetYaxis()->SetRangeUser(1e-6, 5);
    statsyst_spectrum->GetXaxis()->SetRangeUser(0, 25);
    gPad->Update();

    TF1* expo_fit = new TF1("expo_fit","[0]*(exp(-x/[1]))",min_fit_ranges[i_setting],max_fit_ranges[i_setting]);//27);
    expo_fit->SetParName(0, "N");
    expo_fit->SetParName(1, "T_{0}");
    expo_fit->SetParameter(0, 1e-3);
    expo_fit->SetParLimits(0, 1e-4, 1e2);
    expo_fit->SetParameter(1, 4.6);
    expo_fit->SetParLimits(1, 2, 10);
    auto fit_result = statsyst_spectrum->Fit(expo_fit, "RMES");

    expo_fit->SetLineColor(i_colour);
    expo_fit->Draw("LSAME");

    double start = 2;
    double end = 25;//1500;
    double point_step = 0.500;
    int n_points = (end - start) / point_step;
    TGraphErrors* fit_interval = new TGraphErrors(n_points);
    fit_interval->SetFillColor(i_colour-9);
    fit_interval->SetFillStyle(3001);
    for (int i_point = 0; i_point < n_points; ++i_point) {
      fit_interval->SetPoint(i_point, start + i_point*point_step, 0);
    }
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(fit_interval, 0.68);
    fit_interval->Draw("4 SAME");
    expo_fit->Draw("LSAME");

    double fit_integral = expo_fit->Integral(min_fit_ranges[i_setting],max_fit_ranges[i_setting]);
    double fit_integral_error = expo_fit->IntegralError(min_fit_ranges[i_setting],max_fit_ranges[i_setting], fit_result->GetParams(), fit_result->GetCovarianceMatrix().GetMatrixArray());
    std::cout << "Integral: " << fit_integral << " +/- " << fit_integral_error << std::endl;

    double full_integral_min = 2;
    double full_integral_max = 25;
    double full_integral = expo_fit->Integral(full_integral_min, full_integral_max);
    double full_integral_error = expo_fit->IntegralError(full_integral_min, full_integral_max);
    std::cout << "Integral: " << full_integral << " +/- " << full_integral_error << std::endl;

    std::stringstream text;
    TLatex* latex = new TLatex();
    latex->SetTextColor(i_colour);
    latex->SetTextAlign(22);

    double latex_x = 0.5;
    double latex_y = 0.85;
    double latex_y_step = -0.05;

    text.str("");
    text << Particle;
    if (Particle != "Sum") {
      text << "s";
    }
    latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str());

    latex->SetTextSize(0.035);
    latex_y +=latex_y_step;
    text.str("");
    text << "Fit Range: " << std::fixed << std::setprecision(0) << min_fit_ranges[i_setting] << " MeV - " << max_fit_ranges[i_setting] << " MeV";    
    latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str()); 

    latex_y_step = -0.035;
    latex_y +=latex_y_step;
    text.str("");
    text << "#chi^2 / ndf = " << std::fixed << std::setprecision(2) << expo_fit->GetChisquare() << " / " << expo_fit->GetNDF();
    latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str());

    latex_y +=latex_y_step;
    text.str("");
    text << "T_{0} = " << std::fixed << std::setprecision(1) << expo_fit->GetParameter(1) << " #pm " << expo_fit->GetParError(1) << " MeV";
    latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str());
    
    latex_y +=latex_y_step;
    text.str("");
    text << "Integral (" << std::fixed << std::setprecision(0) << min_fit_ranges[i_setting] << " MeV - " << max_fit_ranges[i_setting] << " MeV) = "
	 << std::setprecision(4) << fit_integral << " #pm " << fit_integral_error << " per muon capture";    
    latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str()); 

    latex_y +=latex_y_step;
    text.str("");
    text << "Integral (" << std::fixed << std::setprecision(0) << full_integral_min << " MeV - " << full_integral_max << " MeV) = "
	 << std::setprecision(4) << full_integral << " #pm " << full_integral_error << " per muon capture";
    latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str());

    numbers_file << "\\newcommand\\Sib" << Particle << "ExpoFitT{$"<< std::fixed << std::setprecision(1) << expo_fit->GetParameter(1) << " \\pm " << expo_fit->GetParError(1) << "$}" << std::endl;
    numbers_file << "\\newcommand\\Sib" << Particle << "ExpoFitRate{$"<< std::fixed << std::setprecision(4) << full_integral << " \\pm " << full_integral_error << "$}" << std::endl;

    // c_all_log->cd();
    // TGraphErrors* for_all = (TGraphErrors*) statsyst_spectrum->Clone("for_all");
    // for_all->SetLineColor(i_colour);
    // if (i_setting==0) {
    //   for_all->Draw("APE");
    // }
    // else {
    //   for_all->Draw("PE SAME");
    // }
    // //    latex->DrawLatexNDC(0.6, 0.5, "AlCap #bf{#it{Preliminary} }");

    // leglabel.str("");
    // leglabel << leglabels[i_setting] << " (T_{0} = " << std::fixed << std::setprecision(1) << expo_fit->GetParameter(1) << " #pm " << expo_fit->GetParError(1) << " MeV)";
    // leg_for_all->AddEntry(for_all, leglabel.str().c_str(), "l");

    
    c_fits_only->cd();
    double thresh = 0.0;
    //    TF1* expo_fit_full = new TF1("expo_fit_full","[0]*(exp(-x/[1]))",0, 26);
    TF1* expo_fit_full = new TF1("expo_fit_full","[0]*(1 - [1]/x)^[2]*(exp(-x/[3]))",thresh, 50);
    expo_fit_full->SetParameter(0, expo_fit->GetParameter(0));
    expo_fit_full->SetParameter(1, thresh);
    expo_fit_full->SetParLimits(1, thresh, 5.0);
    expo_fit_full->SetParameter(2, 1.3);
    expo_fit_full->SetParLimits(2, 0, 5);
    expo_fit_full->SetParameter(3, expo_fit->GetParameter(1));
    expo_fit_full->SetLineStyle(kDashed);
    expo_fit_full->SetLineColor(i_colour);
    //    statsyst_spectrum->RemovePoint(0);
    //    statsyst_spectrum->RemovePoint(0);
    //    statsyst_spectrum->InsertPointBefore(0, 0.0, 0.0);//1e-12);
    //    for (int i_point = 0; i_point < statsyst_spectrum->GetN(); ++i_point) {
    //      std::cout << "X,Y = " << *(statsyst_spectrum->GetX()+i_point) << ", " << *(statsyst_spectrum->GetY()+i_point) << std::endl;
    //    }
    //    statsyst_spectrum->InsertPointBefore(1, 0.01, 1e-12);
    //    statsyst_spectrum->Fit(expo_fit_full, "RME0", "", 0.0, max_fit_ranges[i_setting]);
    
    if (i_setting == 0) {
      expo_fit_full->Draw("");
    }
    else {
      expo_fit_full->Draw("LSAME");
    }
    expo_fit->Draw("LSAME");

    //    particle_canvases[i_setting]->cd();
    //    expo_fit_full->Draw("LSAME");
    
    std::cout << "Integral (full fit, 2 -- 26 MeV) " << expo_fit_full->Integral(2, 26) << std::endl;
    std::cout << "Integral (expo fit, 2 -- 26 MeV) " << expo_fit->Integral(2,26) << " +/- " << expo_fit->IntegralError(2,26) << std::endl;

    const int n_max_points = 40;
    double peak_positions[n_max_points] = {0};
    double integrals[n_max_points] = {0};
    int counter = 0;
    for (double peak_pos = 0.0; peak_pos < min_fit_ranges[i_setting]; peak_pos += 0.5) {
      //      std::cout << Particle_names[i_setting] << ": Assume " << peak_pos << " MeV is peak" << std::endl;

      double main_integral = expo_fit->Integral(peak_pos,50);
      //      std::cout << "From fit: integral (" << peak_pos << " -- 50) = " << main_integral << std::endl;

      double extra_integral = peak_pos*expo_fit->Eval(peak_pos)*0.5;
      //      std::cout << "Low E Rate (0 -- " << peak_pos << ") = " << extra_integral << std::endl;

      double total_integral = main_integral + extra_integral;
      //      std::cout << "Total rate = " << total_integral << std::endl;
      peak_positions[counter] = peak_pos;
      integrals[counter] = total_integral;
      ++counter;
    }
    grIntegrals[i_setting] = new TGraph(counter, peak_positions, integrals);
    grIntegrals[i_setting]->SetMarkerColor(colours[i_setting]);
    grIntegrals[i_setting]->SetLineColor(colours[i_setting]);
    // for (int i_point = 0; i_point < grIntegrals[i_setting]->GetN(); ++i_point) {
    //   std::cout << "X,Y = " << *(grIntegrals[i_setting]->GetX()+i_point) << ", " << *(grIntegrals[i_setting]->GetY()+i_point) << std::endl;
    // }
    //    double fit_thresh = expo_fit_full->GetParameter(1);
    //    std::cout << "Integral (full_fit, " << fit_thresh << " -- 50 MeV) " << expo_fit_full->Integral(fit_thresh, 50) << std::endl;
    //    std::cout << "Integral " << expo_fit->Integral(0, 50) << " +/- " << expo_fit->IntegralError(0, 50) << std::endl;
  }
  c_all_log->cd();
  leg_for_all->Draw();
  numbers_file << std::endl;

  TCanvas* c_integrals = new TCanvas("c_integrals", "c_integrals");
  grIntegrals[3]->Draw("A*L");
  grIntegrals[3]->GetXaxis()->SetTitle("Assumed Spectrum Peak [MeV]");
  grIntegrals[3]->GetYaxis()->SetTitle("Total Emission Rate [particles / muon capture]");
  grIntegrals[3]->GetYaxis()->SetRangeUser(0.0, 0.3);
  grIntegrals[3]->GetXaxis()->SetRangeUser(0.0, 6.0);
  grIntegrals[2]->Draw("*L SAME");
  grIntegrals[1]->Draw("*L SAME");
  grIntegrals[0]->Draw("*L SAME");
  
  if (savedir != "") {
    for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
      std::string savename = savedir + "AlCapData_Si16bDataset_SpectrumFit_" + Particle_names[i_setting];

      std::string pdfname = savename + ".pdf";
      particle_canvases[i_setting]->SaveAs(pdfname.c_str());
      std::string pngname = savename + ".png";
      particle_canvases[i_setting]->SaveAs(pngname.c_str());
    }
  }
}
