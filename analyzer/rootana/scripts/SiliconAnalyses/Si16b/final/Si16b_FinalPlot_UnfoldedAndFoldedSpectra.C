void Si16b_FinalPlot_UnfoldedAndFoldedSpectra() {

  std::string particle = "proton";
  //  std::string particle = "deuteron";
  //  std::string particle = "triton";
  //    std::string particle = "alpha";
  std::string infilename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse.root";
  std::string indirname = "ResponseMatrix_" + particle + "_PSel";
  std::string folded_histname = indirname + "/hInputSpectrum";
  std::string unfolded_histname = indirname + "/hCorrectedSpectrum";
  std::string outhisttitle = "Si16b Dataset, Right Arm, " + particle;
  int rebin_factor = 1;
  double x_max = 25000;
  
  TFile* infile = new TFile(infilename.c_str(), "READ");
  TH1F* hFoldedSpectrum = (TH1F*) infile->Get(folded_histname.c_str());
  if (!hFoldedSpectrum) {
    std::cout << "Can't find histogram " << folded_histname.c_str() << std::endl;
    return;
  }
  TH1F* hUnfoldedSpectrum = (TH1F*) infile->Get(unfolded_histname.c_str());

  TCanvas* c_Spectra = new TCanvas("c_Spectra", "c_Spectra");
  std::stringstream axistitle;
  
  hFoldedSpectrum->SetTitle(outhisttitle.c_str());
  hFoldedSpectrum->SetStats(false);
  hFoldedSpectrum->SetLineColor(kBlack);
  hFoldedSpectrum->Rebin(rebin_factor);
  hFoldedSpectrum->GetXaxis()->SetRangeUser(0, x_max);
  axistitle.str(""); axistitle << "Folded Count / " << hFoldedSpectrum->GetXaxis()->GetBinWidth(1) << " keV";
  hFoldedSpectrum->SetYTitle(axistitle.str().c_str());
  hFoldedSpectrum->GetYaxis()->SetTitleOffset(1.3);
  
  hUnfoldedSpectrum->SetTitle("");
  hUnfoldedSpectrum->SetStats(false);
  hUnfoldedSpectrum->SetLineColor(kRed);
  hUnfoldedSpectrum->Rebin(rebin_factor);
  hUnfoldedSpectrum->GetXaxis()->SetRangeUser(0, x_max);
  axistitle.str(""); axistitle << "Unfolded Count / " << hUnfoldedSpectrum->GetXaxis()->GetBinWidth(1) << " keV";
  hUnfoldedSpectrum->SetYTitle(axistitle.str().c_str());

  hFoldedSpectrum->Draw("HIST E");
  c_Spectra->Update();
  
  double rightmax = 1.1*hUnfoldedSpectrum->GetMaximum();
  double scale = gPad->GetUymax()/rightmax;
  hUnfoldedSpectrum->Scale(scale);
  hUnfoldedSpectrum->Draw("HIST E SAME");
  
  TGaxis* axis = new TGaxis(gPad->GetUxmax(), gPad->GetUymin(), gPad->GetUxmax(), gPad->GetUymax(),0,rightmax,510,"+L");
  axis->SetLineColor(kRed);
  axis->SetLabelColor(kRed);
  axis->SetTitleColor(kRed);
  axis->SetTitle(hUnfoldedSpectrum->GetYaxis()->GetTitle());
  axis->Draw();

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");

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
