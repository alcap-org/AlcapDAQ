void SiL3_FinalPlot_RawSpectrum_TimeSlices() {

  std::string filename = "~/data/results/SiL3/unfold.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  double min_time_slice = 1000;
  double max_time_slice = 5000;//3000;//5000;
  double time_slice_step = 200;

  for (double i_min_time_slice = min_time_slice; i_min_time_slice < max_time_slice; i_min_time_slice += time_slice_step) {

    double i_max_time_slice = i_min_time_slice+time_slice_step;
    double i_time_slice_step = 100;
    const int n_slices = 2;
    Int_t colours[n_slices] = {kBlue, kRed};//, kBlack, kMagenta, kSpring, kGray, kGreen+2};

    std::stringstream time_slice_str;
    
    TCanvas* c1 = new TCanvas("c1", "c1");
    c1->SetLogy();

    TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.03);
    leg->SetFillStyle(0);
    leg->SetFillColor(kWhite);

    for (double j_min_time_slice = i_min_time_slice; j_min_time_slice < i_max_time_slice; j_min_time_slice += i_time_slice_step) {
      double j_max_time_slice = j_min_time_slice + i_time_slice_step;
      
      time_slice_str.str("");
      time_slice_str << "TimeSlice" << j_min_time_slice << "_" << j_max_time_slice;
      
      int j_slice = (j_min_time_slice - i_min_time_slice) / i_time_slice_step;
      Int_t j_colour = colours[j_slice];

      std::string j_foldername = "TimeCut_" + time_slice_str.str();
      std::string j_histname = j_foldername + "/hInputSpectrum";

      TH1F* spectrum = (TH1F*) file->Get(j_histname.c_str());
      if (!spectrum) {
	std::cout << "Error: Problem getting spectrum " << j_histname << std::endl;
	return;
      }
      spectrum->SetStats(false);
      spectrum->SetMinimum(0.1);
      spectrum->SetMaximum(5e4);
      spectrum->GetXaxis()->SetRangeUser(0,10000);
      spectrum->SetLineColor(j_colour);
      spectrum->Draw("HIST E SAMES");

      leg->AddEntry(spectrum, time_slice_str.str().c_str(), "l");
    }
    leg->Draw();

    std::string pngname = "~/plots/2018-11-26/AlCapData_SiL3Dataset_" + time_slice_str.str() + ".png";
    c1->SaveAs(pngname.c_str());
  }
}
