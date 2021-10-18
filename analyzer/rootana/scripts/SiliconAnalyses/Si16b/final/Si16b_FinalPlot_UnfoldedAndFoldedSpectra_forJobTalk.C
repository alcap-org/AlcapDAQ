void Si16b_FinalPlot_UnfoldedAndFoldedSpectra_forJobTalk(std::string savedir = "") {

  const int n_particles = 1;//4;
  std::string particles[n_particles] = {"proton"};//{"proton"};//, "deuteron", "triton", "alpha"};
  double x_maxes[n_particles] = {25};//{17, 17, 17, 19};
  //  double x_maxes[n_particles] = {25, 25, 25, 25};
  Int_t unfolded_colours[n_particles] = {kRed};//, kCyan, kMagenta, kSpring};
  
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    TLegend* leg = new TLegend(0.55,0.60,0.85,0.85);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.035);
    leg->SetFillColor(kWhite);

    std::string particle = particles[i_particle];
    //    std::string infilename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_JohnVetoEff_1.root";
    //    std::string indirname = "ResponseMatrix_" + particle + "_TCutG_2sig_layerCoinc500ns_tGT0ns";
    std::string infilename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_JohnVetoEff_100MTests_1.root";
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
    axistitle.str(""); axistitle << "Folded Count / " << hFoldedSpectrum->GetXaxis()->GetBinWidth(1) << " MeV";
    hFoldedSpectrum->SetYTitle(axistitle.str().c_str());
    hFoldedSpectrum->SetXTitle("Energy [MeV]");
    hFoldedSpectrum->GetXaxis()->SetTitleOffset(1.0);
    hFoldedSpectrum->GetYaxis()->SetTitleOffset(1.0);
    hFoldedSpectrum->GetXaxis()->SetLabelSize(0.04);
    hFoldedSpectrum->GetYaxis()->SetLabelSize(0.04);
    std::string newname = hFoldedSpectrum->GetName();
    newname += "_Ratio";
    TH1F* hRatio = (TH1F*) hFoldedSpectrum->Clone(newname.c_str());
    hRatio->Reset();
    for (int i_bin = 1; i_bin <= hFoldedSpectrum->GetNbinsX(); ++i_bin) {
      std::cout << "Bin #" << i_bin << ": " << hFoldedSpectrum->GetBinContent(i_bin) << " +/- " << hFoldedSpectrum->GetBinError(i_bin) << " (" << (hFoldedSpectrum->GetBinError(i_bin) / hFoldedSpectrum->GetBinContent(i_bin))*100 << "%, sqrt(N) = " << std::sqrt(hFoldedSpectrum->GetBinContent(i_bin)) << ")" << std::endl;
      if (i_bin > 1) {
	if (hFoldedSpectrum->GetBinContent(i_bin-1)>0) {
	  hRatio->SetBinContent(i_bin, hFoldedSpectrum->GetBinContent(i_bin)/hFoldedSpectrum->GetBinContent(i_bin-1));
	}
      }
    }
    TCanvas* cRatio = new TCanvas();
    hRatio->Draw("HIST");

    c_Spectra->cd();
    hUnfoldedSpectrum->SetTitle("");
    hUnfoldedSpectrum->SetStats(false);
    hUnfoldedSpectrum->SetLineColor(unfolded_colours[i_particle]);
    hUnfoldedSpectrum->Rebin(rebin_factor);
    hUnfoldedSpectrum->GetXaxis()->SetRangeUser(0, x_max);
    axistitle.str(""); axistitle << "Unfolded Count / " << hUnfoldedSpectrum->GetXaxis()->GetBinWidth(1) << " MeV";
    hUnfoldedSpectrum->SetYTitle(axistitle.str().c_str());
    hUnfoldedSpectrum->SetXTitle("Energy [MeV]");
    hUnfoldedSpectrum->GetXaxis()->SetTitleOffset(1.0);
    hUnfoldedSpectrum->GetYaxis()->SetTitleOffset(1.0);
    hUnfoldedSpectrum->GetXaxis()->SetLabelSize(0.04);
    hUnfoldedSpectrum->GetYaxis()->SetLabelSize(0.04);
    for (int i_bin = 1; i_bin <= hUnfoldedSpectrum->GetNbinsX(); ++i_bin) {
      std::cout << "Unf Bin #" << i_bin << ": " << hUnfoldedSpectrum->GetBinContent(i_bin) << " +/- " << hUnfoldedSpectrum->GetBinError(i_bin) << " (" << (hUnfoldedSpectrum->GetBinError(i_bin) / hUnfoldedSpectrum->GetBinContent(i_bin))*100 << "%)" << std::endl;
    }

    double minE = 5;
    double maxE = 17;
    double error = 0;
    //    std::cout << "AE: " << hUnfoldedSpectrum->GetMaximum() << std::endl;
    int minE_bin = hUnfoldedSpectrum->FindBin(minE);
    int maxE_bin = hUnfoldedSpectrum->FindBin(maxE)-1;
    double n_cap = 2.06848e+07;
    std::cout << "Integral = " << hUnfoldedSpectrum->IntegralAndError(minE_bin,maxE_bin,error)/n_cap << " +/- " << error/n_cap << std::endl;    
    hFoldedSpectrum->Draw("HIST E");
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

    //    alcaphistogram(hFoldedSpectrum);
    //    alcaphistogram(hUnfoldedSpectrum);
    //    hFoldedSpectrum->SetDrawOption("HIST E1");
    //    hUnfoldedSpectrum->SetDrawOption("HIST E1 SAME");
    //    alcapPreliminary(hFoldedSpectrum);
    TLatex* latex = new TLatex();
    latex->SetTextAlign(22);
    latex->DrawLatexNDC(0.7, 0.5, "AlCap #it{#bf{Preliminary}}");

    latex->SetTextSize(0.04);
    latex->DrawLatexNDC(0.7, 0.45, "stat. errors only");

    std::string leglabel = "folded " + particle + "s";
    leg->AddEntry(hFoldedSpectrum, leglabel.c_str(), "l");
    leglabel = "unfolded " + particle + "s";
    leg->AddEntry(hUnfoldedSpectrum, leglabel.c_str(), "l");
    leg->Draw();
      
    if (savedir != "") {
      std::string savename = savedir + "AlCapData_Si16bDataset_UnfoldedAndFoldedSpectra_" + particle + "_forJobTalk";
      
      std::string pdfname = savename + ".pdf";
      c_Spectra->SaveAs(pdfname.c_str());
      std::string pngname = savename + ".png";
      c_Spectra->SaveAs(pngname.c_str());
    }
  }

  /*
  double trust_at_observed = 11;
  double up_to = 15;
  int trust_bin = hFoldedSpectrum->GetXaxis()->FindBin(trust_at_observed);
  int up_to_bin = hFoldedSpectrum->GetXaxis()->FindBin(up_to);
  int max_bin = hFoldedSpectrum->GetXaxis()->FindBin(x_max);
  std::cout << "Folded Spectrum (0 -- " << trust_at_observed << " MeV) = " << hFoldedSpectrum->Integral(1, trust_bin) << std::endl;
  std::cout << "Folded Spectrum (" << trust_at_observed << " -- " << x_max << " MeV) = " << hFoldedSpectrum->Integral(trust_bin, max_bin) << std::endl;
  std::cout << "Folded Spectrum (" << trust_at_observed << " -- " << up_to << " MeV) = " << hFoldedSpectrum->Integral(trust_bin, up_to_bin) << std::endl;
  */
}
