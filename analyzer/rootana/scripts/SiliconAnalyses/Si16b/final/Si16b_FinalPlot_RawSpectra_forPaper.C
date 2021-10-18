void Si16b_FinalPlot_RawSpectra_forPaper() {

  TCanvas* c = new TCanvas();
  c->SetLogy();
  
  const int n_particles = 4;
  std::string particle_names[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  Int_t colours[n_particles] = {kRed, kBlue, kGreen, kMagenta};
  //  double highE_cut[n_particles] = {20, 17, 17, 25};
  
  std::string infilename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_2.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  TLegend* leg = new TLegend(0.65,0.65,0.85,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);
  
  std::stringstream axistitle;
  int rebin_factor = 1;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particle_names[i_particle];
    Int_t colour = colours[i_particle];
    
    std::string folded_histname = "ResponseMatrix_" + particle + "_TCutG_2sig_layerCoinc500ns_tGT0ns_BinW500keV/hInputSpectrum";
    std::string outhisttitle = "Si16b Dataset, Right Arm, " + particle;
  
    TH1F* hFoldedSpectrum = (TH1F*) infile->Get(folded_histname.c_str());

    hFoldedSpectrum->Rebin(rebin_factor);
    axistitle.str("");
    axistitle << "Raw Count / " << hFoldedSpectrum->GetXaxis()->GetBinWidth(1) << " MeV";
    hFoldedSpectrum->SetTitle("Si16b Dataset, Right Arm, Raw Spectrum");
    hFoldedSpectrum->SetYTitle(axistitle.str().c_str());
    hFoldedSpectrum->SetXTitle("Observed Energy [MeV]");
    hFoldedSpectrum->GetXaxis()->SetTitleOffset(1.1);
    hFoldedSpectrum->SetStats(false);
    hFoldedSpectrum->SetLineColor(colour);
    hFoldedSpectrum->SetLineWidth(2);
    hFoldedSpectrum->SetMinimum(0.5);
    //    hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 15000);
    
    hFoldedSpectrum->Draw("HIST E SAME");

    std::stringstream leglabel;
    leglabel.str("");
    leglabel.str("");
    leglabel << particle;// << " (" << hFoldedSpectrum->Integral() << " entries)";
    leg->AddEntry(hFoldedSpectrum, leglabel.str().c_str(), "l");
  }

  leg->SetHeader("AlCap Si");
  leg->Draw();
  
  //  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.55, 0.80, "AlCap Preliminary");
}
