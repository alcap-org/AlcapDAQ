void Si16b_FinalPlot_SiR2Inc_DecayElectronCorrection() {

  std::string filename = "~/data/results/Si16b/unfold.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_slices = 1;
  double min_time_slice = 1000;
  double max_time_slice = 5000;
  double time_slice_step = 4000;
  Int_t colours[n_slices] = {kBlack};

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
  c1->SetGridx();
  c1->SetGridy();

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);

  std::stringstream time_slice_str;
  for (double i_min_time_slice = min_time_slice; i_min_time_slice < max_time_slice; i_min_time_slice += time_slice_step) {

    double i_max_time_slice = i_min_time_slice+time_slice_step;
    int i_slice = (i_min_time_slice - min_time_slice) / time_slice_step;
    
    time_slice_str.str("");
    time_slice_str << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;

    std::string foldername = "SiR2_DecayElectron_" + time_slice_str.str();
    std::string histname = foldername + "/hInputSpectrum";
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

    int rebin_factor = 5;
    raw_spectrum->Rebin(rebin_factor);
    correction->Rebin(rebin_factor);
    corrected_spectrum->Rebin(rebin_factor);

    raw_spectrum->SetTitle("Si16b Dataset, SiR2 Inclusive Analysis, Decay Electron Correction");
    raw_spectrum->SetStats(false);
    raw_spectrum->GetXaxis()->SetRangeUser(0,20000);
    raw_spectrum->SetLineColor(colours[i_slice]);
    
    std::stringstream axislabel;
    axislabel << "Counts / " << raw_spectrum->GetBinWidth(1) << " keV";
    raw_spectrum->SetYTitle(axislabel.str().c_str());

    correction->SetLineColor(kBlue);
    corrected_spectrum->SetLineColor(kRed);
    
    raw_spectrum->Draw("HIST E");
    correction->Draw("HIST E SAME");
    corrected_spectrum->Draw("HIST E SAME");

    leg->AddEntry(raw_spectrum, "Spectrum w/ Flat Bkg Correction", "l");
    leg->AddEntry(correction, "Decay Electron Correction", "l");
    leg->AddEntry(corrected_spectrum, "Corrected Spectrum", "l");
  }

  leg->Draw();

  //    std::string pngname = "~/plots/2018-11-26/AlCapData_Si16bDataset_" + time_slice_str.str() + ".png";
  //    c1->SaveAs(pngname.c_str());
}
