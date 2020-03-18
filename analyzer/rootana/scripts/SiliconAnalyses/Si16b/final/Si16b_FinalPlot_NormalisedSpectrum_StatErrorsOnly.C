void Si16b_FinalPlot_NormalisedSpectrum_StatErrorsOnly(std::string savedir = "") {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
    
  std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  
  const int n_settings = 4;
  std::string particle_names[n_settings] = {"proton", "deuteron", "triton", "alpha"};
  std::string Particle_names[n_settings] = {"Proton", "Deuteron", "Triton", "Alpha"};
  Int_t colours[n_settings] = {kRed, kCyan, kMagenta, kSpring};
  std::string leglabels[n_settings] = {"protons", "deuterons", "tritons", "alphas"};

  const int n_ranges = 3;
  double min_energies[n_ranges] = {15000, 15000, 16000};
  double max_energies[n_ranges] = {17000, 16000, 17000};
  /*  const int n_ranges = 7;
  double min_energies[n_ranges] = {0};
  double max_energies[n_ranges] = {0};
  for (int i_energy = 0; i_energy < n_ranges; ++i_energy) {
    min_energies[i_energy] = i_energy*1000 + 10000;
    max_energies[i_energy] = min_energies[i_energy] + 1000;;
  }
  */
  
  double rates[n_settings][n_ranges] = {0};
  double rate_errs[n_settings][n_ranges] = {0};

  TLegend* leg = new TLegend(0.60,0.65,0.85,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);
  
  int rebin_factor = 1;
  std::stringstream time_setting_str, leglabel;
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string i_particle_name = particle_names[i_setting];
    Int_t i_colour = colours[i_setting];

    std::string i_dirname = "FinalNormalisation_" + i_particle_name + "_TCutG";
    std::string i_histname = i_dirname + "/hNormalisedSpectrum";

    TH1F* spectrum = (TH1F*) file->Get(i_histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << i_histname << std::endl;
      return;
    }
    spectrum->Sumw2();

    spectrum->SetTitle("Si16b Dataset");
    spectrum->Rebin(rebin_factor);
    spectrum->Scale(1.0/rebin_factor);
    spectrum->SetStats(false);
    spectrum->SetLineColor(i_colour);
    spectrum->GetXaxis()->SetRangeUser(0,26000);
    //    spectrum->GetXaxis()->SetRangeUser(2000,16000);
    spectrum->SetMinimum(1e-8);
    spectrum->SetMaximum(3e-5);
    spectrum->Draw("HIST E SAMES");

    alcaphistogram(spectrum);
    if (i_setting == 0) {
      alcapPreliminary(spectrum);
      spectrum->SetDrawOption("HIST E1");
    }
    else {
      spectrum->SetDrawOption("HIST E1 SAME");
    }

    leglabel.str("");
    leglabel << leglabels[i_setting];
    leg->AddEntry(spectrum, leglabel.str().c_str(), "l");

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
  for (int i_range = 0; i_range < n_ranges; ++i_range) {
    double min_energy = min_energies[i_range];
    double max_energy = max_energies[i_range];

    for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
      std::cout << "AlCap, " << leglabels[i_setting] << ": Integral (" << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << rates[i_setting][i_range] << " +/- " << rate_errs[i_setting][i_range] <<  " (" << (rate_errs[i_setting][i_range]/rates[i_setting][i_range])*100 << "%)" << std::endl;

      total_rates[i_range] += rates[i_setting][i_range];	  
      total_rate_errs[i_range] += (rate_errs[i_setting][i_range]*rate_errs[i_setting][i_range]);
    }

    total_rate_errs[i_range] = std::sqrt(total_rate_errs[i_range]);
    std::cout << "Total Si16b: Integral (" << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << total_rates[i_range] << " +/- " << total_rate_errs[i_range] << " (" << (total_rate_errs[i_range]/total_rates[i_range])*100 << "%)" << std::endl;

    for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
      std::string particle = particle_names[i_setting];
      std::string Particle = Particle_names[i_setting];
      double ratio = rates[i_setting][i_range] / total_rates[i_range];
      double ratio_err = ratio*std::sqrt( (rate_errs[i_setting][i_range]/rates[i_setting][i_range])*(rate_errs[i_setting][i_range]/rates[i_setting][i_range]) +
					  (total_rate_errs[i_range]/total_rates[i_range])*(total_rate_errs[i_range]/total_rates[i_range]) );
      std::cout << "Ratio (" << leglabels[i_setting] << ") = " << ratio << " +/- " << ratio_err << std::endl;
    }
  }
  
  leg->Draw();

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_Si16bDataset_NormalisedSpectrum";

    std::string pdfname = savename + ".pdf";
    c1->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c1->SaveAs(pngname.c_str());
  }
}
