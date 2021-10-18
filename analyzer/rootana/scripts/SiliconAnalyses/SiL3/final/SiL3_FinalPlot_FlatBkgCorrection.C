void SiL3_FinalPlot_FlatBkgCorrection(std::string savedir = "", std::ostream& numbers_file = std::cout) {

  numbers_file << "% SiL3_FinalPlot_FlatBkgCorrection.C" << std::endl;
  
  std::string filename = "~/data/results/SiL3/unfold_geq2TgtPulse_newPP20us_1_test_KFactor0-85.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_slices = 1;
  double min_time_slice = 3000;
  double max_time_slice = 4000;
  double time_slice_step = 1000;
  Int_t colours[n_slices] = {kBlack};

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
  c1->SetGridx();
  c1->SetGridy();

  TLegend* leg = new TLegend(0.45,0.55,0.85,0.85);
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

    std::string foldername = "FlatBkg_" + time_slice_str.str() + "_allRecoil";
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
    raw_spectrum->GetXaxis()->SetRangeUser(0,30);
    raw_spectrum->GetXaxis()->SetTitleOffset(0.9);
    raw_spectrum->GetYaxis()->SetTitleOffset(0.9);
    raw_spectrum->SetLineColor(colours[i_slice]);

    std::stringstream axislabel;
    axislabel << "Counts / " << raw_spectrum->GetBinWidth(1) << " MeV";
    raw_spectrum->SetYTitle(axislabel.str().c_str());
    raw_spectrum->SetXTitle("Energy [MeV]");
    
    correction->SetLineColor(kBlue);
    corrected_spectrum->SetLineColor(kRed);

    alcaphistogram(raw_spectrum);
    alcaphistogram(correction);
    alcaphistogram(corrected_spectrum);
    
    raw_spectrum->Draw("HIST E");
    correction->Draw("HIST E SAME");
    corrected_spectrum->Draw("HIST E SAME");

    alcapPreliminary(raw_spectrum);

    leg->AddEntry(raw_spectrum, "Raw Spectrum (w/ recoil)", "l");
    leg->AddEntry(correction, "Flat Bkg Correction (w/ no recoil)", "l");
    leg->AddEntry(corrected_spectrum, "Corrected Spectrum", "l");

    numbers_file << "\\newcommand\\SiLFlatBkgSlice{\\SI{-4000}{\\nano\\second} $<t<$ \\SI{-2000}{\\nano\\second}}" << std::endl;
  }

  leg->Draw();

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_SiL3Dataset_ActiveTarget_FlatBkgCorrection";
    
    std::string pdfname = savename + ".pdf";
    c1->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c1->SaveAs(pngname.c_str());
  }
  numbers_file << std::endl;
}
