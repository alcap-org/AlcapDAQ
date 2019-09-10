void Si16b_FinalPlot_NormalisedSpectrum() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
    
  std::string filename = "~/data/results/Si16b/unfold_newPP.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  
  const int n_settings = 2;
  std::string particle_names[n_settings] = {"proton", "deuteron"};
  Int_t colours[n_settings] = {kRed, kCyan};
  std::string leglabels[n_settings] = {"Si16b (protons)", "Si16b (deuterons)"};

  TLegend* leg = new TLegend(0.20,0.65,0.60,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);
  
  int rebin_factor = 2;
  std::stringstream time_setting_str, leglabel;
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string i_particle_name = particle_names[i_setting];
    Int_t i_colour = colours[i_setting];

    std::string i_dirname = "FinalNormalisation_" + i_particle_name;
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
    spectrum->GetXaxis()->SetRangeUser(0,15000);
    spectrum->Draw("HIST E SAMES");

    std::string i_fitname = i_dirname + "/spectral_fit";
    TF1* spectral_fit = (TF1*) file->Get(i_fitname.c_str());
    spectral_fit->SetLineColor(i_colour);
    spectral_fit->Draw("LSAME");

    leglabel.str("");
    leglabel << "#splitline{" << leglabels[i_setting] << "}{"
	     << std::fixed << std::setprecision(2) << "(T_{th} = " << spectral_fit->GetParameter(1)/1000 << " #pm " << spectral_fit->GetParError(1)/1000 << " MeV, "
	     << std::fixed << std::setprecision(2) << " #alpha = " << spectral_fit->GetParameter(2) << " #pm " << spectral_fit->GetParError(2) << ", "
	     << std::fixed << std::setprecision(2) << " T_{0} = " << spectral_fit->GetParameter(3)/1000 << " #pm " << spectral_fit->GetParError(3)/1000 << " MeV)}";
    std::cout << leglabel.str() << std::endl;
    leg->AddEntry(spectrum, leglabel.str().c_str(), "l");
  }

  leg->Draw();
}
