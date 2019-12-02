void Si16b_FinalPlot_NormalisedSpectrum_wBudyashov() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();

  const int n_ranges = 5;
  double min_energies[n_ranges] = {15000, 15000, 16000, 15000, 16000};//3500, 4000, 3500};//0,    1400,  10000, 5000,   3500, 4000, 3500, 14000};
  double max_energies[n_ranges] = {16000, 17000, 17000, 18000, 18000};//10000, 8000, 15000};//26000,26000, 26000, 10000, 10000, 8000, 14000, 15000};

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);

  std::string Budyashov_filename = "~/data/results/Si16b/Budyashov-plots.root";
  TFile* Budyashov_file = new TFile(Budyashov_filename.c_str(), "READ");
  TGraphErrors* Budyashov_gre_proton = (TGraphErrors*) Budyashov_file->Get("proton");
  Budyashov_gre_proton->SetMarkerColor(kRed);
  Budyashov_gre_proton->SetMarkerStyle(kFullTriangleUp);
  Budyashov_gre_proton->SetLineColor(kBlack);
  Budyashov_gre_proton->SetTitle("Charged Particle Emission");
  Budyashov_gre_proton->GetXaxis()->SetRangeUser(0,26000);
  Budyashov_gre_proton->GetXaxis()->SetTitle("Energy [keV]");
  Budyashov_gre_proton->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per keV");
  leg->AddEntry(Budyashov_gre_proton, "Budyashov et al. (proton)", "pl");

  TGraphErrors* Budyashov_gre_deuteron = (TGraphErrors*) Budyashov_file->Get("deuteron");
  Budyashov_gre_deuteron->SetMarkerColor(kCyan);
  Budyashov_gre_deuteron->SetMarkerStyle(kFullTriangleUp);
  Budyashov_gre_deuteron->SetLineColor(kBlack);
  Budyashov_gre_deuteron->SetTitle("Charged Particle Emission");
  Budyashov_gre_deuteron->GetXaxis()->SetRangeUser(0,26000);
  Budyashov_gre_deuteron->GetXaxis()->SetTitle("Energy [keV]");
  Budyashov_gre_deuteron->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per keV");
  leg->AddEntry(Budyashov_gre_deuteron, "Budyashov et al. (deuteron)", "pl");

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
      hStack->Add(spectrum);

      
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
 

  //  hStack->GetXaxis()->SetRangeUser(0,26000);
  //  hStack->GetXaxis()->SetTitle("Energy [keV]");
  //  hStack->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per keV");
  hStack->SetMaximum(1e-3);
  hStack->SetMinimum(1e-8);
  hStack->Draw("HIST E nostack");
  //  hStack->Draw("HIST E");
  if (hSi16b_SiL3Inc) {
    hSi16b_SiL3Inc->Draw("HIST E SAME");
  }
  Budyashov_gre_proton->Draw("PE SAME");
  Budyashov_gre_deuteron->Draw("PE SAME");

  /*

  TGraphErrors* Budyashov_gre_sum = (TGraphErrors*) Budyashov_file->Get("sum");
  Budyashov_gre_sum->SetMarkerColor(kBlack);
  Budyashov_gre_sum->SetMarkerStyle(7);
  Budyashov_gre_sum->SetLineColor(kBlack);
  Budyashov_gre_sum->Draw("PE SAME");
  Budyashov_gre_sum->SetTitle("Charged Particle Emission");
  Budyashov_gre_sum->GetXaxis()->SetRangeUser(0,26000);
  Budyashov_gre_sum->GetXaxis()->SetTitle("Energy [keV]");
  Budyashov_gre_sum->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per keV");
  leg->AddEntry(Budyashov_gre_sum, "Budyashov et al. (sum)", "pl");
  */
  leg->Draw();
}
