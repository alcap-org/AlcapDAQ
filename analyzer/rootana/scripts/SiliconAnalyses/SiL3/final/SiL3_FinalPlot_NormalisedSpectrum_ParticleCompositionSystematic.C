void SiL3_FinalPlot_NormalisedSpectrum_ParticleCompositionSystematic() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
    
  std::string filename = "~/data/results/SiL3/unfold_geq2TgtPulse_newPP20us.root";
  TFile* file = new TFile(filename.c_str(), "READ");


  const int i_central = 0; // the element that is the central value
  const int n_settings = 3;
  std::string recoils[n_settings] = {"allRecoil", "lowAllRecoil", "highAllRecoil"};
  //  std::string responses[n_settings] = {"", "_CombinedLow", "_CombinedHigh"};
  std::string responses[n_settings] = {"Combined", "CombinedLow", "CombinedHigh"};
  Int_t colours[n_settings] = {kBlue, kRed, kMagenta};
  std::string leglabels[n_settings] = {"central value", "low value", "high value"};
  
  TLegend* leg = new TLegend(0.25,0.65,0.65,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  const int n_ranges = 6;
  double min_energies[n_ranges] = {1400, 15000, 15000, 16000, 15000, 20000};//3500, 4000, 3500};//0,    1400,  10000, 5000,   3500, 4000, 3500, 14000};
  double max_energies[n_ranges] = {26000, 16000, 17000, 17000, 18000, 50000};//10000, 8000, 15000};//26000,26000, 26000, 10000, 10000, 8000, 14000, 15000};
  
  int rebin_factor = 5;
  std::stringstream leglabel;
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string recoil = recoils[i_setting];
    std::string response = responses[i_setting];
    Int_t i_colour = colours[i_setting];

    //    std::string i_dirname = "FinalNormalisation" + response + "_TimeSlice2000_4000_" + recoil;
    //    std::string i_histname = i_dirname + "/hNormalisedSpectrum";
    std::string i_dirname = response + "Escape_TimeSlice2000_4000_" + recoil;
    //    std::string i_histname = i_dirname + "/hInputSpectrum";
    std::string i_histname = i_dirname + "/hCorrectedSpectrum";

    std::cout << i_dirname << std::endl;
    TH1F* spectrum = (TH1F*) file->Get(i_histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << i_histname << std::endl;
      return;
    }
    spectrum->Sumw2();

    spectrum->SetTitle("SiL3 Dataset, Active Target Analysis, Charged Particle Emission Spectrum");
    spectrum->Rebin(rebin_factor);
    spectrum->Scale(1.0/rebin_factor);
    spectrum->SetStats(false);
    spectrum->SetLineColor(i_colour);
    spectrum->GetXaxis()->SetRangeUser(0,50000);
    spectrum->Draw("HIST E SAMES");
    /*
    std::string i_fitname = i_dirname + "/spectral_fit";
    TF1* spectral_fit = (TF1*) file->Get(i_fitname.c_str());
    spectral_fit->SetLineColor(i_colour);
    spectral_fit->Draw("LSAME");
    */
    
    leglabel.str("");
    /*    leglabel << "#splitline{" << leglabels[i_slice] << "}{"
	     << std::fixed << std::setprecision(2) << "(T_{th} = " << spectral_fit->GetParameter(1)/1000 << " #pm " << spectral_fit->GetParError(1)/1000 << " MeV, "
	     << std::fixed << std::setprecision(2) << " #alpha = " << spectral_fit->GetParameter(2) << " #pm " << spectral_fit->GetParError(2) << ", "
	     << std::fixed << std::setprecision(2) << " T_{0} = " << spectral_fit->GetParameter(3)/1000 << " #pm " << spectral_fit->GetParError(3)/1000 << " MeV)}";
    */
    leglabel << leglabels[i_setting];
    std::cout << leglabel.str() << std::endl;
    leg->AddEntry(spectrum, leglabel.str().c_str(), "l");

    for (int i_range = 0; i_range < n_ranges; ++i_range) {
      double min_energy = min_energies[i_range];
      double max_energy = max_energies[i_range];
      int min_energy_bin = spectrum->GetXaxis()->FindBin(min_energy);
      int max_energy_bin = spectrum->GetXaxis()->FindBin(max_energy) - 1;
      double error = -1;
      double integral = spectrum->IntegralAndError(min_energy_bin, max_energy_bin, error, "width");
      std::cout << "Rate (" << min_energy/1000 << " MeV -- " << max_energy/1000 << " MeV) = " << integral << " +/- " << error << std::endl;
    }
  }

  leg->Draw();
}
