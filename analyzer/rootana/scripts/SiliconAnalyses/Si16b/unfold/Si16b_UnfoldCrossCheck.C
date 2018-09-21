void Si16b_UnfoldCrossCheck() {

  std::string filename = "~/data/results/Si16b_passive/unfold.root";
  TFile* file = new TFile(filename.c_str(), "READ");


  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();

  TLegend* leg = new TLegend(0.30,0.55,0.70,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);

  std::string i_foldername = "DecayElectronCorrection";
  std::string i_histname = i_foldername + "/hCorrectedSpectrum";
  //  std::string i_histname = i_foldername + "/hInputSpectrum";

  TH1F* spectrum = (TH1F*) file->Get(i_histname.c_str());
  if (!spectrum) {
    std::cout << "Error: Problem getting spectrum " << i_spectraname << std::endl;
    return;
  }
  //    spectrum->Sumw2();
  //    spectrum->Rebin(2);
  spectrum->SetLineColor(kRed);
  spectrum->Draw("HIST E SAMES");

  i_histname = i_foldername + "/hCorrection";
  TH1F* correction = (TH1F*) file->Get(i_histname.c_str());
  if (!correction) {
    std::cout << "Error: Problem getting correction " << i_spectraname << std::endl;
    return;
  }
  correction->SetLineColor(kBlue);
  correction->Draw("HIST E SAMES");

  TLine* peak_line = new TLine(2500, spectrum->GetMinimum(), 2500, spectrum->GetMaximum());
  peak_line->SetLineColor(kBlack);
  peak_line->Draw("LSAME");
  
  TLine* trough_line = new TLine(1400, spectrum->GetMinimum(), 1400, spectrum->GetMaximum());
  trough_line->SetLineColor(kBlack);
  trough_line->Draw("LSAME");

  leg->Draw();
}
