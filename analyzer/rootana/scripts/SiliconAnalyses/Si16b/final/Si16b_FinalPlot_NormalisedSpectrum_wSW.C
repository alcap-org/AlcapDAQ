void Si16b_FinalPlot_NormalisedSpectrum_wSW() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
    
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
