void SiL3_FinalPlot_RawSpectrum(std::string SiL3_tag = "geq2TgtPulse_newPP20us_1", std::string savedir = "", std::ostream& numbers_file = std::cout) {

  std::string filename = "~/data/results/SiL3/raw_spectra_" + SiL3_tag + ".root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_slices = 1;
  double min_time_slice = 3000;
  double max_time_slice = 4000;//3000;//5000;
  double time_slice_step = 1000;
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

  numbers_file << "% from SiL3_FinalPlot_RawSpectrum.C" << std::endl;
  
  std::stringstream time_slice_str, histtitle, axistitle;
  for (double i_min_time_slice = min_time_slice; i_min_time_slice < max_time_slice; i_min_time_slice += time_slice_step) {

    double i_max_time_slice = i_min_time_slice+time_slice_step;
    int i_slice = (i_min_time_slice - min_time_slice) / time_slice_step;
    
    time_slice_str.str("");
    time_slice_str << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;
      
    std::string foldername = "SiL3_ActiveTarget_" + time_slice_str.str() + "_allRecoil";
    std::string histname = foldername + "/hRawSpectrum";

    TH1F* spectrum = (TH1F*) file->Get(histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << histname << std::endl;
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
    axistitle << "Count / " << spectrum->GetXaxis()->GetBinWidth(1) << " MeV";
    spectrum->GetXaxis()->SetRangeUser(0,30);
    spectrum->GetXaxis()->SetTitleOffset(0.9);
    spectrum->GetYaxis()->SetTitleOffset(0.9);
    spectrum->SetXTitle("Energy [MeV]");
    spectrum->SetYTitle(axistitle.str().c_str());
    spectrum->SetLineColor(colours[i_slice]);
    spectrum->Draw("HIST E SAMES");

    float n_total_count = spectrum->GetEntries();
    float n_total_count_error = std::sqrt(n_total_count);
    numbers_file << std::fixed << "\\newcommand\\SiLNTotalCount{\\num{" << n_total_count << "\\pm" << n_total_count_error << "}}" << std::endl;
    numbers_file << std::fixed << "\\newcommand\\SiLNTotalCountTab{\\num[round-precision=4,round-mode=figures]{" << n_total_count/1e3 << "}(\\num[round-precision=1,round-mode=figures]{";
    if (n_total_count_error/1e3 > 1) {
      numbers_file << n_total_count_error/1e3 << "})}" << std::endl;
    }
    else {
      numbers_file << n_total_count_error/1e3 * 10<< "})}" << std::endl;
    }
    
    alcaphistogram(spectrum);
    alcapPreliminary(spectrum);
    spectrum->SetLineWidth(1);
    spectrum->SetDrawOption("HIST E");
    
    leg->AddEntry(spectrum, time_slice_str.str().c_str(), "l");
  }

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_SiL3Dataset_ActiveTarget_RawSpectrum";
    
    std::string pdfname = savename + ".pdf";
    c1->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c1->SaveAs(pngname.c_str());
  }
  numbers_file << std::endl;
  //  leg->Draw();
}
