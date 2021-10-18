void Si16b_FinalPlot_NormalisedSpectrum_wSiL3_forPaper(std::string savedir = "") {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();

  const int n_recoils = 1;
  std::string recoils[n_recoils] = {"allRecoil"};//, "noRecoil"};
  std::string leglabels[n_recoils] = {"active Si target"};//, "without recoil correction"};
  Int_t colours[n_recoils] = {kBlack};
  std::string SiL3_filename = "~/data/results/SiL3/systematics_geq2TgtPulse_newPP20us_1.root";
  TFile* SiL3_file = new TFile(SiL3_filename.c_str(), "READ");

  TLegend* leg = new TLegend(0.45,0.65,0.80,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.04);
  leg->SetFillColor(kWhite);
  leg->SetHeader("AlCap Si");
  
  for (int i_recoil = 0; i_recoil < n_recoils; ++i_recoil) {
    std::string recoil = recoils[i_recoil];
    Int_t colour = colours[i_recoil];
    
    std::string SiL3_dirname = "FinalSystPlot_TimeSlice2000_4000_" + recoil + "_500keVBins";
    //  std::string SiL3_dirname = "FinalSystPlot_TimeSlice2000_4000_noRecoil_500keVBins";
    //  std::string SiL3_dirname = "FinalSystPlot_TimeSlice2000_4000_nuRecoil_500keVBins";
    std::string SiL3_histname = SiL3_dirname + "/hFinalStatSyst";
    TGraphAsymmErrors* SiL3_hist = (TGraphAsymmErrors*) SiL3_file->Get(SiL3_histname.c_str());
    if (i_recoil == 0) {
      SiL3_hist->Draw("APE");
    }
    else {
      SiL3_hist->Draw("PE SAME");
    }
    //  SiL3_hist->SetStats(false);
    SiL3_hist->SetLineWidth(2);
    SiL3_hist->SetLineColor(colour);
    SiL3_hist->SetTitle("Charged Particle Emission");
    SiL3_hist->GetXaxis()->SetRangeUser(2,17);
    SiL3_hist->GetXaxis()->SetTitle("Energy [MeV]");
    SiL3_hist->GetYaxis()->SetTitle("Particles / muon capture / MeV");
    SiL3_hist->GetXaxis()->SetTitleOffset(0.9);
    SiL3_hist->GetYaxis()->SetTitleOffset(1.0);
    //    SiL3_hist->GetYaxis()->SetRangeUser(1e-8, 1e-3);
    //    SiL3_hist->GetYaxis()->SetRangeUser(5e-4, 1e-1);
    SiL3_hist->GetYaxis()->SetRangeUser(5e-4, 5e-2);
    SiL3_hist->SetMarkerStyle(kFullCircle);
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

    std::string leglabel = leglabels[i_recoil];
    leg->AddEntry(SiL3_hist, leglabel.c_str(), "lp");
  }
  
  //  std::string filename = "~/data/results/Si16b/unfold_newPP.root";
  std::string filename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_1.root";
  //    std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_SiL1-2--6.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  
  const int n_settings = 1;
  std::string particle_names[n_settings] = {"Sum"};
  //  Int_t colours[n_settings] = {kBlack};
  std::string leglabels_Si16b[n_settings] = {"passive Si target (p+d+t+#alpha)"};

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
    spectrum->SetLineColor(kRed);
    spectrum->SetFillStyle(0);
    spectrum->SetFillColor(0);
    spectrum->SetLineWidth(2);
    spectrum->SetMarkerStyle(kOpenCircle);
    spectrum->SetMarkerColor(kRed);
    //    spectrum->SetLineColor(kBlack);
    //    spectrum->SetFillColor(i_colour);
    spectrum->Draw("PE SAMEs");
    leg->AddEntry(spectrum, leglabels_Si16b[i_setting].c_str(), "lp");

  }
  leg->Draw();

  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.17, 0.75, "AlCap #bf{#it{Preliminary}}");
  
  if (savedir != "") {
    std::string savename = savedir + "AlCapData_Si16bDataset_NormalisedSpectra_wSiL3_forPaper";
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
}
