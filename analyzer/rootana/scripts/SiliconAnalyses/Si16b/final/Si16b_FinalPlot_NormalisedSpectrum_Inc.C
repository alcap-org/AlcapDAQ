void Si16b_FinalPlot_NormalisedSpectrum_Inc() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);


  const int n_ranges = 1;
  double min_energies[n_ranges] = {10000};//0,    1400,  10000, 5000,   3500, 4000, 3500, 14000};
  double max_energies[n_ranges] = {14000};//26000,26000, 26000, 10000, 10000, 8000, 14000, 15000};
  double SiL3_rate[n_ranges] = {0};
  double SiL3_rate_err[n_ranges] = {0};
  /*
  std::string SiL3_filename = "~/data/results/SiL3/unfold_geq2TgtPulse_newPP20us.root";
  TFile* SiL3_file = new TFile(SiL3_filename.c_str(), "READ");
  TH1F* SiL3_hist = (TH1F*) SiL3_file->Get("FinalNormalisation_TimeSlice2000_4000/hNormalisedSpectrum");
  int rebin_SiL3 = 5;
  SiL3_hist->Rebin(rebin_SiL3);
  SiL3_hist->Scale(1.0 / rebin_SiL3);
  SiL3_hist->Draw("HIST E");
  SiL3_hist->SetLineColor(kBlack);
  SiL3_hist->SetTitle("Charged Particle Emission");
  SiL3_hist->GetXaxis()->SetRangeUser(0,26000);
  SiL3_hist->GetXaxis()->SetTitle("Energy [keV]");
  SiL3_hist->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per keV");
  //  SiL3_hist->SetMinimum(1e-9);
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
  leg->AddEntry(SiL3_hist, "SiL3 (active target)", "l");
  */

  //  std::string filename = "~/data/results/Si16b/unfold_newPP.root";
  std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse.root";
  //    std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_SiL1-2--6.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  
  const int n_settings = 2;
  std::string particle_names[n_settings] = {"SiL3", "SiR12"};
  Int_t colours[n_settings] = {kGray, kRed};
  std::string leglabels[n_settings] = {"Si16b (SiL3 inc.)", "Si16b (SiR12 inc.)"};
  double rates[n_settings][n_ranges] = {0};
  double rate_errs[n_settings][n_ranges] = {0};
  
  int rebin_factors[n_settings] = {10, 50};
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {

    std::string i_particle_name = particle_names[i_setting];
    Int_t i_colour = colours[i_setting];
    int rebin_factor = rebin_factors[i_setting];

    //    std::string dirname = "FinalNormalisation_" + i_particle_name;
    //    std::string i_histname = dirname + "_retune/hNormalisedSpectrum";
    //    std::string dirname = "TimeCut_" + i_particle_name;
    std::string dirname = "GeomAcceptance_" + i_particle_name;
    std::string i_histname = dirname + "/hCorrectedSpectrum";

    TH1F* spectrum = (TH1F*) file->Get(i_histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << i_histname << std::endl;
      //      return;
      continue;
    }
    spectrum->Sumw2();

    spectrum->Rebin(rebin_factor);
    spectrum->Scale(1.0 / spectrum->GetXaxis()->GetBinWidth(1));
    //    spectrum->Scale(1.0/rebin_factor);
    spectrum->SetStats(false);
    spectrum->SetLineColor(i_colour);
    //    spectrum->SetFillStyle(0);
    spectrum->SetLineWidth(2);
    //    spectrum->SetLineColor(kBlack);
    //    spectrum->SetFillColor(i_colour);
    spectrum->Draw("HIST E SAMES");
    leg->AddEntry(spectrum, leglabels[i_setting].c_str(), "l");

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

  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    for (int i_range = 0; i_range < n_ranges; ++i_range) {
      double min_energy = min_energies[i_range];
      double max_energy = max_energies[i_range];
      
      std::cout << "AlCap, " << leglabels[i_setting] << ": Integral (" << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << rates[i_setting][i_range] << " +/- " << rate_errs[i_setting][i_range] << std::endl;
    }
  }
  
  for (int i_range = 0; i_range < n_ranges; ++i_range) {
    double min_energy = min_energies[i_range];
    double max_energy = max_energies[i_range];

    std::cout << "Total SiL3 integral ("  << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << SiL3_rate[i_range] << " +/- " << SiL3_rate_err[i_range] << std::endl;

    TLine* min_line = new TLine(min_energy, 0, min_energy, 1);
    min_line->SetLineColor(kRed);
    min_line->SetLineWidth(2);
    min_line->Draw("LSAME");

    TLine* max_line = new TLine(max_energy, 0, max_energy, 1);
    max_line->SetLineColor(kRed);
    max_line->SetLineWidth(2);
    max_line->Draw("LSAME");
  }

  leg->Draw();
}
