void Si16b_FinalPlot_RawSpectra() {

  const int n_particles = 4;
  std::string particle_names[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  Int_t colours[n_particles] = {kRed, kCyan, kMagenta, kSpring};
  
  std::string infilename = "~/data/results/Si16b/raw_spectra_newPP.root";
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
    
    std::string folded_histname = particle + "_PSel/hRawSpectrum";
    std::string outhisttitle = "Si16b Dataset, Right Arm, " + particle;
  
    TH1F* hFoldedSpectrum = (TH1F*) infile->Get(folded_histname.c_str());

    hFoldedSpectrum->Rebin(rebin_factor);
    axistitle.str("");
    axistitle << "Raw Count / " << hFoldedSpectrum->GetXaxis()->GetBinWidth(1) << " keV";
    hFoldedSpectrum->SetTitle("Si16b Dataset, Right Arm, Raw Spectrum");
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
  }

  leg->Draw();
  
  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.80, "AlCap Preliminary");
}
