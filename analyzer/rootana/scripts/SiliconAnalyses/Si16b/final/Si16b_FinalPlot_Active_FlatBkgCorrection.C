void Si16b_FinalPlot_Active_FlatBkgCorrection() {

  std::string filename = "~/data/results/Si16b/unfold_newPP20us_geq2TgtPulse.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
  c1->SetGridx();
  c1->SetGridy();

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);

  std::string foldername = "FlatBkg_Active";
  std::string histname = foldername + "/hRawSpectrum";
  TH1F* raw_spectrum = (TH1F*) file->Get(histname.c_str());
  if (!raw_spectrum) {
    std::cout << "Error: Problem getting raw_spectrum " << histname << std::endl;
    return;
  }
  histname = foldername + "/hCorrection";
  TH1F* correction = (TH1F*) file->Get(histname.c_str());
  if (!correction) {
    std::cout << "Error: Problem getting correction " << histname << std::endl;
    return;
  }
  histname = foldername + "/hCorrectedSpectrum";
  TH1F* corrected_spectrum = (TH1F*) file->Get(histname.c_str());
  if (!corrected_spectrum) {
    std::cout << "Error: Problem getting corrected_spectrum " << histname << std::endl;
    return;
  }
  
  raw_spectrum->SetTitle("Si16b Dataset, Active Target Analysis, Flat Background Correction");
  raw_spectrum->SetStats(false);
  raw_spectrum->GetXaxis()->SetRangeUser(0,9000);
  raw_spectrum->SetLineColor(kBlack);

  std::stringstream axislabel;
  axislabel << "Counts / " << raw_spectrum->GetBinWidth(1) << " keV";
  raw_spectrum->SetYTitle(axislabel.str().c_str());
  
  correction->SetLineColor(kBlue);
  corrected_spectrum->SetLineColor(kRed);
  
  raw_spectrum->Draw("HIST E");
  correction->Draw("HIST E SAME");
  corrected_spectrum->Draw("HIST E SAME");

  leg->AddEntry(raw_spectrum, "Raw Spectrum", "l");
  leg->AddEntry(correction, "Flat Bkg Correction", "l");
  leg->AddEntry(corrected_spectrum, "Corrected Spectrum", "l");

  leg->Draw();

  //    std::string pngname = "~/plots/2018-11-26/AlCapData_Si16bDataset_" + time_slice_str.str() + ".png";
  //    c1->SaveAs(pngname.c_str());
}
