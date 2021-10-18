void SiL3_FinalPlot_EscapeCorrection(std::string savedir = "") {

  std::string filename = "~/data/results/SiL3/unfold_geq2TgtPulse_newPP20us_1.root";
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

    //    std::string foldername = "ProtonEscape_" + time_slice_str.str();
    std::string foldername = "CombinedEscape_" + time_slice_str.str() + "_allRecoil";

    std::string histname = foldername + "/hInputSpectrum";
    TH1F* raw_spectrum = (TH1F*) file->Get(histname.c_str());
    if (!raw_spectrum) {
      std::cout << "Error: Problem getting raw_spectrum " << histname << std::endl;
      return;
    }
    histname = foldername + "/hCorrectedSpectrum";
    TH1F* corrected_spectrum = (TH1F*) file->Get(histname.c_str());
    if (!corrected_spectrum) {
      std::cout << "Error: Problem getting corrected_spectrum " << histname << std::endl;
      return;
    }

    int rebin_factor = 1;
    raw_spectrum->Rebin(rebin_factor);
    corrected_spectrum->Rebin(rebin_factor);

    raw_spectrum->SetTitle("SiL3 Dataset, Active Target Analysis, Combined Escape Correction");
    raw_spectrum->SetStats(false);
    raw_spectrum->GetXaxis()->SetRangeUser(0,30);
    //    raw_spectrum->GetYaxis()->SetRangeUser(1e-3, 1e7);
    raw_spectrum->SetLineColor(colours[i_slice]);
    raw_spectrum->GetXaxis()->SetTitleOffset(0.9);
    raw_spectrum->GetYaxis()->SetTitleOffset(0.9);

    std::stringstream axislabel;
    axislabel << "Counts / " << raw_spectrum->GetBinWidth(1) << " MeV";
    raw_spectrum->SetYTitle(axislabel.str().c_str());

    corrected_spectrum->SetLineColor(kRed);

    alcaphistogram(raw_spectrum);
    alcaphistogram(corrected_spectrum);

    raw_spectrum->Draw("HIST E");
    corrected_spectrum->Draw("HIST E SAME");

    alcapPreliminary(raw_spectrum);
    
    leg->AddEntry(raw_spectrum, "Spectrum (w/ decay electron correction)", "l");
    leg->AddEntry(corrected_spectrum, "Unfolded Spectrum", "l");
  }

  leg->Draw();

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_SiL3Dataset_ActiveTarget_EscapeCorrection";
    
    std::string pdfname = savename + ".pdf";
    c1->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c1->SaveAs(pngname.c_str());
  }
}
