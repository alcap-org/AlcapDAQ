void Si16b_FinalPlot_UnfoldedSpectra() {

  const int n_particles = 4;
  std::string particle_names[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  Int_t colours[n_particles] = {kRed, kCyan, kMagenta, kSpring};
  
  std::string infilename = "~/data/results/Si16b/unfold.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  THStack* hStack = new THStack("hStack", "");
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particle_names[i_particle];
    Int_t colour = colours[i_particle];
    
    std::string unfolded_histname = "ResponseMatrix_" + particle + "/hUnfoldedSpectrum";
    std::string outhisttitle = "Si16b Dataset, Right Arm, " + particle;
  
    TH1F* hUnfoldedSpectrum = (TH1F*) infile->Get(unfolded_histname.c_str());

    hUnfoldedSpectrum->SetTitle("");
    hUnfoldedSpectrum->SetStats(false);
    hUnfoldedSpectrum->SetLineColor(colour);
    hUnfoldedSpectrum->SetLineWidth(2);
    hUnfoldedSpectrum->GetXaxis()->SetRangeUser(0, 15000);
    
    hStack->Add(hUnfoldedSpectrum);
  }
  
  TCanvas* c_Spectra = new TCanvas("c_Spectra", "c_Spectra");
  //  hStack->Draw("HIST E nostack");
  hStack->Draw("HIST E");
  hStack->GetXaxis()->SetRangeUser(0, 15000);
  c_Spectra->Update();
  
  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
}
