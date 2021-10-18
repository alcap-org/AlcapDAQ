void Si16b_FinalPlot_RawSpectra() {

  const int n_particles = 4;
  std::string particle_names[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  Int_t colours[n_particles] = {kRed, kCyan, kMagenta, kSpring};
  TH1D* hOutHists[n_particles];
  
  std::string infilename = "~/data/results/Si16b/raw_spectra_newPP_geq1TgtPulse_2.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  TLegend* leg = new TLegend(0.50,0.45,0.80,0.75);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);
  
  std::stringstream axistitle;
  int rebin_factor = 5;//50;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particle_names[i_particle];
    Int_t colour = colours[i_particle];
    
    std::string folded_histname = particle + "_TCutG_2sig_layerCoinc500ns_tGT0ns/hRawSpectrum";
    std::string outhisttitle = "Si16b Dataset, Right Arm, " + particle;
  
    TH1F* hFoldedSpectrum = (TH1F*) infile->Get(folded_histname.c_str());

    hFoldedSpectrum->Rebin(rebin_factor);
    axistitle.str("");
    axistitle << "Raw Count / " << hFoldedSpectrum->GetXaxis()->GetBinWidth(1) << " MeV";
    hFoldedSpectrum->SetTitle("Si16b Dataset, Right Arm, Raw Spectrum");
    hFoldedSpectrum->SetYTitle(axistitle.str().c_str());
    hFoldedSpectrum->SetStats(false);
    hFoldedSpectrum->SetLineColor(colour);
    hFoldedSpectrum->SetLineWidth(2);
    //    hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 15000);
    
    hFoldedSpectrum->Draw("HIST E SAME");

    if (particle == "proton") {
      for (int i_bin = 1; i_bin <= hFoldedSpectrum->GetNbinsX(); ++i_bin) {
	double E = hFoldedSpectrum->GetXaxis()->GetBinCenter(i_bin);
      //      int i_bin = hFoldedSpectrum->FindBin(15);
	std::cout << E << " MeV = " << hFoldedSpectrum->GetBinContent(i_bin) << " +/- " << hFoldedSpectrum->GetBinError(i_bin) << std::endl;
      }
    }


    std::stringstream leglabel;
    leglabel.str("");
    leglabel.str("");
    leglabel << particle << " (" << hFoldedSpectrum->Integral() << " entries)";
    leg->AddEntry(hFoldedSpectrum, leglabel.str().c_str(), "l");

    std::string outhistname = "h";
    outhistname += particle[0];
    outhistname += "SiR";
    hOutHists[i_particle] = (TH1D*) hFoldedSpectrum->Clone(outhistname.c_str());
    hOutHists[i_particle]->SetTitle(particle.c_str());
    hOutHists[i_particle]->SetDirectory(0);
  }

  leg->Draw();
  
  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.80, "AlCap Preliminary");

  TFile* outfile = new TFile("~/data/results/Si16b/si16b-folded.root", "RECREATE");
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    hOutHists[i_particle]->Write();
  }
  outfile->Write();
  outfile->Close();
}
