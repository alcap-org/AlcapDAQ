void Si16b_FinalPlot_RawSpectrum_TimeSlices() {

  std::string filename = "~/data/results/Si16b/unfold.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_slices = 3;
  double min_time_slice = 1000;
  double max_time_slice = 4000;
  double time_slice_step = 1000;
  Int_t colours[n_slices] = {kBlue, kRed, kBlack};//, kMagenta};//, kSpring, kGray, kGreen+2};

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();

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

    std::string foldername = "SiL3_TimeCut_" + time_slice_str.str();
    //    std::string foldername = "SiL3_FlatBkg_" + time_slice_str.str();
    //    std::string histname = foldername + "/hRawSpectrum";
    //    std::string histname = foldername + "/hCorrection";
    std::string histname = foldername + "/hCorrectedSpectrum";

    TH1F* spectrum = (TH1F*) file->Get(histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << histname << std::endl;
      return;
    }
    int i_bin = 5;
    std::cout << time_slice_str.str() << ": Bin #" << i_bin << " = " << spectrum->GetBinContent(i_bin) << " +/- " << spectrum->GetBinError(i_bin) << std::endl;
      
    spectrum->SetStats(false);
    //    spectrum->Rebin(5);
    //    spectrum->SetMinimum(0.1);
    //    spectrum->SetMaximum(5e4);
    spectrum->GetXaxis()->SetRangeUser(0,10000);
    spectrum->SetLineColor(colours[i_slice]);
    spectrum->Draw("HIST E SAMES");

    leg->AddEntry(spectrum, time_slice_str.str().c_str(), "l");
  }

  leg->Draw();

  //    std::string pngname = "~/plots/2018-11-26/AlCapData_Si16bDataset_" + time_slice_str.str() + ".png";
  //    c1->SaveAs(pngname.c_str());
}
