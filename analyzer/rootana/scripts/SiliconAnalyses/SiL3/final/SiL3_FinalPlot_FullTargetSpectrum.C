void SiL3_FinalPlot_FullTargetSpectrum() {

  std::string plots_file_name = "~/data/results/SiL3_active/plots.root";
  std::string full_spectrum_name = "Target/hEnergyTime";
  std::string norm_file_name = "/home/edmonds/data/results/SiL3_active/normalisation.root";
  std::string norm_ws_name = "ws_timecut";
    
  TFile* plots_file = new TFile(plots_file_name.c_str(), "READ");
  TH1D* hTargetSpectrum = ((TH2F*) plots_file->Get(full_spectrum_name.c_str()))->ProjectionY();

  //  TFile* norm_file = new TFile(norm_file_name.c_str(), "READ");
  //  RooWorkspace* ws = (RooWorkspace*) norm_file->Get(norm_ws_name.c_str());
  
  TCanvas* c_TargetSpectrum = new TCanvas("c_TargetSpectrum", "c_TargetSpectrum");

  //  hTargetSpectrum->Rebin(4);
  //  hTargetSpectrum->SetLineColor(kBlue);
  std::string histtitle = "SiL3 Dataset, Full Target Spectrum";
  hTargetSpectrum->SetTitle(histtitle.c_str());
  hTargetSpectrum->SetStats(false);
  hTargetSpectrum->GetXaxis()->SetRangeUser(0, 30000);

  std::stringstream axislabel;
  axislabel << "Counts / " << hTargetSpectrum->GetBinWidth(1) << " keV";
  hTargetSpectrum->SetYTitle(axislabel.str().c_str());
  hTargetSpectrum->GetYaxis()->SetTitleOffset(1.3);
  
  hTargetSpectrum->Draw("HIST E");

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.50, 0.70, "AlCap Preliminary");

}
