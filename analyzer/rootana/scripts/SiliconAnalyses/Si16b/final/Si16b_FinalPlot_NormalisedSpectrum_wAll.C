void Si16b_FinalPlot_NormalisedSpectrum_wAll(std::string savedir = "", std::ostream& numbers_file = std::cout) {

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

  
  
  std::string SiL3_filename = "~/data/results/SiL3/systematics_geq2TgtPulse_newPP20us.root";
  TFile* SiL3_file = new TFile(SiL3_filename.c_str(), "READ");
  //  TH1F* SiL3_hist = (TH1F*) SiL3_file->Get("FinalNormalisation_TimeSlice2000_4000_allRecoil/hNormalisedSpectrum");
  TGraphAsymmErrors* SiL3_hist = (TGraphAsymmErrors*) SiL3_file->Get("FinalSystPlot_TimeSlice2000_4000_allRecoil_500keVBins/hFinalStatSyst");
  //  SiL3_hist->SetStats(false);
  SiL3_hist->SetLineColor(kRed);
  SiL3_hist->SetLineWidth(2);
  // SiL3_hist->SetTitle("Charged Particle Emission");
  // SiL3_hist->GetXaxis()->SetRangeUser(0,26000);
  // SiL3_hist->GetXaxis()->SetTitle("Energy [keV]");
  // SiL3_hist->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per keV");
  // SiL3_hist->SetMaximum(1e-3);
  // SiL3_hist->SetMinimum(1e-8);
  // for (int i_range = 0; i_range < n_ranges; ++i_range) {
  //   double min_energy = min_energies[i_range];
  //   double max_energy = max_energies[i_range];
  //   int min_energy_bin = SiL3_hist->GetXaxis()->FindBin(min_energy);
  //   int max_energy_bin = SiL3_hist->GetXaxis()->FindBin(max_energy) - 1;
  //   double error = -1;
  //   double integral = SiL3_hist->IntegralAndError(min_energy_bin, max_energy_bin, error, "width");
  //   SiL3_rate[i_range] = integral;
  //   SiL3_rate_err[i_range] = error;
  // }
  //  int rebin_SiL3 = 2;
  //  SiL3_hist->Rebin(rebin_SiL3);
  //  SiL3_hist->Scale(1.0 / rebin_SiL3);
  SiL3_hist->Draw("PE SAME");
  leg->AddEntry(SiL3_hist, "SiL3 (active target)", "l");

  //  std::string filename = "~/data/results/Si16b/unfold_newPP.root";
  std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse.root";
  //    std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_SiL1-2--6.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  std::string systfilename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_3sigma.root";
  TFile* systfile = new TFile(systfilename.c_str(), "READ");

  const int n_settings = 1;
  std::string particle_names[n_settings] = {"sum"};
  std::string Particle_names[n_settings] = {"Sum"};
  Int_t colours[n_settings] = {kMagenta};
  std::string leglabels[n_settings] = {"Si16b (sum)"};
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {

    std::string i_particle_name = particle_names[i_setting];
    Int_t i_colour = colours[i_setting];

    std::string dirname = "FinalSystPlot_" + Particle_names[i_setting] + "_TCutG";
    std::string i_histname = dirname + "/hFinalStatSyst";

    TGraphAsymmErrors* spectrum = (TGraphAsymmErrors*) systfile->Get(i_histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << i_histname << std::endl;
      return;
    }
    
    //    spectrum->SetStats(false);
    spectrum->SetLineColor(i_colour);
    spectrum->SetFillStyle(0);
    spectrum->SetFillColor(0);
    spectrum->SetLineWidth(2);
    
    //    alcaphistogram(spectrum);
    leg->AddEntry(spectrum, leglabels[i_setting].c_str());

    spectrum->Draw("PE SAME");
  }

  
  leg->Draw();
  if (savedir != "") {
    std::string savename = savedir + "AlCapData_NormalisedSpectraComparison_wLiterature";
    std::string pdfname = savename + ".pdf";
    c1->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c1->SaveAs(pngname.c_str());
  }
}
