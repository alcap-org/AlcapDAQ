void SiL3_FinalPlot_RawSpectrum_TimeSlices() {

  std::string filename = "~/data/results/SiL3/unfold_geq2TgtPulse_newPP20us_1.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_slices = 5;
  //  double min_time_slice = 2000;
  //  double max_time_slice = 4000;//3000;//5000;
  //  double time_slice_step = 500;
  double min_time_slices[n_slices] = {2000, 2000, 2500, 3000, 3500};
  double max_time_slices[n_slices] = {4000, 2500, 3000, 3500, 4000};
  Int_t colours[n_slices] = {kBlack, kBlue, kRed, kCyan, kMagenta};//, kSpring, kGray, kGreen+2};

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
  c1->SetGridx();
  c1->SetGridy();

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);

  std::stringstream time_slice_str, histtitle, axistitle, leglabel;
  //  for (double i_min_time_slice = min_time_slice; i_min_time_slice < max_time_slice; i_min_time_slice += time_slice_step) {

  //    double i_max_time_slice = i_min_time_slice+time_slice_step;
  //    int i_slice = (i_min_time_slice - min_time_slice) / time_slice_step;
  for (int i_slice = 0; i_slice < n_slices; ++i_slice) {
    double i_min_time_slice = min_time_slices[i_slice];
    double i_max_time_slice = max_time_slices[i_slice];
	
    time_slice_str.str("");
    time_slice_str << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;
      
    //    std::string foldername = "TimeCut_" + time_slice_str.str() + "_allRecoil";
    //    std::string foldername = "FalseTimeCut_" + time_slice_str.str();
    //    std::string foldername = "DecayElectron_" + time_slice_str.str() + "_allRecoil";
    //    std::string foldername = "CombinedEscape_" + time_slice_str.str() + "_allRecoil";
    //std::string histname = foldername + "/hCorrectedSpectrum";
    //    std::string histname = foldername + "/hInputSpectrum";
    std::string foldername = "FinalNormalisation_" + time_slice_str.str() + "_allRecoil_100keVBins";
    std::string histname = foldername + "/hNormalisedSpectrum";
    
    TH1F* spectrum = (TH1F*) file->Get(histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << histname << std::endl;
      return;
    }
    spectrum->SetStats(false);
    
    histtitle.str("");
    histtitle << "SiL3 Dataset, Active Target Analysis, After Time Cut Correction";
    spectrum->SetTitle(histtitle.str().c_str());
    spectrum->Rebin(5);
    //    spectrum->SetMinimum(0.1);
    //    spectrum->SetMaximum(5e4);
    axistitle.str("");
    //    axistitle << "Count / " << spectrum->GetXaxis()->GetBinWidth(1) << " MeV";
    spectrum->Scale(1.0/5);
    axistitle << "Count / muon capture / MeV";
    spectrum->GetXaxis()->SetRangeUser(0,17);
    //    c1->SetLogy(0);
    //    spectrum->GetXaxis()->SetRangeUser(0,20000);
    //    spectrum->GetYaxis()->SetRangeUser(0,200e3);
    spectrum->SetXTitle("Energy [MeV]");
    spectrum->SetYTitle(axistitle.str().c_str());
    spectrum->SetLineColor(colours[i_slice]);
    spectrum->Draw("HIST E SAMES");

    leglabel.str("");
    leglabel << i_min_time_slice << " ns < t < " << i_max_time_slice << " ns";
    leg->AddEntry(spectrum, leglabel.str().c_str(), "l");
  }

  leg->Draw();

  //    std::string pngname = "~/plots/2018-11-26/AlCapData_SiL3Dataset_" + time_slice_str.str() + ".png";
  //    c1->SaveAs(pngname.c_str());
}
