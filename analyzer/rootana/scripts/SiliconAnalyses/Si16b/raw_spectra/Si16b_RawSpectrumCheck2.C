void Si16b_RawSpectrumCheck2() {

  std::string infilename = "~/data/results/Si16b/raw_spectra.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");
  
  const int n_components = 4;
  std::string indirnames[n_components] = {"proton_combined", "deuteron", "triton", "alpha"};
  Int_t colours[n_components] = {kRed, kCyan, kMagenta, kGreen};
  
  THStack* total = new THStack("total", "");
  for (int i_component = 0; i_component < n_components; ++i_component) {
    std::string folded_histname = indirnames[i_component] + "/hRawSpectrum";
  
    TH1F* hFoldedSpectrum = (TH1F*) infile->Get(folded_histname.c_str());

    //  hFoldedSpectrum->SetTitle(outhisttitle.c_str());
    hFoldedSpectrum->Rebin(50);
    hFoldedSpectrum->SetStats(false);
    hFoldedSpectrum->SetLineColor(kBlack);
    hFoldedSpectrum->SetFillColor(colours[i_component]);
    hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 15000);
    hFoldedSpectrum->GetYaxis()->SetTitleOffset(1.3);
  
    total->Add(hFoldedSpectrum);
  }
  total->Draw("nostack");
}
