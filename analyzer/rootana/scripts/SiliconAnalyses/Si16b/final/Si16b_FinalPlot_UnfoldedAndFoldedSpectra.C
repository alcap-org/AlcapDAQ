void Si16b_FinalPlot_UnfoldedAndFoldedSpectra(std::string savedir = "", std::ostream& numbers_file = std::cout) {

  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  std::string Particles[n_particles] = {"Proton", "Deuteron", "Triton", "Alpha"};
  //  double x_maxes[n_particles] = {17000, 17000, 17000, 19000};
  double x_maxes[n_particles] = {25, 25, 25, 25};
  Int_t unfolded_colours[n_particles] = {kRed, kCyan, kMagenta, kSpring};

  numbers_file << "% from Si16b_FinalPlot_UnfoldedAndFoldedSpectra.C" << std::endl;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    TLegend* leg = new TLegend(0.55,0.60,0.85,0.85);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.035);
    leg->SetFillColor(kWhite);

    std::string particle = particles[i_particle];
    std::string Particle = Particles[i_particle];
    std::string infilename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_2.root";
    std::string indirname = "ResponseMatrix_" + particle + "_TCutG_2sig_layerCoinc500ns_tGT0ns_BinW500keV";
    std::string folded_histname = indirname + "/hInputSpectrum";
    std::string unfolded_histname = indirname + "/hCorrectedSpectrum";
    std::string outhisttitle = "Si16b Dataset, Right Arm, " + particle;
    int rebin_factor = 1;
    double x_max = x_maxes[i_particle];
    
    TFile* infile = new TFile(infilename.c_str(), "READ");
    TH1F* hFoldedSpectrum = (TH1F*) infile->Get(folded_histname.c_str());
    if (!hFoldedSpectrum) {
      std::cout << "Can't find histogram " << folded_histname.c_str() << std::endl;
      return;
    }
    TH1F* hUnfoldedSpectrum = (TH1F*) infile->Get(unfolded_histname.c_str());
    
    TCanvas* c_Spectra = new TCanvas();
    std::stringstream axistitle;
    
    hFoldedSpectrum->SetTitle(outhisttitle.c_str());
    hFoldedSpectrum->SetStats(false);
    hFoldedSpectrum->SetLineColor(kBlack);
    hFoldedSpectrum->Rebin(rebin_factor);
    hFoldedSpectrum->GetXaxis()->SetRangeUser(0, x_max);
    axistitle.str(""); axistitle << "Folded Count / " << hFoldedSpectrum->GetXaxis()->GetBinWidth(1) << " keV";
    hFoldedSpectrum->SetYTitle(axistitle.str().c_str());
    hFoldedSpectrum->SetXTitle("Energy [keV]");
    hFoldedSpectrum->GetXaxis()->SetTitleOffset(0.9);
    hFoldedSpectrum->GetYaxis()->SetTitleOffset(0.9);
    
    hUnfoldedSpectrum->SetTitle("");
    hUnfoldedSpectrum->SetStats(false);
    hUnfoldedSpectrum->SetLineColor(unfolded_colours[i_particle]);
    hUnfoldedSpectrum->Rebin(rebin_factor);
    hUnfoldedSpectrum->GetXaxis()->SetRangeUser(0, x_max);
    axistitle.str(""); axistitle << "Unfolded Count / " << hUnfoldedSpectrum->GetXaxis()->GetBinWidth(1) << " keV";
    hUnfoldedSpectrum->SetYTitle(axistitle.str().c_str());
    hUnfoldedSpectrum->SetXTitle("Energy [keV]");
    hUnfoldedSpectrum->GetXaxis()->SetTitleOffset(0.9);
    hUnfoldedSpectrum->GetYaxis()->SetTitleOffset(0.9);
    
    hFoldedSpectrum->Draw("HIST E");
    double raw_count = hFoldedSpectrum->GetEntries();
    double raw_count_error = std::sqrt(raw_count);
    numbers_file << "\\newcommand\\SibNRaw" << Particle << "{\\num[round-precision=3, round-mode=figures, scientific-notation=engineering]{" << raw_count << "\\pm" << raw_count_error << "}}" << std::endl;
    numbers_file << "\\newcommand\\SibNRaw" << Particle << "Tab{\\num[round-precision=3, round-mode=figures]{" << raw_count/1e3 << "}(\\num[";
    int round_precision = 1;
    if (raw_count < 1000) {
      round_precision = 2;
    }
    numbers_file << "round-precision=" << round_precision << ", round-mode=figures]{";
    double raw_count_error_print = raw_count_error/1e3;
    while (raw_count_error_print < std::pow(10,round_precision-1)) { raw_count_error_print *= 10;}
    numbers_file << raw_count_error_print << "})}" << std::endl;
    //    numbers_file << std::fixed << std::setprecision(0) << "\\newcommand\\SibNRaw" << Particle << "Count{\\num{" << raw_count << "}}" << std::endl;
    //    numbers_file << std::fixed << std::setprecision(0) << "\\newcommand\\SibNRaw" << Particle << "Error{\\num{" << raw_count_error << "}}" << std::endl;
    //    numbers_file << std::fixed << std::setprecision(0) << "\\newcommand\\SibNRaw" << Particle << "{$\\SibNRaw" << Particle << "Count \\pm \\SibNRaw" << Particle << "Error$}" << std::endl;
    c_Spectra->Update();
    
    double rightmax = 1.1*hUnfoldedSpectrum->GetMaximum();
    double scale = gPad->GetUymax()/rightmax;
    hUnfoldedSpectrum->Scale(scale);
    hUnfoldedSpectrum->Draw("HIST E SAME");
    
    TGaxis* axis = new TGaxis(gPad->GetUxmax(), gPad->GetUymin(), gPad->GetUxmax(), gPad->GetUymax(),0,rightmax,510,"+L");
    axis->SetLineColor(unfolded_colours[i_particle]);
    axis->SetLabelColor(unfolded_colours[i_particle]);
    axis->SetTitleColor(unfolded_colours[i_particle]);
    axis->SetTitle(hUnfoldedSpectrum->GetYaxis()->GetTitle());
    axis->Draw();

    alcaphistogram(hFoldedSpectrum);
    alcaphistogram(hUnfoldedSpectrum);
    hFoldedSpectrum->SetDrawOption("HIST E1");
    hUnfoldedSpectrum->SetDrawOption("HIST E1 SAME");
    alcapPreliminary(hFoldedSpectrum);

    std::string leglabel = "folded " + particle + "s";
    leg->AddEntry(hFoldedSpectrum, leglabel.c_str(), "l");
    leglabel = "unfolded " + particle + "s";
    leg->AddEntry(hUnfoldedSpectrum, leglabel.c_str(), "l");
    leg->Draw();
      
    if (savedir != "") {
      std::string savename = savedir + "AlCapData_Si16bDataset_UnfoldedAndFoldedSpectra_" + particle;
      
      std::string pdfname = savename + ".pdf";
      c_Spectra->SaveAs(pdfname.c_str());
      std::string pngname = savename + ".png";
      c_Spectra->SaveAs(pngname.c_str());
    }
  }
  numbers_file << std::endl;

  /*
  double trust_at_observed = 11000;
  double up_to = 15000;
  int trust_bin = hFoldedSpectrum->GetXaxis()->FindBin(trust_at_observed);
  int up_to_bin = hFoldedSpectrum->GetXaxis()->FindBin(up_to);
  int max_bin = hFoldedSpectrum->GetXaxis()->FindBin(x_max);
  std::cout << "Folded Spectrum (0 -- " << trust_at_observed/1000 << " MeV) = " << hFoldedSpectrum->Integral(1, trust_bin) << std::endl;
  std::cout << "Folded Spectrum (" << trust_at_observed/1000 << " -- " << x_max/1000 << " MeV) = " << hFoldedSpectrum->Integral(trust_bin, max_bin) << std::endl;
  std::cout << "Folded Spectrum (" << trust_at_observed/1000 << " -- " << up_to/1000 << " MeV) = " << hFoldedSpectrum->Integral(trust_bin, up_to_bin) << std::endl;
  */
}
