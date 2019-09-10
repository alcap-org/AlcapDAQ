void Si16b_FinalPlot_RawSpectra_wSiL1Coinc() {

  const int n_particles = 4;
  std::string particle_names[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  Int_t colours[n_particles] = {kRed, kCyan, kMagenta, kSpring};

  const int n_channels = 16;

  std::string infilename = "~/data/results/Si16b/raw_spectra_newPP.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");
  
  std::stringstream axistitle, canvasname, histname;;
  int rebin_factor = 100;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particle_names[i_particle];
    Int_t colour = colours[i_particle];

    canvasname.str("");
    canvasname << "c_" << particle;
    TCanvas* c = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());

    TLegend* leg = new TLegend(0.50,0.45,0.80,0.75);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.035);
    leg->SetFillColor(kWhite);

    TH2F* h2D = 0;
    for (int i_channel = 2; i_channel < n_channels; ++i_channel) {
      histname.str("");
      histname << particle << "_SiL1_" << i_channel << "/hRawSpectrum";
      
      std::string folded_histname = histname.str();
      std::string outhisttitle = "Si16b Dataset, Right Arm, " + particle;
  
      TH1F* hFoldedSpectrum = (TH1F*) infile->Get(folded_histname.c_str());     

      hFoldedSpectrum->Rebin(rebin_factor);
      axistitle.str("");
      axistitle << "Raw Count / " << hFoldedSpectrum->GetXaxis()->GetBinWidth(1) << " keV";
      hFoldedSpectrum->SetTitle("Si16b Dataset, Right Arm, Raw Spectrum");
      hFoldedSpectrum->SetYTitle(axistitle.str().c_str());
      hFoldedSpectrum->SetStats(false);
      hFoldedSpectrum->SetLineColor(i_channel);
      hFoldedSpectrum->SetLineWidth(2);
      hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 20000);
      hFoldedSpectrum->GetYaxis()->SetRangeUser(0, 80);
    
      /*      hFoldedSpectrum->Draw("HIST E SAME");

      std::stringstream leglabel;
      leglabel.str("");
      leglabel.str("");
      leglabel << "w/ SiL1-" << i_channel << " Coinc (" << hFoldedSpectrum->Integral() << " entries)";
      leg->AddEntry(hFoldedSpectrum, leglabel.str().c_str(), "l");
      */
      if (!h2D) {
	h2D = new TH2F("h2D", "", hFoldedSpectrum->GetNbinsX(), hFoldedSpectrum->GetXaxis()->GetXmin(), hFoldedSpectrum->GetXaxis()->GetXmax(), n_channels,0.5,n_channels+0.5);
      }
      for (int i_bin = 1; i_bin <= hFoldedSpectrum->GetNbinsX(); ++i_bin) {
	double bin_center = hFoldedSpectrum->GetBinCenter(i_bin);
	double bin_content = hFoldedSpectrum->GetBinContent(i_bin);
	h2D->Fill(bin_center, i_channel, bin_content);
      }
    }

    h2D->Draw("COLZ");
    leg->Draw();
  
    TLatex* latex = new TLatex();
    latex->DrawLatexNDC(0.55, 0.80, "AlCap Preliminary");
  }
}
