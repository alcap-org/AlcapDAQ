void SiL3_UnfoldCrossCheck() {

  std::string filename = "~/data/results/SiL3/unfold.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  double min_time_slice = 1000;
  double max_time_slice = 1700;//3000;//5000;
  double time_slice_step = 100;
  const int n_slices = 7;
  Int_t colours[n_slices] = {kBlue, kRed, kBlack, kMagenta, kSpring, kGray, kGreen+2};

  std::stringstream time_slice_str;

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);

  for (double i_min_time_slice = min_time_slice; i_min_time_slice < max_time_slice; i_min_time_slice += time_slice_step) {
    double i_max_time_slice = i_min_time_slice + time_slice_step;

    time_slice_str.str("");
    time_slice_str << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;

    int i_slice = (i_min_time_slice - min_time_slice) / time_slice_step;
    Int_t i_colour = colours[i_slice];

    std::string i_foldername = "TimeCut_" + time_slice_str.str();
    //    std::string i_histname = i_foldername + "/hCorrectedSpectrum";
    //    std::string i_foldername = "DecayElectronCorrection_" + time_slice_str.str();
    //    std::string i_histname = i_foldername + "/hCorrectedSpectrum";
    std::string i_histname = i_foldername + "/hInputSpectrum";

    TH1F* spectrum = (TH1F*) file->Get(i_histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << i_histname << std::endl;
      return;
    }
    //    spectrum->Sumw2();
    //    spectrum->Rebin(2);
    spectrum->GetXaxis()->SetRangeUser(0,10000);
    spectrum->SetLineColor(i_colour);
    spectrum->Draw("HIST E SAMES");

    /*    i_histname = i_foldername + "/hCorrection";
    TH1F* correction = (TH1F*) file->Get(i_histname.c_str());
    if (!correction) {
      std::cout << "Error: Problem getting correction " << i_spectraname << std::endl;
      return;
    }
    correction->SetLineColor(i_colour);
    correction->Draw("HIST E SAMES");
    */

    leg->AddEntry(spectrum, time_slice_str.str().c_str(), "l");
  }

  TLine* peak_line = new TLine(2500, spectrum->GetMinimum(), 2500, spectrum->GetMaximum());
  peak_line->SetLineColor(kBlack);
  peak_line->Draw("LSAME");
  
  TLine* trough_line = new TLine(1400, spectrum->GetMinimum(), 1400, spectrum->GetMaximum());
  trough_line->SetLineColor(kBlack);
  trough_line->Draw("LSAME");

  leg->Draw();
}
