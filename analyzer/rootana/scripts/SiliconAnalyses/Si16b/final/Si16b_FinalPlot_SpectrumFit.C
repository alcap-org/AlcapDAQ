//#include "../../Utils/IntegrateRates.C"

void Si16b_FinalPlot_SpectrumFit(std::string savedir = "", std::ostream& numbers_file = std::cout) {
    
  std::string filename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_3sigma.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  double x_max = 25000;
  
  const int n_ranges = 1;
  double min_energies[n_ranges] = {15000};
  double max_energies[n_ranges] = {16000};
  
  const int n_settings = 5;
  TCanvas* particle_canvases[n_settings] = {0};
  std::string particle_names[n_settings] = {"proton", "deuteron", "triton", "alpha", "Sum"};
  std::string Particle_names[n_settings] = {"Proton", "Deuteron", "Triton", "Alpha", "Sum"};
  Int_t colours[n_settings] = {kRed, kCyan, kMagenta, kSpring, kBlue};
  std::string leglabels[n_settings] = {"protons", "deuterons", "tritons", "alphas", "sum"};
  double min_fit_ranges[n_settings] = {3000, 5000, 6000, 13000, 3000};
  double max_fit_ranges[n_settings] = {17000, 16000, 17000, 20000, 17000};
  
  TLegend* leg_for_all = new TLegend(0.60,0.65,0.85,0.85);
  leg_for_all->SetBorderSize(0);
  leg_for_all->SetTextSize(0.035);
  leg_for_all->SetFillColor(kWhite);

  std::stringstream leglabel;
  TCanvas* c_all_log = new TCanvas();
  c_all_log->SetLogy();

  TCanvas* c_fits_only = new TCanvas();
  
  numbers_file << "% Si16b_FinalPlot_SpectrumFit.C" << std::endl;
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    particle_canvases[i_setting] = new TCanvas();
    particle_canvases[i_setting]->SetLogy();
    
    std::string particle = particle_names[i_setting];
    std::string Particle = Particle_names[i_setting];
    Int_t i_colour = colours[i_setting];

    std::string i_dirname = "FinalSystPlot_" + particle + "_TCutG";
    std::string i_statsystname = i_dirname + "/hFinalStatSyst";
    TGraphAsymmErrors* statsyst_spectrum = (TGraphAsymmErrors*) file->Get(i_statsystname.c_str());
    if (!statsyst_spectrum) {
      std::cout << "Error: Problem getting statsyst_spectrum " << i_statsystname << std::endl;
      return;
    }
    statsyst_spectrum->Draw("APE");
    statsyst_spectrum->GetYaxis()->SetRangeUser(1e-9, 5e-3);
    statsyst_spectrum->GetXaxis()->SetRangeUser(0, 25000);
    gPad->Update();

    TF1* expo_fit = new TF1("expo_fit","[0]*(exp(-x/[1]))",min_fit_ranges[i_setting],max_fit_ranges[i_setting]);//27000);
    expo_fit->SetParName(0, "N");
    expo_fit->SetParName(1, "T_{0}");
    expo_fit->SetParameter(0, 1e-6);
    expo_fit->SetParLimits(0, 1e-7, 1e-4);
    expo_fit->SetParameter(1, 4600);
    expo_fit->SetParLimits(1, 2000, 10000);
    statsyst_spectrum->Fit(expo_fit, "RME");

    expo_fit->SetLineColor(i_colour);
    expo_fit->Draw("LSAME");

    double start = 2000;
    double end = 25000;//1500;
    double point_step = 500;
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
    double fit_integral_error = expo_fit->IntegralError(min_fit_ranges[i_setting],max_fit_ranges[i_setting]);
    std::cout << "Integral: " << fit_integral << " +/- " << fit_integral_error << std::endl;

    double full_integral_min = 2000;
    double full_integral_max = 25000;
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
    text << "Fit Range: " << std::fixed << std::setprecision(0) << min_fit_ranges[i_setting]/1e3 << " MeV - " << max_fit_ranges[i_setting]/1e3 << " MeV";    
    latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str()); 

    latex_y_step = -0.035;
    latex_y +=latex_y_step;
    text.str("");
    text << "#chi^2 / ndf = " << std::fixed << std::setprecision(2) << expo_fit->GetChisquare() << " / " << expo_fit->GetNDF();
    latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str());

    latex_y +=latex_y_step;
    text.str("");
    text << "T_{0} = " << std::fixed << std::setprecision(1) << expo_fit->GetParameter(1)/1e3 << " #pm " << expo_fit->GetParError(1)/1e3 << " MeV";
    latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str());
    
    latex_y +=latex_y_step;
    text.str("");
    text << "Integral (" << std::fixed << std::setprecision(0) << min_fit_ranges[i_setting]/1e3 << " MeV - " << max_fit_ranges[i_setting]/1e3 << " MeV) = "
	 << std::setprecision(4) << fit_integral << " #pm " << fit_integral_error << " per muon capture";    
    latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str()); 

    latex_y +=latex_y_step;
    text.str("");
    text << "Integral (" << std::fixed << std::setprecision(0) << full_integral_min/1e3 << " MeV - " << full_integral_max/1e3 << " MeV) = "
	 << std::setprecision(4) << full_integral << " #pm " << full_integral_error << " per muon capture";
    latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str());

    numbers_file << "\\newcommand\\Sib" << Particle << "ExpoFitT{$"<< std::fixed << std::setprecision(1) << expo_fit->GetParameter(1)/1e3 << " \\pm " << expo_fit->GetParError(1)/1e3 << "$}" << std::endl;
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
    // leglabel << leglabels[i_setting] << " (T_{0} = " << std::fixed << std::setprecision(1) << expo_fit->GetParameter(1)/1e3 << " #pm " << expo_fit->GetParError(1)/1e3 << " MeV)";
    // leg_for_all->AddEntry(for_all, leglabel.str().c_str(), "l");

    
    c_fits_only->cd();
    TF1* expo_fit_full = new TF1("expo_fit_full","[0]*(exp(-x/[1]))",0, 26000);
    expo_fit_full->SetParameter(0, expo_fit->GetParameter(0));
    expo_fit_full->SetParameter(1, expo_fit->GetParameter(1));
    expo_fit_full->SetLineStyle(kDashed);
    expo_fit_full->SetLineColor(i_colour);

    if (i_setting == 0) {
      expo_fit_full->Draw("");
    }
    else {
      expo_fit_full->Draw("LSAME");
    }
    expo_fit->Draw("LSAME");
    
    std::cout << "Integral (full) " << expo_fit_full->Integral(2000, 26000) << std::endl;
    std::cout << "Integral " << expo_fit->Integral(2000,26000) << " +/- " << expo_fit->IntegralError(2000,26000) << std::endl;
  }
  c_all_log->cd();
  leg_for_all->Draw();
  numbers_file << std::endl;
  
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
