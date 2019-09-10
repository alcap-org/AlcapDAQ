void SiL3_FinalPlot_MuScInefficiency() {

  int run_number = 9040;
  std::stringstream filename;
  filename.str("");
  filename << "~/data/out/local/out0" << run_number << "_consecutiveTDiff_noTemplates.root";
  TFile* file = new TFile(filename.str().c_str(), "READ");
  
  TCanvas* c_tdiff = new TCanvas("c_tdiff", "c_tdiff");
  c_tdiff->SetLogy();
  //  std::string tdiff_histname = "PlotTAP_ConsecutiveTDiff/hmuSc#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= false}_ConsecutiveTDiffZoom";
  std::string tdiff_histname = "PlotTAP_ConsecutiveTDiff/hmuSc#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= false}_ConsecutiveTDiffZoomWAmpCut";
  TH1F* hConsecutiveTDiff = file->Get(tdiff_histname.c_str());
  hConsecutiveTDiff->SetTitle("Time Between Consecutive #muSc Pulses, Run 9040 (SiL3 Dataset)");
  hConsecutiveTDiff->SetStats(false);
  //  hConsecutiveTDiff->Rebin(2);
  //  hConsecutiveTDiff->GetXaxis()->SetRangeUser(0,1000);
  std::stringstream axislabel;
  axislabel.str("");
  axislabel << "Count / " << hConsecutiveTDiff->GetXaxis()->GetBinWidth(1) << " ns";
  hConsecutiveTDiff->SetYTitle(axislabel.str().c_str());
  hConsecutiveTDiff->SetXTitle("t_{i+1} - t_{i} [ns]");
  hConsecutiveTDiff->Draw();

  hConsecutiveTDiff->Fit("expo", "R", "", 20e3, 100e3);

  std::stringstream text;
  TLatex* latex = new TLatex();
  latex->SetTextAlign(22);

  double t_block = 96e-3; // s
  double n_block = 2704;
  double rate = hConsecutiveTDiff->GetFunction("expo")->GetParameter(1)*-1*1e9; // Hz
  double expected_N_mu = rate * n_block * t_block;
  text.str("");
  text << "R_{#mu} = " << std::fixed << std::setprecision(1) << rate*1e-3 << " kHz";
  latex->DrawLatexNDC(0.65, 0.8, text.str().c_str());

  text.str("");
  text << "N_{block} = " << std::fixed << std::setprecision(0) << n_block;
  latex->DrawLatexNDC(0.65, 0.75, text.str().c_str());

  text.str("");
  text << "t_{block} = " << std::fixed << std::setprecision(0) << t_block*1e3 << " ms";
  latex->DrawLatexNDC(0.65, 0.7, text.str().c_str());

  text.str("");
  text << "N_{#mu} (expected) = " << std::fixed << std::setprecision(0) << expected_N_mu;
  latex->DrawLatexNDC(0.65, 0.65, text.str().c_str());

  TCanvas* c_amp = new TCanvas("c_amp", "c_amp");
  std::string amp_histname = "PlotTAP_Amplitude/hmuSc#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= false}_Amplitude";
  TH1F* hAmp = file->Get(amp_histname.c_str());
  hAmp->SetTitle("Pulse Amplitude Spectrum of #muSc Pulses, Run 9040 (SiL3 Dataset)");
  hAmp->SetStats(false);
  hAmp->SetXTitle("Pulse Amplitude [ADC]");

  double amp_cut = 1500;
  int min_integral_bin = hAmp->GetXaxis()->FindBin(amp_cut);
  int max_integral_bin = hAmp->GetNbinsX();
  double total_integral = hAmp->Integral(min_integral_bin, max_integral_bin);

  hAmp->Rebin(16);
  axislabel.str("");
  axislabel << "Count / " << hAmp->GetXaxis()->GetBinWidth(1) << " ADC";
  hAmp->SetYTitle(axislabel.str().c_str());
  hAmp->Draw();

  TLine* amp_cut_line = new TLine(amp_cut, 0, amp_cut, hAmp->GetMaximum());
  amp_cut_line->SetLineWidth(2);
  amp_cut_line->Draw("LSAME");

  text.str("");
  text << "Integral (Amp > " << std::setprecision(0) << amp_cut << " ADC) = " << total_integral;
  latex->DrawLatexNDC(0.6, 0.6, text.str().c_str());

  c_tdiff->cd();
  text.str("");
  text << "N_{#mu} (actual) = " << std::fixed << std::setprecision(0) << total_integral;
  latex->DrawLatexNDC(0.65, 0.6, text.str().c_str());

  double efficiency = total_integral / expected_N_mu;
  text.str("");
  text << "Efficiency = " << std::fixed << std::setprecision(3) << efficiency;
  latex->DrawLatexNDC(0.65, 0.55, text.str().c_str());

}
