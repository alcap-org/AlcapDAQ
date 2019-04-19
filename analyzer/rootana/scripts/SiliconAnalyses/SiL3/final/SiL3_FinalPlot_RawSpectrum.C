void SiL3_FinalPlot_RawSpectrum() {

  std::string filename = "~/data/results/SiL3/raw_spectra_geq2TgtPulse.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_slices = 1;
  double min_time_slice = 2000;
  double max_time_slice = 4000;//3000;//5000;
  double time_slice_step = 2000;
  Int_t colours[n_slices] = {kBlue};

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
  c1->SetGridx();
  c1->SetGridy();

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);

  std::stringstream time_slice_str, histtitle, axistitle;
  for (double i_min_time_slice = min_time_slice; i_min_time_slice < max_time_slice; i_min_time_slice += time_slice_step) {

    double i_max_time_slice = i_min_time_slice+time_slice_step;
    int i_slice = (i_min_time_slice - min_time_slice) / time_slice_step;
    
    time_slice_str.str("");
    time_slice_str << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;
      
    std::string foldername = "SiL3_ActiveTarget_" + time_slice_str.str();
    std::string histname = foldername + "/hRawSpectrum";

    TH1F* spectrum = (TH1F*) file->Get(histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << j_histname << std::endl;
      return;
    }
    spectrum->SetStats(false);
    histtitle.str("");
    histtitle << "SiL3 Dataset, Active Target Analysis, Energy Spectrum (" << i_min_time_slice << " ns < t < " << i_max_time_slice << " ns)";
    spectrum->SetTitle(histtitle.str().c_str());
    //    spectrum->Rebin(5);
    //    spectrum->SetMinimum(0.1);
    //    spectrum->SetMaximum(5e4);
    axistitle.str("");
    axistitle << "Count / " << spectrum->GetXaxis()->GetBinWidth(1) << " keV";
    spectrum->GetXaxis()->SetRangeUser(0,30000);
    spectrum->SetYTitle(axistitle.str().c_str());
    spectrum->SetLineColor(colours[i_slice]);
    spectrum->Draw("HIST E SAMES");

    leg->AddEntry(spectrum, time_slice_str.str().c_str(), "l");
  }

  //  leg->Draw();
}
