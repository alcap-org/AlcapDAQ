void SiL3_FinalPlot_FlatBkgCorrection() {

  std::string filename = "~/data/results/SiL3/unfold_geq2TgtPulse.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_slices = 1;
  double min_time_slice = 2000;
  double max_time_slice = 4000;
  double time_slice_step = 2000;
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

    std::string foldername = "FlatBkg_" + time_slice_str.str();
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

    raw_spectrum->SetTitle("SiL3 Dataset, Active Target Analysis, Flat Background Correction");
    raw_spectrum->SetStats(false);
    raw_spectrum->GetXaxis()->SetRangeUser(0,30000);
    raw_spectrum->SetLineColor(colours[i_slice]);

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
  }

  leg->Draw();

  //    std::string pngname = "~/plots/2018-11-26/AlCapData_SiL3Dataset_" + time_slice_str.str() + ".png";
  //    c1->SaveAs(pngname.c_str());
}
