void Si16b_FinalPlot_NormalisedSpectrum_wAll(std::string savedir = "") {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();

  TLegend* leg = new TLegend(0.45,0.55,0.85,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);

  const int n_ranges = 6;
  double min_energies[n_ranges] = {1400, 15000, 15000, 16000, 15000, 8000};//3500, 4000, 3500};//0,    1400,  10000, 5000,   3500, 4000, 3500, 14000};
  double max_energies[n_ranges] = {26000, 16000, 17000, 17000, 18000, 9000};//10000, 8000, 15000};//26000,26000, 26000, 10000, 10000, 8000, 14000, 15000};
  double SiL3_rate[n_ranges] = {0};
  double SiL3_rate_err[n_ranges] = {0};
  double SW_rate[n_ranges] = {0};
    
  std::string SW_filename = "~/data/results/SiL3/SW-plots.root";
  //  std::string SW_filename = "~/data/results/Si16b/SW-plots-above1400keV.root";
  TFile* SW_file = new TFile(SW_filename.c_str(), "READ");
  TGraphErrors* SW_gre = (TGraphErrors*) SW_file->Get("Graph");
  SW_gre->Draw("APE");
  SW_gre->SetTitle("Charged Particle Emission");
  SW_gre->GetXaxis()->SetRangeUser(0,26000);
  SW_gre->GetXaxis()->SetTitle("Energy [keV]");
  SW_gre->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per keV");
  //  SW_gre->SetMarkerStyle(kFullTriangleUp);
  //  SW_gre->SetMarkerColor(kRed);
  //  SW_gre->SetLineColor(kBlack);
  SW_gre->GetFunction("tdr_fit")->SetLineColor(0);
  leg->AddEntry(SW_gre, "Sobottka-Wills", "pl");

  std::string Budyashov_filename = "~/data/results/Si16b/Budyashov-plots.root";
  TFile* Budyashov_file = new TFile(Budyashov_filename.c_str(), "READ");
  TGraphErrors* Budyashov_gre_sum = (TGraphErrors*) Budyashov_file->Get("sum");
  Budyashov_gre_sum->SetMarkerColor(kBlue);
  Budyashov_gre_sum->SetMarkerStyle(kFullTriangleUp);
  Budyashov_gre_sum->SetLineColor(kBlue);
  Budyashov_gre_sum->SetLineWidth(2);
  Budyashov_gre_sum->Draw("PE SAME");
  Budyashov_gre_sum->SetTitle("Charged Particle Emission");
  Budyashov_gre_sum->GetXaxis()->SetRangeUser(0,26000);
  Budyashov_gre_sum->GetXaxis()->SetTitle("Energy [keV]");
  Budyashov_gre_sum->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per keV");
  leg->AddEntry(Budyashov_gre_sum, "Budyashov et al. (p + d)", "pl");

  
  
  std::string SiL3_filename = "~/data/results/SiL3/unfold_geq2TgtPulse_newPP20us.root";
  TFile* SiL3_file = new TFile(SiL3_filename.c_str(), "READ");
  TH1F* SiL3_hist = (TH1F*) SiL3_file->Get("FinalNormalisation_TimeSlice2000_4000_allRecoil/hNormalisedSpectrum");
  SiL3_hist->SetStats(false);
  SiL3_hist->SetLineColor(kRed);
  SiL3_hist->SetTitle("Charged Particle Emission");
  SiL3_hist->GetXaxis()->SetRangeUser(0,26000);
  SiL3_hist->GetXaxis()->SetTitle("Energy [keV]");
  SiL3_hist->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per keV");
  SiL3_hist->SetMaximum(1e-3);
  SiL3_hist->SetMinimum(1e-8);
  for (int i_range = 0; i_range < n_ranges; ++i_range) {
    double min_energy = min_energies[i_range];
    double max_energy = max_energies[i_range];
    int min_energy_bin = SiL3_hist->GetXaxis()->FindBin(min_energy);
    int max_energy_bin = SiL3_hist->GetXaxis()->FindBin(max_energy) - 1;
    double error = -1;
    double integral = SiL3_hist->IntegralAndError(min_energy_bin, max_energy_bin, error, "width");
    SiL3_rate[i_range] = integral;
    SiL3_rate_err[i_range] = error;
  }
  int rebin_SiL3 = 2;
  SiL3_hist->Rebin(rebin_SiL3);
  SiL3_hist->Scale(1.0 / rebin_SiL3);
  SiL3_hist->Draw("HIST E SAME");
  leg->AddEntry(SiL3_hist, "SiL3 (active target)", "l");

  //  std::string filename = "~/data/results/Si16b/unfold_newPP.root";
  std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse.root";
  //    std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_SiL1-2--6.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  
  const int n_settings = 5;
  std::string particle_names[n_settings] = {"proton", "deuteron", "triton", "alpha", "SiL3"};
  Int_t colours[n_settings] = {kRed, kCyan, kMagenta, kSpring, kGray};
  std::string leglabels[n_settings] = {"Si16b (proton)", "Si16b (deuterons)", "Si16b (tritons)", "Si16b (alphas)", "Si16b (SiL3 inc.)"};
  double rates[n_settings][n_ranges] = {0};
  double rate_errs[n_settings][n_ranges] = {0};
  
  int rebin_factors[n_settings] = {1, 1, 1, 1, 1};
  THStack* hStack = new THStack("hStack", "");
  TH1F* hSi16b_Total = NULL;
  TH1F* hSi16b_SiL3Inc = NULL;
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {

    std::string i_particle_name = particle_names[i_setting];
    Int_t i_colour = colours[i_setting];

    std::string dirname = "FinalNormalisation_" + i_particle_name;
    if (i_particle_name != "SiL3") {
      if (i_particle_name == "alpha") {
	//	dirname += "_TCutG";
	dirname += "_PSel";
      }
      else {
	dirname += "_PSel";
	//	dirname += "_TCutG";
      }
    }
    //    else {
    //      dirname += "_PSel";
    //    }
    //    std::string i_histname = dirname + "_retune/hNormalisedSpectrum";
    std::string i_histname = dirname + "/hNormalisedSpectrum";

    TH1F* spectrum = (TH1F*) file->Get(i_histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << i_histname << std::endl;
      //      return;
      continue;
    }
    spectrum->Sumw2();
    
    int rebin_factor = rebin_factors[i_setting];
    spectrum->Rebin(rebin_factor);
    spectrum->Scale(1.0/rebin_factor);
    spectrum->SetStats(false);
    spectrum->SetLineColor(i_colour);
    spectrum->SetFillStyle(0);
    spectrum->SetFillColor(0);
    spectrum->SetLineWidth(2);
    alcaphistogram(spectrum);
    //    spectrum->SetLineColor(kBlack);
    //    spectrum->SetFillColor(i_colour);
    //    spectrum->Draw("HIST E SAMES");
    //    leg->AddEntry(spectrum, leglabels[i_setting].c_str(), "l");

    /*    std::string fitname = dirname + "/spectral_fit";
    TF1* fit = (TF1*) file->Get(fitname.c_str());
    if (fit) {
      fit->SetLineWidth(2);
      fit->SetLineColor(kCyan);
      fit->Draw("LSAME");
    }
    */
    for (int i_range = 0; i_range < n_ranges; ++i_range) {
      double min_energy = min_energies[i_range];
      double max_energy = max_energies[i_range];
      int min_energy_bin = spectrum->GetXaxis()->FindBin(min_energy);
      int max_energy_bin = spectrum->GetXaxis()->FindBin(max_energy) - 1;
      double error = -1;
      double integral = spectrum->IntegralAndError(min_energy_bin, max_energy_bin, error, "width");
      rates[i_setting][i_range] = integral;
      rate_errs[i_setting][i_range] = error;
    }

    if (i_particle_name != "SiL3") {

      /*
      double additional_error = 0.50;
      for (int i_bin = 1; i_bin <= spectrum->GetNbinsX(); ++i_bin) {
	double old_bin_content = spectrum->GetBinContent(i_bin);
	double old_bin_error = spectrum->GetBinError(i_bin);
	double old_bin_frac_error = old_bin_error / old_bin_content;
	double new_bin_frac_error = std::sqrt(additional_error*additional_error + old_bin_frac_error*old_bin_frac_error);
	double new_bin_error = new_bin_frac_error*old_bin_content;
	std::cout << "Old: " << old_bin_content << " +/- " << old_bin_error << " (" << old_bin_frac_error*100 << "%)" << std::endl;
	std::cout << "New: " << old_bin_content << " +/- " << new_bin_error << " (" << new_bin_frac_error*100 << "%)" << std::endl;
	spectrum->SetBinError(i_bin, new_bin_error);
      }
      */
      
      hStack->Add(spectrum);

      if (!hSi16b_Total) {
	hSi16b_Total = (TH1F*) spectrum->Clone("hSi16b_Total");
	hSi16b_Total->SetLineColor(kMagenta);
      }
      else {
	hSi16b_Total->Add(spectrum);
      }
      
      //      spectrum->Draw("HIST E SAME");
      /*
      if (i_particle_name != "alpha") {
	spectrum->Fit("expo", "+", "", 5000, 15000);
      }
      else {
	spectrum->Fit("expo", "+", "", 17000, 18000);
      }
      if (spectrum->GetFunction("expo")) {
	double eval_lo = spectrum->GetFunction("expo")->Eval(min_energies[0]);
	double eval_hi = spectrum->GetFunction("expo")->Eval(max_energies[0]);
	std::cout << "from Fit: " << ((eval_lo + eval_hi)/2.0) * 1000 << std::endl;
	spectrum->GetFunction("expo")->Draw("LSAME");
      }
      */
    }
    else {
      hSi16b_SiL3Inc = spectrum;
    }
  }

  double total_rates[n_ranges] = {0};
  double total_rate_errs[n_ranges] = {0};
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    for (int i_range = 0; i_range < n_ranges; ++i_range) {
      double min_energy = min_energies[i_range];
      double max_energy = max_energies[i_range];
      
      std::cout << "AlCap, " << leglabels[i_setting] << ": Integral (" << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << rates[i_setting][i_range] << " +/- " << rate_errs[i_setting][i_range] << std::endl;
      
      if (particle_names[i_setting] != "SiL3") {
	total_rates[i_range] += rates[i_setting][i_range];	  
	total_rate_errs[i_range] += (rate_errs[i_setting][i_range]*rate_errs[i_setting][i_range]);
      }	
    }
  }
  
  for (int i_range = 0; i_range < n_ranges; ++i_range) {
    double min_energy = min_energies[i_range];
    double max_energy = max_energies[i_range];

    total_rate_errs[i_range] = std::sqrt(total_rate_errs[i_range]);

    std::cout << "Total SiL3 integral ("  << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << SiL3_rate[i_range] << " +/- " << SiL3_rate_err[i_range] << std::endl;
    std::cout << "Total Si16b: Integral (" << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << total_rates[i_range] << " +/- " << total_rate_errs[i_range] << std::endl;

    for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
      if (particle_names[i_setting] != "SiL3") {
	std::cout << "Ratio (" << leglabels[i_setting] << ") = " << rates[i_setting][i_range] / total_rates[i_range] << std::endl;
      }
    }

    int min_bin_stack = hSi16b_Total->GetXaxis()->FindBin(min_energy);
    int max_bin_stack = hSi16b_Total->GetXaxis()->FindBin(max_energy)-1;
    double stack_integral_error = 0;
    double stack_integral = hSi16b_Total->IntegralAndError(min_bin_stack, max_bin_stack, stack_integral_error, "width");
    std::cout << "hSi16b_Total (" << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << stack_integral << " +/- " << stack_integral_error << std::endl;

    bool start_integral = false;
    double SW_integral = 0;
    double SW_error = 0;
    for (int i_element = 0; i_element < SW_gre->GetN(); ++i_element) {
      if (*(SW_gre->GetX()+i_element) > min_energy) {
        start_integral = true;
      }
      if (start_integral) {
        double previous_x = *(SW_gre->GetX()+i_element-1);
        double this_x = *(SW_gre->GetX()+i_element);
        double width_x = this_x - previous_x;
	if (width_x == 0) {
	  continue;
	}
	double this_y = *(SW_gre->GetY()+i_element);
	double this_err = *(SW_gre->GetEY()+i_element);

	double this_integral = this_y * width_x;
	double this_integral_error = (this_err/this_y)*this_integral;
	SW_integral += this_integral;
	SW_error += (this_integral_error * this_integral_error);

	if (i_range == n_ranges-1) {
	  std::cout << "AE; x = " << this_x << ", w = " << width_x << ", y = " << this_y << " +/- " << this_err
		    << ", integral = " << this_integral << " +/- " << this_integral_error
		    << ", (total err)^2 so far = " << SW_error
		    << ", total err so far = " << std::sqrt(SW_error) << std::endl;
	}
      }
      if (*(SW_gre->GetX()+i_element) > max_energy) {
        break;
      }
    }
    SW_error = std::sqrt(SW_error);
    SW_rate[i_range] = SW_integral;
    std::cout << "S-W Integral (" << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << SW_integral << " +/- " << SW_error << std::endl;

    /*
    TLine* min_line = new TLine(min_energy, 0, min_energy, 1);
    min_line->SetLineColor(kRed);
    min_line->SetLineWidth(2);
    min_line->Draw("LSAME");

    TLine* max_line = new TLine(max_energy, 0, max_energy, 1);
    max_line->SetLineColor(kRed);
    max_line->SetLineWidth(2);
    max_line->Draw("LSAME");
    */
  }

  //  hStack->GetXaxis()->SetRangeUser(0,26000);
  //  hStack->GetXaxis()->SetTitle("Energy [keV]");
  //  hStack->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per keV");
  hStack->SetMaximum(1e-3);
  hStack->SetMinimum(1e-8);
  //  hStack->Draw("HIST E SAMES nostack");
  //  hStack->Draw("HIST E");
  if (hSi16b_SiL3Inc) {
    hSi16b_SiL3Inc->Draw("HIST E SAME");
  }
  alcaphistogram(hSi16b_Total);
  alcaphistogram(SiL3_hist);
  leg->AddEntry(hSi16b_Total, "Si16b (p + d + t + #alpha)", "pl");

  SW_gre->Draw("APE");
  Budyashov_gre_sum->Draw("PE SAME");
  hSi16b_Total->Draw("HIST E SAME");
  SiL3_hist->Draw("HIST E SAME");

  alcapPreliminary(SiL3_hist);

  leg->Draw();
  if (savedir != "") {
    std::string savename = savedir + "AlCapData_NormalisedSpectraComparison_wLiterature";
    std::string pdfname = savename + ".pdf";
    c1->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c1->SaveAs(pngname.c_str());
  }
}
