void Ti50_FinalPlot_RawSpectra() {

  const int n_particles = 4;
  std::string particle_names[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  Int_t colours[n_particles] = {kRed, kCyan, kMagenta, kSpring};
  
  std::string infilename = "~/data/results/Ti50/raw_spectra_newPP_3sigma_layerCoinc500ns.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  TLegend* leg = new TLegend(0.50,0.45,0.80,0.75);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);
  
  std::stringstream axistitle;
  int rebin_factor = 10;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particle_names[i_particle];
    Int_t colour = colours[i_particle];
    
    //    std::string folded_histname = particle + "_PSel/hRawSpectrum";
    //    std::string folded_histname = particle + "_TCutG_noTimeCut_SiL/hRawSpectrum";
    std::string folded_histname = particle + "_TCutG_SiL/hRawSpectrum";
    std::string outhisttitle = "Ti50 Dataset, Right Arm, " + particle;
  
    TH1F* hFoldedSpectrum = (TH1F*) infile->Get(folded_histname.c_str());

    hFoldedSpectrum->Rebin(rebin_factor);
    axistitle.str("");
    axistitle << "Raw Count / " << hFoldedSpectrum->GetXaxis()->GetBinWidth(1) << " keV";
    hFoldedSpectrum->SetTitle("Ti50 Dataset, Right Arm, Raw Spectrum");
    hFoldedSpectrum->SetYTitle(axistitle.str().c_str());
    hFoldedSpectrum->SetStats(false);
    hFoldedSpectrum->SetLineColor(colour);
    hFoldedSpectrum->SetLineWidth(2);
    //    hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 15000);
    
    hFoldedSpectrum->Draw("HIST E SAME");

    std::stringstream leglabel;
    leglabel.str("");
    leglabel.str("");
    leglabel << particle << " (" << hFoldedSpectrum->Integral() << " entries)";
    leg->AddEntry(hFoldedSpectrum, leglabel.str().c_str(), "l");

    double min_energy = 3500;
    double max_energy = 10000;
    int min_energy_bin = hFoldedSpectrum->GetXaxis()->FindBin(min_energy);
    int max_energy_bin = hFoldedSpectrum->GetXaxis()->FindBin(max_energy)-1;
    double integral = hFoldedSpectrum->Integral(min_energy_bin, max_energy_bin);
    std::cout << particle << " (" << min_energy << " -- " << max_energy << " keV) = " << integral << " entries" << std::endl;

    min_energy = 4000;
    max_energy = 8000;
    min_energy_bin = hFoldedSpectrum->GetXaxis()->FindBin(min_energy);
    max_energy_bin = hFoldedSpectrum->GetXaxis()->FindBin(max_energy)-1;
    integral = hFoldedSpectrum->Integral(min_energy_bin, max_energy_bin);
    std::cout << particle << " (" << min_energy << " -- " << max_energy << " keV) = " << integral << " entries" << std::endl;
  }
  leg->Draw();
  
  
  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.80, "AlCap Preliminary");
}
