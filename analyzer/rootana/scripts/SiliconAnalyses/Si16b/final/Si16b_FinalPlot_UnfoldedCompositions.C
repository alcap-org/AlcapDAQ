void Si16b_FinalPlot_UnfoldedCompositions() {

  const int n_particles = 3;
  std::string particle_names[n_particles] = {"proton", "deuteron", "triton"};//, "alpha"};
  Int_t colours[n_particles] = {kRed, kCyan, kMagenta};//, kSpring};
  int rebin_factor = 5;
  
  std::string infilename = "~/data/results/Si16b/unfold_newPP.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  int i_numerator_particle = 0;
  TH1F* hNumerator = 0;
  TH1F* hDenominator;
  std::stringstream axistitle;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particle_names[i_particle];
    Int_t colour = colours[i_particle];

    //    std::string unfolded_histname = "ResponseMatrix_" + particle + "/hInputSpectrum";
    std::string unfolded_histname = "ResponseMatrix_" + particle + "/hCorrectedSpectrum";
    std::string outhisttitle = "Si16b Dataset, Right Arm, " + particle;
  
    TH1F* hUnfoldedSpectrum = (TH1F*) infile->Get(unfolded_histname.c_str());

    hUnfoldedSpectrum->SetTitle("");
    hUnfoldedSpectrum->SetStats(false);
    hUnfoldedSpectrum->Rebin(rebin_factor);
    hUnfoldedSpectrum->SetLineColor(colour);
    hUnfoldedSpectrum->SetLineWidth(2);
    hUnfoldedSpectrum->GetXaxis()->SetRangeUser(0, 15000);
    axistitle.str(""); axistitle << "Unfolded Count / " << hUnfoldedSpectrum->GetXaxis()->GetBinWidth(1) << " keV";
    hUnfoldedSpectrum->SetYTitle(axistitle.str().c_str());

    if (i_particle == i_numerator_particle) {
      hNumerator = (TH1F*) hUnfoldedSpectrum->Clone("hNumerator");
    }
    else {
      if(!hDenominator) {
	hDenominator = (TH1F*) hUnfoldedSpectrum->Clone("hDenominator");
      }
      else {
	hDenominator->Add(hUnfoldedSpectrum);
      }
    }
  }

  TCanvas* c_both = new TCanvas("c_both", "c_both");
  //  hStack->Draw("HIST E nostack");
  hNumerator->Draw("HIST E");
  hDenominator->Draw("HIST E SAMES");

  
  TH1F* hRatio = (TH1F*) hNumerator->Clone("hRatio");
  hRatio->Divide(hDenominator);
  TCanvas* c_Spectra = new TCanvas("c_Spectra", "c_Spectra");
  //  hStack->Draw("HIST E nostack");
  hRatio->Draw("HIST E");
  
  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
}
