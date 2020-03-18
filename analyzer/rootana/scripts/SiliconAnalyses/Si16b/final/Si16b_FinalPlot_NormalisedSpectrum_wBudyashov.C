void Si16b_FinalPlot_NormalisedSpectrum_wBudyashov(std::string savedir = "") {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();

  const int n_ranges = 5;
  double min_energies[n_ranges] = {15000, 15000, 16000, 15000, 16000};//3500, 4000, 3500};//0,    1400,  10000, 5000,   3500, 4000, 3500, 14000};
  double max_energies[n_ranges] = {16000, 17000, 17000, 18000, 18000};//10000, 8000, 15000};//26000,26000, 26000, 10000, 10000, 8000, 14000, 15000};

  TLegend* leg = new TLegend(0.50,0.55,0.85,0.85);
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
  std::string filename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_3sigma.root";
  //    std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_SiL1-2--6.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  
  const int n_settings = 2;
  std::string particle_names[n_settings] = {"proton", "deuteron"};//, "triton", "alpha", "SiL3"};
  Int_t colours[n_settings] = {kRed, kCyan};//, kMagenta, kSpring, kGray};
  std::string leglabels[n_settings] = {"AlCap (proton)", "AlCap (deuterons)"};//, "AlCap (tritons)", "AlCap (alphas)", "AlCap (SiL3 inc.)"};
  double rates[n_settings][n_ranges] = {0};
  double rate_errs[n_settings][n_ranges] = {0};
  
  int rebin_factors[n_settings] = {1, 1};//, 1, 1, 1};
  THStack* hStack = new THStack("hStack", "");
  TH1F* hSi16b_SiL3Inc = NULL;
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {

    std::string particle = particle_names[i_setting];
    Int_t i_colour = colours[i_setting];

    std::string dirname = "FinalSystPlot_" + particle + "_TCutG";
    std::string histname = dirname + "/hFinalStatSyst";
    TGraphAsymmErrors* spectrum = (TGraphAsymmErrors*) file->Get(histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << histname << std::endl;
      return;
    }

    spectrum->SetLineColor(i_colour);
    spectrum->SetLineWidth(2);
    //    spectrum->SetLineColor(kBlack);
    //    spectrum->SetFillColor(i_colour);
    //    spectrum->Draw("HIST E SAMES");
    leg->AddEntry(spectrum, leglabels[i_setting].c_str(), "l");

    if (i_setting == 0) {
      spectrum->Draw("APE");
      spectrum->GetXaxis()->SetTitle("Energy [keV]");
      spectrum->GetYaxis()->SetTitle("Charged Particles per muon capture per keV");
      spectrum->GetXaxis()->SetTitleOffset(0.9);
      spectrum->GetYaxis()->SetTitleOffset(0.9);
      spectrum->GetXaxis()->SetRangeUser(0, 20000);
      spectrum->GetYaxis()->SetRangeUser(0, 1e-4);
    }
    else {
      spectrum->Draw("PE SAME");
    }
    // alcaphistogram(spectrum);
    // if (i_setting == 0) {
    //   alcapPreliminary(spectrum);
    // }
    /*    std::string fitname = dirname + "/spectral_fit";
    TF1* fit = (TF1*) file->Get(fitname.c_str());
    if (fit) {
      fit->SetLineWidth(2);
      fit->SetLineColor(kCyan);
      fit->Draw("LSAME");
    }
    */
  }
 
  Budyashov_gre_proton->Draw("PE SAME");
  Budyashov_gre_deuteron->Draw("PE SAME");

  leg->Draw();

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_Si16bDataset_NormalisedSpectrum_wBudyashov";

    std::string pdfname = savename + ".pdf";
    c1->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c1->SaveAs(pngname.c_str());
  }
}
