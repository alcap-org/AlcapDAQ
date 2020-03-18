void Si16b_FinalPlot_Target_RawSpectrum(std::string savedir = "") {

  std::string raw_spectra_file_name = "~/data/results/Si16b/raw_spectra_newPP_geq1TgtPulse.root";
  std::string full_spectrum_name = "TargetMuons/hRawSpectrum";
  std::string cuttree_name = "TargetMuons/cuttree";

  TCanvas* c1 = new TCanvas();
  
  TFile* raw_spectra_file = new TFile(raw_spectra_file_name.c_str(), "READ");
  TH1F* hRawSpectrum = (TH1F*) raw_spectra_file->Get(full_spectrum_name.c_str());
  TTree* cuttree = (TTree*) raw_spectra_file->Get(cuttree_name.c_str());

  double min_time_cut = 0;
  double max_time_cut = 0;
  cuttree->SetBranchAddress("min_time", &min_time_cut);
  cuttree->SetBranchAddress("max_time", &max_time_cut);
  cuttree->GetEntry(0);

  double min_muon_energy = 700;
  double max_muon_energy = 1500;
  int min_energy_bin = hRawSpectrum->GetXaxis()->FindBin(min_muon_energy);
  int max_energy_bin = hRawSpectrum->GetXaxis()->FindBin(max_muon_energy);
  double direct_count = hRawSpectrum->Integral(min_energy_bin, max_energy_bin);
  //  std::cout << "AE: Integral = " << direct_count << std::endl;
  
  TCanvas* c_RawSpectrum = new TCanvas("c_RawSpectrum", "c_RawSpectrum");

  std::string histtitle = "Si16b Dataset, Target Spectrum, Energy";
  hRawSpectrum->SetTitle(histtitle.c_str());
  hRawSpectrum->SetStats(false);
  hRawSpectrum->GetXaxis()->SetRangeUser(0,4000);
  hRawSpectrum->GetXaxis()->SetTitleOffset(0.9);
  
  std::stringstream axislabel;
  axislabel << "Counts / " << hRawSpectrum->GetBinWidth(1) << " keV";
  hRawSpectrum->SetYTitle(axislabel.str().c_str());
  hRawSpectrum->GetYaxis()->SetTitleOffset(0.9);

  hRawSpectrum->Draw("HIST E");
  
  alcaphistogram(hRawSpectrum);
  hRawSpectrum->SetDrawOption("HIST E1");
  alcapPreliminary(hRawSpectrum);

  TLatex* latex = new TLatex();
  std::stringstream text;
  text.str("");
  text << min_time_cut << " ns < t < " << max_time_cut << "ns";
  latex->DrawLatexNDC(0.35, 0.5, text.str().c_str());

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_Si16bDataset_Target_RawSpectrum";

    std::string pdfname = savename + ".pdf";
    c_RawSpectrum->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c_RawSpectrum->SaveAs(pngname.c_str());
  }
}
