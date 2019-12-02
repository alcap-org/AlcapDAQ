void Si16b_FinalPlot_NormalisedSpectrum_wSW() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();

  const int n_ranges = 8;
  double min_energies[n_ranges] = {0,    1400,  10000, 5000,   3500, 4000, 3500, 14000};
  double max_energies[n_ranges] = {26000,26000, 26000, 10000, 10000, 8000, 14000, 15000};
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

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(SW_gre, "Sobottka-Wills", "pl");
  SW_gre->GetFunction("tdr_fit")->SetLineColor(kBlack);
  leg->AddEntry(SW_gre->GetFunction("tdr_fit"), "S-W Fit", "l");

  for (int i_range = 0; i_range < n_ranges; ++i_range) {
    double min_energy = min_energies[i_range];
    double max_energy = max_energies[i_range];
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
  }

  //  std::string filename = "~/data/results/Si16b/unfold_newPP.root";
  std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse.root";
  //  std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_SiL1-2--6.root";
  //  std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_SiL1-7--10.root";
  //  std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_SiL1-11--15.root";
  //  std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_SiL1-2--14.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  
  const int n_settings = 5;
  std::string particle_names[n_settings] = {"proton", "deuteron", "triton", "alpha", "SiL3"};
  Int_t colours[n_settings] = {kRed, kCyan, kSpring, kMagenta, kGray};
  std::string leglabels[n_settings] = {"Si16b (proton)", "Si16b (deuterons)", "Si16b (tritons)", "Si16b (alphas)", "Si16b (SiL3 inc.)"};
  double rates[n_settings][n_ranges] = {0};
  double rate_errs[n_settings][n_ranges] = {0};

  int rebin_factor = 5;
  THStack* hStack = new THStack("hStack", "");
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {

    std::string i_particle_name = particle_names[i_setting];
    Int_t i_colour = colours[i_setting];

    std::string dirname = "FinalNormalisation_" + i_particle_name;
    if (i_particle_name != "SiL3") {
      //      dirname += "_PSel";
      dirname += "_TCutG";
    }
    //    std::string dirname = "FinalNormalisation_" + i_particle_name + "_TCutG";
    //  std::string i_histname = dirname + "/hNormalisedSpectrum";
    std::string i_histname = dirname + "_retune/hNormalisedSpectrum";

    TH1F* spectrum = (TH1F*) file->Get(i_histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << i_histname << std::endl;
      //      return;
      continue;
    }
    spectrum->Sumw2();

    spectrum->Rebin(rebin_factor);
    spectrum->Scale(1.0/rebin_factor);
    spectrum->SetStats(false);
    spectrum->SetLineColor(i_colour);
    //    spectrum->SetFillStyle(0);
    spectrum->SetLineWidth(2);
    //    spectrum->SetLineColor(kBlack);
    //    spectrum->SetFillColor(i_colour);
    //    spectrum->Draw("HIST E SAMES");
    leg->AddEntry(spectrum, leglabels[i_setting].c_str(), "l");

    /*    std::string fitname = dirname + "/spectral_fit";
    TF1* fit = (TF1*) file->Get(fitname.c_str());
    if (fit) {
      fit->SetLineWidth(2);
      fit->SetLineColor(kCyan);
      fit->Draw("LSAME");
    }
    */
    if (i_particle_name != "SiL3") {
      hStack->Add(spectrum);
    }

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
  }

  double total_rates[n_ranges] = {0};
  double total_rate_errs[n_ranges] = {0};
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    for (int i_range = 0; i_range < n_ranges; ++i_range) {
      double min_energy = min_energies[i_range];
      double max_energy = max_energies[i_range];

      std::cout << "AlCap, " << leglabels[i_setting] << ": Integral (" << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << rates[i_setting][i_range] << " +/- " << rate_errs[i_setting][i_range] << std::endl;
      total_rates[i_range] += rates[i_setting][i_range];

      total_rate_errs[i_range] += (rate_errs[i_setting][i_range]*rate_errs[i_setting][i_range]);
    }
  }
  
  for (int i_range = 0; i_range < n_ranges; ++i_range) {
    double min_energy = min_energies[i_range];
    double max_energy = max_energies[i_range];

    total_rate_errs[i_range] = std::sqrt(total_rate_errs[i_range]);
    
    std::cout << "Total AlCap: Integral (" << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << total_rates[i_range] << " +/- " << total_rate_errs[i_range] << std::endl;

    for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
      std::cout << "Ratio (" << leglabels[i_setting] << ") = " << rates[i_setting][i_range] / total_rates[i_range] << std::endl;
    }
  }

  
  //  hStack->Draw("HIST E SAMES nostack");
  hStack->Draw("HIST E SAMES");  
  leg->Draw();
}
