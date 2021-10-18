void Si16b_FinalPlot_NormalisedSpectrum_wAll_forPaper(std::string savedir = "") {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();

  TLegend* leg = new TLegend(0.45,0.55,0.80,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.04);
  leg->SetFillColor(kWhite);
  //  leg->SetHeader("#mu Si");

  const int n_axis_points = 2;
  double x_axis_points[n_axis_points] = {0, 50};
  double y_axis_points[n_axis_points] = {1e-6, 1};
  TGraph* gr_axes = new TGraph(n_axis_points, x_axis_points, y_axis_points);
  gr_axes->Draw("APE");
			       
  
  std::string SW_filename = "~/data/results/SiL3/SW-plots_betterErrors_MeV.root";
  //  std::string SW_filename = "~/data/results/Si16b/SW-plots-above1400keV.root";
  TFile* SW_file = new TFile(SW_filename.c_str(), "READ");
  TGraphErrors* SW_gre = (TGraphErrors*) SW_file->Get("Graph");
  SW_gre->SetLineColor(kBlue);
  SW_gre->SetLineWidth(2);
  SW_gre->SetMarkerColor(kBlue);
  SW_gre->SetMarkerStyle(kDot);//OpenSquare);
  SW_gre->Draw("PE SAME");
  SW_gre->SetTitle("Charged Particle Emission");
  SW_gre->GetXaxis()->SetRangeUser(0,26);
  SW_gre->GetXaxis()->SetTitle("Energy [MeV]");
  SW_gre->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per MeV");
  //  SW_gre->SetMarkerStyle(kFullTriangleUp);
  //  SW_gre->SetMarkerColor(kRed);
  //  SW_gre->SetLineColor(kBlack);
  SW_gre->GetFunction("tdr_fit")->SetLineColor(0);

  std::string Budyashov_filename = "~/data/results/Si16b/Budyashov-plots_MeV.root";
  TFile* Budyashov_file = new TFile(Budyashov_filename.c_str(), "READ");
  TGraphErrors* Budyashov_gre_proton = (TGraphErrors*) Budyashov_file->Get("proton");
  Budyashov_gre_proton->SetMarkerColor(kRed);
  Budyashov_gre_proton->SetMarkerStyle(kFullTriangleUp);
  Budyashov_gre_proton->SetLineColor(kRed);
  Budyashov_gre_proton->SetLineWidth(2);
  Budyashov_gre_proton->Draw("PE SAME");
  Budyashov_gre_proton->SetTitle("Charged Particle Emission");
  Budyashov_gre_proton->GetXaxis()->SetRangeUser(0,26);
  Budyashov_gre_proton->GetXaxis()->SetTitle("Energy [MeV]");
  Budyashov_gre_proton->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per MeV");

  TGraphErrors* Budyashov_gre_deuteron = (TGraphErrors*) Budyashov_file->Get("deuteron");
  Budyashov_gre_deuteron->SetMarkerColor(kBlue);
  Budyashov_gre_deuteron->SetMarkerStyle(kFullTriangleUp);
  Budyashov_gre_deuteron->SetLineColor(kBlue);
  Budyashov_gre_deuteron->SetLineWidth(2);
  Budyashov_gre_deuteron->Draw("PE SAME");
  //  leg->AddEntry(Budyashov_gre_sum, "Budyashov et al. (p + d)", "pl");


  const int n_recoils = 1;
  std::string recoils[n_recoils] = {"allRecoil"};//, "noRecoil"};
  std::string leglabels[n_recoils] = {"AlCap All Particles"};//, "without recoil correction"};
  Int_t colours[n_recoils] = {kBlack};
  std::string SiL3_filename = "~/data/results/SiL3/systematics_geq2TgtPulse_newPP20us_1_KFactor0-85_test.root";
  TFile* SiL3_file = new TFile(SiL3_filename.c_str(), "READ");
  
  for (int i_recoil = 0; i_recoil < n_recoils; ++i_recoil) {
    std::string recoil = recoils[i_recoil];
    Int_t colour = colours[i_recoil];
    
    std::string SiL3_dirname = "FinalSystPlot_TimeSlice3000_4000_" + recoil + "_500keVBins";
    //  std::string SiL3_dirname = "FinalSystPlot_TimeSlice2000_4000_noRecoil_500keVBins";
    //  std::string SiL3_dirname = "FinalSystPlot_TimeSlice2000_4000_nuRecoil_500keVBins";
    std::string SiL3_histname = SiL3_dirname + "/hFinalStatSyst";
    TGraphAsymmErrors* SiL3_hist = (TGraphAsymmErrors*) SiL3_file->Get(SiL3_histname.c_str());
    for (int i_point = 0; i_point < SiL3_hist->GetN(); ++i_point) {
      //      std::cout << "X,Y = " << *(SiL3_hist->GetX()+i_point) << ", " << *(SiL3_hist->GetY()+i_point) << std::endl;
      if (*(SiL3_hist->GetX()+i_point) > 26) {
	*(SiL3_hist->GetY()+i_point) = 0;
	*(SiL3_hist->GetEYlow()+i_point) = 0;
	*(SiL3_hist->GetEYhigh()+i_point) = 0;
      }
      //      std::cout << "X,Y = " << *(SiL3_hist->GetX()+i_point) << ", " << *(SiL3_hist->GetY()+i_point) << std::endl;
    }
    if (i_recoil == 0) {
      SiL3_hist->Draw("PE SAME");
    }
    else {
      SiL3_hist->Draw("PE SAME");
    }
    //  SiL3_hist->SetStats(false);
    SiL3_hist->SetLineWidth(2);
    SiL3_hist->SetLineColor(colour);
    SiL3_hist->SetTitle("Charged Particle Emission");
    SiL3_hist->GetXaxis()->SetRangeUser(2,17);
    SiL3_hist->GetXaxis()->SetTitle("Energy [MeV]");
    SiL3_hist->GetYaxis()->SetTitle("Particles / muon capture / MeV");
    SiL3_hist->GetXaxis()->SetTitleOffset(0.9);
    SiL3_hist->GetYaxis()->SetTitleOffset(1.0);
    //    SiL3_hist->GetYaxis()->SetRangeUser(1e-8, 1e-3);
    SiL3_hist->GetYaxis()->SetRangeUser(5e-4, 1e-1);
    SiL3_hist->SetMarkerStyle(kOpenCircle);
    //  SiL3_hist->SetMaximum(1e-3);
    //  SiL3_hist->SetMinimum(1e-8);
    // for (int i_range = 0; i_range < n_ranges; ++i_range) {
    //   double min_energy = min_energies[i_range];
    //   double max_energy = max_energies[i_range];
    //   int min_energy_bin = SiL3_hist->GetXaxis()->FindBin(min_energy);
    //   int max_energy_bin = SiL3_hist->GetXaxis()->FindBin(max_energy) - 1;
    //   double error = -1;
    //   double integral = SiL3_hist->IntegralAndError(min_energy_bin, max_energy_bin, error, "width");
    //   SiL3_rate[i_range] = integral;
    //   SiL3_rate_err[i_range] = error;
    // }

    std::string leglabel = leglabels[i_recoil];
    leg->AddEntry(SiL3_hist, leglabel.c_str(), "lp");
  }
  
  //  std::string filename = "~/data/results/Si16b/unfold_newPP.root";
  std::string filename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_JohnVetoEff_1.root";
  //    std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_SiL1-2--6.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  
  const int n_settings = 2;
  std::string particle_names[n_settings] = {"proton", "deuteron"};//{"Sum"};
  Int_t si16b_colours[n_settings] = {kRed, kBlue};//Black, kBlack};//{kBlack};
  Int_t si16b_styles[n_settings] = {kOpenCircle, kOpenCircle};//{kBlack, kBlack};//{kBlack};
  std::string leglabels_Si16b[n_settings] = {"AlCap Protons", "AlCap Deuterons"};//{"passive Si target (p+d+t+#alpha)"};

  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {

    std::string i_particle_name = particle_names[i_setting];
    Int_t i_colour = si16b_colours[i_setting];

    std::string dirname = "FinalSystPlot_" + i_particle_name + "_TCutG_2sig_layerCoinc500ns_tGT0ns";
    std::string i_histname = dirname + "/hFinalStatSyst";

    TGraphAsymmErrors* spectrum = (TGraphAsymmErrors*) file->Get(i_histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << i_histname << std::endl;
      //      return;
      continue;
    }
    for (int i_point = 0; i_point < spectrum->GetN(); ++i_point) {
      //      std::cout << "X,Y = " << *(spectrum->GetX()+i_point) << ", " << *(spectrum->GetY()+i_point) << std::endl;
      if (*(spectrum->GetX()+i_point) > 17) {
	*(spectrum->GetY()+i_point) = 0;
	*(spectrum->GetEYlow()+i_point) = 0;
	*(spectrum->GetEYhigh()+i_point) = 0;
      }
      //      std::cout << "X,Y = " << *(spectrum->GetX()+i_point) << ", " << *(spectrum->GetY()+i_point) << std::endl;
    }
    //    spectrum->SetStats(false);
    spectrum->SetLineColor(si16b_colours[i_setting]);
    spectrum->SetFillStyle(0);
    spectrum->SetFillColor(0);
    spectrum->SetLineWidth(2);
    spectrum->SetMarkerStyle(si16b_styles[i_setting]);//kOpenCircle);
    spectrum->SetMarkerColor(si16b_colours[i_setting]);
    //    spectrum->SetLineColor(kBlack);
    //    spectrum->SetFillColor(i_colour);
    spectrum->Draw("PE SAMEs");
    leg->AddEntry(spectrum, leglabels_Si16b[i_setting].c_str(), "lp");

  }
  leg->AddEntry(SW_gre, "Sobottka and Wills All Particles", "pl");
  leg->AddEntry(Budyashov_gre_proton, "Budyashov et al. Protons", "pl");
  leg->AddEntry(Budyashov_gre_deuteron, "Budyashov et al. Deuterons", "pl");
  leg->Draw();

  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.17, 0.75, "AlCap #bf{#it{Preliminary}}");
  
  if (savedir != "") {
    std::string savename = savedir + "AlCapData_Si16bDataset_NormalisedSpectra_wAll_forPaper";
    std::string pdfname = savename + ".pdf";
    c1->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c1->SaveAs(pngname.c_str());

    // savename = savedir + "AlCapData_NormalisedSpectraComparison_Unstacked";    
    // pdfname = savename + ".pdf";
    // //    c_unstacked->SaveAs(pdfname.c_str());
    // pngname = savename + ".png";
    // //    c_unstacked->SaveAs(pngname.c_str());
  }
}
