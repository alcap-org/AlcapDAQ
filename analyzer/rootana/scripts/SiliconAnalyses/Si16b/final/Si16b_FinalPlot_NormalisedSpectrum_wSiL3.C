void Si16b_FinalPlot_NormalisedSpectrum_wSiL3() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
    
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

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(SiL3_hist, "SiL3 (active target)", "l");

  std::string filename = "~/data/results/Si16b/unfold_newPP.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  
  const int n_settings = 4;
  std::string particle_names[n_settings] = {"proton", "deuteron", "triton", "alpha"};
  Int_t colours[n_settings] = {kRed, kCyan, kSpring, kMagenta};
  std::string leglabels[n_settings] = {"Si16b (proton)", "Si16b (deuterons)", "Si16b (tritons)", "Si16b (alphas)"};

  int rebin_factor = 5;
  THStack* hStack = new THStack("hStack", "");
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {

    std::string i_particle_name = particle_names[i_setting];
    Int_t i_colour = colours[i_setting];

    std::string dirname = "FinalNormalisation_" + i_particle_name + "_PSel";
    std::string i_histname = dirname + "/hNormalisedSpectrum";

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
    hStack->Add(spectrum);
  }

  //  hStack->Draw("HIST E SAMES nostack");
  hStack->Draw("HIST E SAMES");  
  leg->Draw();
}
