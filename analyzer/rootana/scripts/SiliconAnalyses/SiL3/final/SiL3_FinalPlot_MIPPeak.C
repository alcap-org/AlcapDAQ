void SiL3_FinalPlot_MIPPeak() {

  std::string filename = "~/data/results/SiL3/raw_spectra_geq1TgtPulse.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_slices = 1;
  double min_time_slice = 2000;//-200;
  double max_time_slice = 4000;//200;
  double time_slice_step = 2000;//400;
  Int_t colours[n_slices] = {kBlack};

  int n_mip_peaks = 2;
  double stopping_power = 3.876; // MeV / cm
  stopping_power *= 1000; // --> keV / cm
  stopping_power /= 1e4; // --> keV / um

  TCanvas* c1 = new TCanvas("c1", "c1");
  //  c1->SetLogy();
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
      std::cout << "Error: Problem getting spectrum " << histname << std::endl;
      return;
    }
    spectrum->SetStats(false);
    histtitle.str("");
    histtitle << "SiL3 Dataset, Active Target, Energy Spectrum (" << i_min_time_slice << " ns < t < " << i_max_time_slice << " ns)";
    spectrum->SetTitle(histtitle.str().c_str());
    //    spectrum->Rebin(5);
    //    spectrum->SetMinimum(0.1);
    //    spectrum->SetMaximum(5e4);
    axistitle.str("");
    axistitle << "Count / " << spectrum->GetXaxis()->GetBinWidth(1) << " keV";
    spectrum->GetXaxis()->SetRangeUser(0,2000);
    spectrum->SetYTitle(axistitle.str().c_str());
    spectrum->SetLineColor(colours[i_slice]);
    spectrum->Draw("HIST E SAMES");

    double y_text_shift = -0.15;
    double current_max_content = 9999999;
    for (int i_mip_peak = 0; i_mip_peak < n_mip_peaks; ++i_mip_peak) {

      current_max_content = spectrum->GetMaximum(current_max_content);
      int peak_bin = 0;
      spectrum->GetBinWithContent(current_max_content, peak_bin);
      double peak_pos = spectrum->GetBinCenter(peak_bin);

      std::stringstream funcname;
      funcname << "gaus_" << i_mip_peak;
      TF1* gaus = new TF1(funcname.str().c_str(), "[0]*TMath::Gaus(x, [1], [2])",  peak_pos-50, peak_pos+100);
      gaus->SetParameters(current_max_content, peak_pos, 10);
      spectrum->Fit(funcname.str().c_str(), "R+");
      spectrum->GetFunction(funcname.str().c_str())->Draw("LSAME");
      
      double mip_peak = spectrum->GetFunction(funcname.str().c_str())->GetParameter(1);
      double mip_peak_err = spectrum->GetFunction(funcname.str().c_str())->GetParError(1);
      double depth = mip_peak / stopping_power;
      
      TLatex* latex = new TLatex();
      latex->SetTextAlign(22);
      std::stringstream text;
      text << std::fixed << std::setprecision(1) << "MIP Peak = " << mip_peak << " #pm " << mip_peak_err << " keV";
      latex->DrawLatexNDC(0.6, 0.7+(i_mip_peak*y_text_shift), text.str().c_str());
      text.str("");
      text << std::setprecision(4) << "Stopping Power = " << stopping_power << " keV / #mum";
      latex->DrawLatexNDC(0.6, 0.65+(i_mip_peak*y_text_shift), text.str().c_str());
      text.str("");
      text << std::setprecision(0) << "#rightarrow depth = " << depth << " #mum";
      latex->DrawLatexNDC(0.6, 0.6+(i_mip_peak*y_text_shift), text.str().c_str());
    }
    
    leg->AddEntry(spectrum, time_slice_str.str().c_str(), "l");
  }

  //  leg->Draw();
}
