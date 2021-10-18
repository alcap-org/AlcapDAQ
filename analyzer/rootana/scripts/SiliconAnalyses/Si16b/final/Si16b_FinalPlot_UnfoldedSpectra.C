void Si16b_FinalPlot_UnfoldedSpectra() {

  const int n_particles = 4;
  std::string particle_names[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  Int_t colours[n_particles] = {kRed, kCyan, kMagenta, kSpring};
  TH1D* hOutHists[n_particles];
  
  std::string infilename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_2.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  double n_captured_muons = 2.06848e+07;
  //  THStack* hStack = new THStack("hStack", "");
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particle_names[i_particle];
    Int_t colour = colours[i_particle];
    
    std::string unfolded_histname = "ResponseMatrix_" + particle + "_TCutG_2sig_layerCoinc500ns_tGT0ns_BinW500keV/hCorrectedSpectrum";
    std::string outhisttitle = "Si16b Dataset, Right Arm, " + particle;
  
    TH1F* hUnfoldedSpectrum = (TH1F*) infile->Get(unfolded_histname.c_str());

    hUnfoldedSpectrum->SetTitle("");
    hUnfoldedSpectrum->SetStats(false);
    hUnfoldedSpectrum->SetLineColor(colour);
    hUnfoldedSpectrum->SetLineWidth(2);
    hUnfoldedSpectrum->GetXaxis()->SetRangeUser(0, 25);
    hUnfoldedSpectrum->Draw("HIST E SAME");
    //    hStack->Add(hUnfoldedSpectrum);

    if (particle == "proton") {
      for (int i_bin = 1; i_bin <= hUnfoldedSpectrum->GetNbinsX(); ++i_bin) {
	double E = hUnfoldedSpectrum->GetXaxis()->GetBinCenter(i_bin);
	std::cout << E << " MeV = " << hUnfoldedSpectrum->GetBinContent(i_bin) << " +/- " << hUnfoldedSpectrum->GetBinError(i_bin) << std::endl;
      }
    }

    //    hUnfoldedSpectrum->Scale(1.0/n_captured_muons);

    std::string outhistname = "h";
    outhistname += particle;
    outhistname += "_SiR";
    hOutHists[i_particle] = (TH1D*) hUnfoldedSpectrum->Clone(outhistname.c_str());
    hOutHists[i_particle]->SetTitle(particle.c_str());
    hOutHists[i_particle]->SetDirectory(0);
  }
  
  //  TCanvas* c_Spectra = new TCanvas("c_Spectra", "c_Spectra");
  //  hStack->Draw("HIST E nostack");
  //  hStack->Draw("HIST E");
  //  hStack->GetXaxis()->SetRangeUser(0, 15000);
  //  c_Spectra->Update();
  
  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");

  TFile* outfile = new TFile("~/data/results/Si16b/si16b-unfolded.root", "RECREATE");
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    hOutHists[i_particle]->Write();
  }
  outfile->Write();
  outfile->Close();
}
