void Si16b_FinalPlot_UnfoldedSpectra() {

  std::string particle = "proton";
  //  std::string particle = "deuteron";
  std::string infilename = "~/data/results/Si16b_passive/unfold.root";
  std::string folded_histname = particle + "/hFoldedSpectrum";
  std::string unfolded_histname = particle + "/hUnfoldedSpectrum";
  std::string outhisttitle = "Si16b Dataset, Right Arm, " + particle;
  
  TFile* infile = new TFile(infilename.c_str(), "READ");
  TH1F* hFoldedSpectrum = (TH1F*) infile->Get(folded_histname.c_str());
  TH1F* hUnfoldedSpectrum = (TH1F*) infile->Get(unfolded_histname.c_str());

  TCanvas* c_Spectra = new TCanvas("c_Spectra", "c_Spectra");

  hFoldedSpectrum->SetTitle(outhisttitle.c_str());
  hFoldedSpectrum->SetStats(false);
  hFoldedSpectrum->SetLineColor(kBlack);
  hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 15000);
  hFoldedSpectrum->GetYaxis()->SetTitleOffset(1.3);
  
  hUnfoldedSpectrum->SetTitle("");
  hUnfoldedSpectrum->SetStats(false);
  hUnfoldedSpectrum->SetLineColor(kRed);
  hUnfoldedSpectrum->GetXaxis()->SetRangeUser(0, 15000);

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
}
