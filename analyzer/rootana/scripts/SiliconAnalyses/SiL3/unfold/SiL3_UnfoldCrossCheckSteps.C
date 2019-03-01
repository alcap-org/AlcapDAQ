void SiL3_UnfoldCrossCheckSteps() {

  std::string filename = "~/data/results/SiL3/unfold.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  double min_time_slice = 2200;
  double max_time_slice = 2300;

  std::stringstream time_slice_str;
  time_slice_str.str("");
  time_slice_str << "TimeSlice" << min_time_slice << "_" << max_time_slice;

  
  TCanvas* c1 = new TCanvas("c1", "c1");
  //  c1->SetLogy();

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);


  std::string i_foldername = "FlatBackground_" + time_slice_str.str();
  std::string i_raw_spectrum = i_foldername + "/hRawSpectrum";
  TH1F* raw_spectrum = (TH1F*) file->Get(i_raw_spectrum.c_str());
  if (!raw_spectrum) {
    std::cout << "Error: Problem getting spectrum " << i_raw_spectrum << std::endl;
    return;
  }
  raw_spectrum->SetLineColor(kBlack);
  raw_spectrum->SetStats(false);
  raw_spectrum->GetXaxis()->SetRangeUser(0,5000);
  raw_spectrum->Draw("HIST");
  leg->AddEntry(raw_spectrum, "Raw Spectrum", "l");
  
  /*  std::string i_flat_correction = i_foldername + "/hCorrection";
  TH1F* flat_correction = (TH1F*) file->Get(i_flat_correction.c_str());
  if (!flat_correction) {
    std::cout << "Error: Problem getting spectrum " << i_flat_correction << std::endl;
    return;
  }
  flat_correction->SetLineColor(kBlue);
  flat_correction->SetLineStyle(kDashed);
  flat_correction->Draw("HIST SAME");
  leg->AddEntry(flat_correction, "Flat Bkg Correction (from data)", "l");

  std::string i_after_flat_correction = i_foldername + "/hCorrectedSpectrum";
  TH1F* after_flat_correction = (TH1F*) file->Get(i_after_flat_correction.c_str());
  if (!after_flat_correction) {
    std::cout << "Error: Problem getting spectrum " << i_after_flat_correction << std::endl;
    return;
  }
  after_flat_correction->SetLineColor(kBlue);
  after_flat_correction->Draw("HIST SAME");
  leg->AddEntry(after_flat_correction, "After Flat Bkg Correction", "l");
  */
  std::string j_foldername = "DecayElectronCorrection_" + time_slice_str.str();
  std::string i_decay_electron_correction = j_foldername + "/hCorrection";
  TH1F* decay_electron_correction = (TH1F*) file->Get(i_decay_electron_correction.c_str());
  if (!decay_electron_correction) {
    std::cout << "Error: Problem getting spectrum " << i_decay_electron_correction << std::endl;
    return;
  }
  decay_electron_correction->SetLineColor(kRed);
  decay_electron_correction->SetLineStyle(kDashed);
  decay_electron_correction->Draw("HIST SAME");
  leg->AddEntry(decay_electron_correction, "Decay Electron Correction (from MC)", "l");
  
  std::string i_after_decay_electron_correction = j_foldername + "/hCorrectedSpectrum";
  TH1F* after_decay_electron_correction = (TH1F*) file->Get(i_after_decay_electron_correction.c_str());
  if (!after_decay_electron_correction) {
    std::cout << "Error: Problem getting spectrum " << i_after_decay_electron_correction << std::endl;
    return;
  }
  after_decay_electron_correction->SetLineColor(kRed);
  after_decay_electron_correction->Draw("HIST SAME");
  leg->AddEntry(after_decay_electron_correction, "After Decay Electron Correction", "l");

  /*
  TLine* peak_line = new TLine(2500, after_decay_electron_correction->GetMinimum(), 2500, after_decay_electron_correction->GetMaximum());
  peak_line->SetLineColor(kBlack);
  peak_line->Draw("LSAME");
  
  TLine* trough_line = new TLine(1400, after_decay_electron_correction->GetMinimum(), 1400, after_decay_electron_correction->GetMaximum());
  trough_line->SetLineColor(kBlack);
  trough_line->Draw("LSAME");
  */
  leg->Draw();
}
