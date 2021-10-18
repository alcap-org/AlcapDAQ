#include "../../Utils/IntegrateRates.C"

void Si16b_FinalPlot_NormalisedSpectrum_wSiL3(std::string savedir = "") {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();

  // const int n_ranges = 5;
  // double min_energies[n_ranges] = {15000, 15000, 16000, 15000, 14000};//3500, 4000, 3500};//0,    1400,  10000, 5000,   3500, 4000, 3500, 14000};
  // double max_energies[n_ranges] = {16000, 17000, 17000, 18000, 15000};//10000, 8000, 15000};//26000,26000, 26000, 10000, 10000, 8000, 14000, 15000};
  // double SiL3_rate[n_ranges] = {0};
  // double SiL3_rate_err[n_ranges] = {0};

  std::string SiL3_filename = "~/data/results/SiL3/systematics_geq2TgtPulse_newPP20us_1_KFactor0-85_test.root";
  TFile* SiL3_file = new TFile(SiL3_filename.c_str(), "READ");
  //  std::string SiL3_dirname = "FinalSystPlot_TimeSlice3000_4000_allRecoil_500keVBins";
  std::string SiL3_dirname = "FinalSystPlot_TimeSlice3000_4000_noRecoil_500keVBins";
  //  std::string SiL3_dirname = "FinalSystPlot_TimeSlice3000_4000_nuRecoil_500keVBins";
  std::string SiL3_histname = SiL3_dirname + "/hFinalStatSyst";
  TGraphAsymmErrors* SiL3_hist = (TGraphAsymmErrors*) SiL3_file->Get(SiL3_histname.c_str());
  SiL3_hist->Draw("APE");
  //  SiL3_hist->SetStats(false);
  SiL3_hist->SetLineWidth(2);
  SiL3_hist->SetLineColor(kBlack);
  SiL3_hist->SetTitle("Charged Particle Emission");
  SiL3_hist->GetXaxis()->SetRangeUser(0,26);
  //  SiL3_hist->GetXaxis()->SetRangeUser(15, 17);
  SiL3_hist->GetXaxis()->SetTitle("Energy [MeV]");
  SiL3_hist->GetXaxis()->SetTitle("Energy [MeV]");
  SiL3_hist->GetYaxis()->SetTitle("Charged Particles / muon capture / MeV");
  SiL3_hist->GetXaxis()->SetTitleOffset(0.9);
  SiL3_hist->GetYaxis()->SetTitleOffset(0.9);
  SiL3_hist->GetYaxis()->SetRangeUser(1e-5, 1);
  //  SiL3_hist->SetMaximum(1e-3);
  //  SiL3_hist->SetMinimum(1e-8);
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
  double rate, total_stat_error, total_low_syst_error, total_high_syst_error;
  double min_energy = 15;
  double max_energy = 17;
  SiL3_histname = SiL3_dirname + "/hFinalStat";
  TGraphAsymmErrors* stat_spectrum = (TGraphAsymmErrors*) SiL3_file->Get(SiL3_histname.c_str());
  SiL3_histname = SiL3_dirname + "/hFinalSyst";
  TGraphAsymmErrors* syst_spectrum = (TGraphAsymmErrors*) SiL3_file->Get(SiL3_histname.c_str());
  IntegrateRates_wStatAndSystSpectra(stat_spectrum, syst_spectrum, min_energy, max_energy,rate, total_stat_error, total_high_syst_error, total_low_syst_error);
  std::cout << "SiL3 (Rate " << min_energy << " -- " << max_energy << ") = " << rate << " +/- " << total_stat_error << " (stat.) + " << total_high_syst_error << " - " << total_low_syst_error << " (syst.)" << std::endl;


  TLegend* leg = new TLegend(0.50,0.55,0.85,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(SiL3_hist, "SiL3 (active target)", "l");

  //  std::string filename = "~/data/results/Si16b/unfold_newPP.root";
  std::string filename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_JohnVetoEff_1.root";
  //    std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_SiL1-2--6.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  
  const int n_settings = 5;
  std::string particle_names[n_settings] = {"proton", "deuteron", "triton", "alpha", "Sum"};
  Int_t colours[n_settings] = {kRed, kCyan, kMagenta, kSpring, kBlue};
  std::string leglabels[n_settings] = {"Si16b (proton)", "Si16b (deuterons)", "Si16b (tritons)", "Si16b (alphas)", "Si16b (sum)"};
  // double rates[n_settings][n_ranges] = {0};
  // double rate_errs[n_settings][n_ranges] = {0};
  
  //  int rebin_factors[n_settings] = {1, 1, 1, 1, 1};
  THStack* hStack = new THStack("hStack", "");
  //  TH1F* hSi16b_Total = NULL;
  //  TH1F* hSi16b_SiL3Inc = NULL;
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {

    std::string i_particle_name = particle_names[i_setting];
    Int_t i_colour = colours[i_setting];

    std::string dirname = "FinalSystPlot_" + i_particle_name + "_TCutG_2sig_layerCoinc500ns_tGT0ns";
    std::string i_histname = dirname + "/hFinalStatSyst";

    TGraphAsymmErrors* spectrum = (TGraphAsymmErrors*) file->Get(i_histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << i_histname << std::endl;
      //      return;
      continue;
    }
    //    spectrum->SetStats(false);
    spectrum->SetLineColor(i_colour);
    spectrum->SetFillStyle(0);
    spectrum->SetFillColor(0);
    spectrum->SetLineWidth(2);
    //    spectrum->SetLineColor(kBlack);
    //    spectrum->SetFillColor(i_colour);
    spectrum->Draw("PE SAMEs");
    leg->AddEntry(spectrum, leglabels[i_setting].c_str(), "l");

    if (i_particle_name != "Sum") {
      rate = total_stat_error = total_high_syst_error = total_low_syst_error = 0;
      i_histname = dirname + "/hFinalStat";
      TGraphAsymmErrors* stat_spectrum = (TGraphAsymmErrors*) file->Get(i_histname.c_str());
      i_histname = dirname + "/hFinalSyst";
      TGraphAsymmErrors* syst_spectrum = (TGraphAsymmErrors*) file->Get(i_histname.c_str());
      IntegrateRates_wStatAndSystSpectra(stat_spectrum, syst_spectrum, min_energy, max_energy,rate, total_stat_error, total_high_syst_error, total_low_syst_error);
      std::cout << "Si16b, " << i_particle_name << " (Rate " << min_energy << " -- " << max_energy << ") = " << rate << " +/- " << total_stat_error << " (stat.) + " << total_high_syst_error << " - " << total_low_syst_error << " (syst.)" << std::endl;
    }
    /*    std::string fitname = dirname + "/spectral_fit";
    TF1* fit = (TF1*) file->Get(fitname.c_str());
    if (fit) {
      fit->SetLineWidth(2);
      fit->SetLineColor(kCyan);
      fit->Draw("LSAME");
    }
    */
    // for (int i_range = 0; i_range < n_ranges; ++i_range) {
    //   double min_energy = min_energies[i_range];
    //   double max_energy = max_energies[i_range];
    //   int min_energy_bin = spectrum->GetXaxis()->FindBin(min_energy);
    //   int max_energy_bin = spectrum->GetXaxis()->FindBin(max_energy) - 1;
    //   double error = -1;
    //   double integral = spectrum->IntegralAndError(min_energy_bin, max_energy_bin, error, "width");
    //   rates[i_setting][i_range] = integral;
    //   rate_errs[i_setting][i_range] = error;
    // }

    //    alcaphistogram(spectrum);
    //      hStack->Add(spectrum);

  }
  leg->Draw();

  // double total_rates[n_ranges] = {0};
  // double total_rate_errs[n_ranges] = {0};
  // for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
  //   for (int i_range = 0; i_range < n_ranges; ++i_range) {
  //     double min_energy = min_energies[i_range];
  //     double max_energy = max_energies[i_range];
      
  //     std::cout << "AlCap, " << leglabels[i_setting] << ": Integral (" << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << rates[i_setting][i_range] << " +/- " << rate_errs[i_setting][i_range] << std::endl;
      
  //     if (particle_names[i_setting] != "SiL3") {
  // 	total_rates[i_range] += rates[i_setting][i_range];	  
  // 	total_rate_errs[i_range] += (rate_errs[i_setting][i_range]*rate_errs[i_setting][i_range]);
  //     }	
  //   }
  // }
  
  // for (int i_range = 0; i_range < n_ranges; ++i_range) {
  //   double min_energy = min_energies[i_range];
  //   double max_energy = max_energies[i_range];

  //   total_rate_errs[i_range] = std::sqrt(total_rate_errs[i_range]);

  //   std::cout << "Total SiL3 integral ("  << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << SiL3_rate[i_range] << " +/- " << SiL3_rate_err[i_range] << std::endl;
  //   std::cout << "Total Si16b: Integral (" << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << total_rates[i_range] << " +/- " << total_rate_errs[i_range] << std::endl;

  //   for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
  //     if (particle_names[i_setting] != "SiL3") {
  // 	std::cout << "Ratio (" << leglabels[i_setting] << ") = " << rates[i_setting][i_range] / total_rates[i_range] << std::endl;
  //     }
  //   }

  //   int min_bin_stack = hSi16b_Total->GetXaxis()->FindBin(min_energy);
  //   int max_bin_stack = hSi16b_Total->GetXaxis()->FindBin(max_energy)-1;
  //   double stack_integral_error = 0;
  //   double stack_integral = hSi16b_Total->IntegralAndError(min_bin_stack, max_bin_stack, stack_integral_error, "width");
  //   std::cout << "hSi16b_Total (" << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << stack_integral << " +/- " << stack_integral_error << std::endl;

  //   /*
  //   TLine* min_line = new TLine(min_energy, 0, min_energy, 1);
  //   min_line->SetLineColor(kRed);
  //   min_line->SetLineWidth(2);
  //   min_line->Draw("LSAME");

  //   TLine* max_line = new TLine(max_energy, 0, max_energy, 1);
  //   max_line->SetLineColor(kRed);
  //   max_line->SetLineWidth(2);
  //   max_line->Draw("LSAME");
  //   */
  // }

  //  hStack->GetXaxis()->SetTitle("Energy [MeV]");
  //  hStack->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per MeV");
  //  hStack->SetMaximum(1e-3);
  //  hStack->SetMinimum(1e-8);
  //  hStack->SetMaximum(0.035e-3);
  //  hStack->SetMinimum(0);
  //  hStack->Draw("HIST E SAMES nostack");

  // TCanvas* c_stacked = new TCanvas();
  // c_stacked->SetLogy();
  // hStack->Draw("HIST E");
  // hStack->GetXaxis()->SetRangeUser(0,26000);
  // if (hSi16b_SiL3Inc) {
  //   hSi16b_SiL3Inc->Draw("HIST E SAME");
  // }
  // //  hSi16b_Total->Draw("HIST E SAME");
  // SiL3_hist->Draw("HIST E SAME");
  // alcapPreliminary(SiL3_hist);
  // leg->Draw();
  
  // TCanvas* c_unstacked = new TCanvas();
  // c_unstacked->SetLogy();
  // hStack->Draw("HIST E nostack");
  // hStack->GetXaxis()->SetRangeUser(0,26000);
  // if (hSi16b_SiL3Inc) {
  //   hSi16b_SiL3Inc->Draw("HIST E SAME");
  // }
  // //  hSi16b_Total->Draw("HIST E SAME");
  // SiL3_hist->Draw("HIST E SAME");
  // alcapPreliminary(SiL3_hist);
  // leg->Draw();
 
  if (savedir != "") {
    std::string savename = savedir + "AlCapData_NormalisedSpectraComparison";
    std::string pdfname = savename + ".pdf";
    c1->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c1->SaveAs(pngname.c_str());

    // savename = savedir + "AlCapData_NormalisedSpectraComparison_Unstacked";    
    // pdfname = savename + ".pdf";
    // //    c_unstacked->SaveAs(pdfname.c_str());
    // pngname = savename + ".png";
    // //    c_unstacked->SaveAs(pngname.c_str());
  }

  /*
  std::string Si16b_filename = "~/data/results/Si16b/unfold_newPP20us_geq2TgtPulse.root";
  TFile* Si16b_file = new TFile(Si16b_filename.c_str(), "READ");
  TH1F* Si16b_hist = (TH1F*) Si16b_file->Get("FinalNormalisation_Active/hNormalisedSpectrum");
  int rebin_Si16b = 1;
  Si16b_hist->Rebin(rebin_Si16b);
  Si16b_hist->Scale(1.0 / rebin_Si16b);
  Si16b_hist->Draw("HIST E SAME");
  Si16b_hist->SetStats(false);
  Si16b_hist->SetLineColor(kBlack);
  Si16b_hist->SetTitle("Charged Particle Emission");
  Si16b_hist->GetXaxis()->SetRangeUser(0,26000);
  Si16b_hist->GetXaxis()->SetTitle("Energy [MeV]");
  Si16b_hist->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per MeV");
  Si16b_hist->SetMaximum(1e-3);
  Si16b_hist->SetMinimum(1e-8);
  */
  
  /*
  std::string Budyashov_filename = "~/data/results/Si16b/Budyashov-plots.root";
  TFile* Budyashov_file = new TFile(Budyashov_filename.c_str(), "READ");
  TGraphErrors* Budyashov_gre_proton = (TGraphErrors*) Budyashov_file->Get("proton");
  Budyashov_gre_proton->SetMarkerColor(kBlue);
  Budyashov_gre_proton->SetMarkerStyle(7);
  Budyashov_gre_proton->SetLineColor(kBlue);
  Budyashov_gre_proton->Draw("PE SAME");
  Budyashov_gre_proton->SetTitle("Charged Particle Emission");
  Budyashov_gre_proton->GetXaxis()->SetRangeUser(0,26000);
  Budyashov_gre_proton->GetXaxis()->SetTitle("Energy [MeV]");
  Budyashov_gre_proton->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per MeV");
  leg->AddEntry(Budyashov_gre_proton, "Budyashov et al. (proton)", "pl");

  TGraphErrors* Budyashov_gre_deuteron = (TGraphErrors*) Budyashov_file->Get("deuteron");
  Budyashov_gre_deuteron->SetMarkerColor(kCyan);
  Budyashov_gre_deuteron->SetMarkerStyle(7);
  Budyashov_gre_deuteron->SetLineColor(kCyan);
  Budyashov_gre_deuteron->Draw("PE SAME");
  Budyashov_gre_deuteron->SetTitle("Charged Particle Emission");
  Budyashov_gre_deuteron->GetXaxis()->SetRangeUser(0,26000);
  Budyashov_gre_deuteron->GetXaxis()->SetTitle("Energy [MeV]");
  Budyashov_gre_deuteron->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per MeV");
  leg->AddEntry(Budyashov_gre_deuteron, "Budyashov et al. (deuteron)", "pl");

  TGraphErrors* Budyashov_gre_sum = (TGraphErrors*) Budyashov_file->Get("sum");
  Budyashov_gre_sum->SetMarkerColor(kBlack);
  Budyashov_gre_sum->SetMarkerStyle(7);
  Budyashov_gre_sum->SetLineColor(kBlack);
  Budyashov_gre_sum->Draw("PE SAME");
  Budyashov_gre_sum->SetTitle("Charged Particle Emission");
  Budyashov_gre_sum->GetXaxis()->SetRangeUser(0,26000);
  Budyashov_gre_sum->GetXaxis()->SetTitle("Energy [MeV]");
  Budyashov_gre_sum->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per MeV");
  leg->AddEntry(Budyashov_gre_sum, "Budyashov et al. (sum)", "pl");
  */
}
