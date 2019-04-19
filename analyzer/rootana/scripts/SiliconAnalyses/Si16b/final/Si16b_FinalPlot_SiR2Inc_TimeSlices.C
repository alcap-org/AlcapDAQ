void Si16b_FinalPlot_SiR2Inc_TimeSlices() {

  std::string filename = "~/data/results/Si16b/unfold.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_slices = 2;
  double min_time_slices[n_slices] = {1000, 2000};//, 1000};
  double max_time_slices[n_slices] = {2000, 5000};//, 4000};
  Int_t colours[n_slices] = {kBlue, kRed};//, kBlack};//, kMagenta};//, kSpring};//, kGray, kGreen+2};

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
  for (int i_slice = 0; i_slice < n_slices; ++i_slice) {
    double i_min_time_slice = min_time_slices[i_slice];
    double i_max_time_slice = max_time_slices[i_slice];
    
    time_slice_str.str("");
    time_slice_str << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;
      
    std::string foldername = "SiR2_TimeCut_" + time_slice_str.str();
    //    std::string foldername = "DecayElectronCorrection_" + time_slice_str.str();
    std::string histname = foldername + "/hCorrectedSpectrum";

    TH1F* spectrum = (TH1F*) file->Get(histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << histname << std::endl;
      return;
    }
    spectrum->SetStats(false);

    histtitle.str("");
    histtitle << "Si16b Dataset, SiR2 Inclusive Analysis, Energy Spectrum (time slices)";
    spectrum->SetTitle(histtitle.str().c_str());
    spectrum->Rebin(2);
    //    spectrum->SetMinimum(0.1);
    //    spectrum->SetMaximum(5e4);
    axistitle.str("");
    axistitle << "Count / " << spectrum->GetXaxis()->GetBinWidth(1) << " keV";
    spectrum->GetXaxis()->SetRangeUser(0,20000);
    spectrum->SetYTitle(axistitle.str().c_str());
    spectrum->SetLineColor(colours[i_slice]);
    spectrum->Draw("HIST E SAMES");

    leglabel.str("");
    leglabel << i_min_time_slice << " ns < t < " << i_max_time_slice << " ns";
    leg->AddEntry(spectrum, leglabel.str().c_str(), "l");
  }

  leg->Draw();

  //    std::string pngname = "~/plots/2018-11-26/AlCapData_Si16bDataset_" + time_slice_str.str() + ".png";
  //    c1->SaveAs(pngname.c_str());
}
