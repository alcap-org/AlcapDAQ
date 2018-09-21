void ConsecutiveTDiffComparisons() {

  const int n_runs = 3;
  int run_numbers[n_runs] = {9302, 9685, 10319};//, 9040, 9740, 10404};
  Int_t colours[n_runs] = {kBlack, kRed, kBlue};//, kGray+1, kMagenta, kCyan};
  bool absolute_eff_run[n_runs] = {true, true, true};//, false, false, false};
  //  double thresholds[n_runs] = {455, 455, 90}; // GeLoGain
  double thresholds[n_runs] = {980, 980, 180}; // GeHiGain
  std::string ge_channel = "GeHiGain";
  
  /*  const int n_runs = 1;
  int run_numbers[n_runs] = {9040};
  Int_t colours[n_runs] = {kBlack};
  bool absolute_eff_run[n_runs] = {false};
  double thresholds[n_runs] = {455};
  */
  TH1F* hDiffs[n_runs] = {0};

  TLegend* leg = new TLegend(0.30,0.55,0.70,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);

  TLegend* legWAmpCut = new TLegend(0.30,0.55,0.70,0.85);
  legWAmpCut->SetBorderSize(0);
  legWAmpCut->SetTextSize(0.03);
  legWAmpCut->SetFillColor(kWhite);

  TCanvas* c_consecTDiff = new TCanvas("c_consecTDiff", "c_consecTDiff");
  c_consecTDiff->SetLogy();

  TCanvas* c_consecTDiffWAmpCut = new TCanvas("c_consecTDiffWAmpCut", "c_consecTDiffWAmpCut");
  c_consecTDiffWAmpCut->SetLogy();

  TCanvas* c_spectrum = new TCanvas("c_spectrum", "c_spectrum");
  c_spectrum->SetLogy();

  TCanvas* c_diff = new TCanvas("c_diff", "c_diff");
  
  std::stringstream basename, leglabel, pdfname;
  std::string foldername, histname;
  TH1F* hist;
  std::string basepdfname = "~/plots/2018-08-13/";
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    basename.str("");
    basename << "out" << std::setw(5) << std::setfill('0') << run_numbers[i_run] << "_ge_calib";

    std::string filename = "~/data/out/local/" + basename.str();
    if (ge_channel == "GeHiGain") {
      filename += "_GeHiGain";
    }
    filename += ".root";
    TFile* file = new TFile(filename.c_str(), "READ");
    if (file->IsZombie()) {
      continue;
    }

    c_spectrum->cd();
    foldername = "PlotTAP_Amplitude"; // just happens to be the same
    histname = foldername + "/h" + ge_channel + "#MaxBinAPGenerator#{no_time_shift=true}_Amplitude";
    //    histname = foldername + "/h" + ge_channel + "#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= true}_Amplitude";
    hist = (TH1F*) (file->Get(histname.c_str()));
    if (!hist) {
      std::cout << "Couldn't get hist" << std::endl;
      continue;
    }
    hist = (TH1F*) hist->Clone(basename.str().c_str());
	

    std::cout << "Actual Count (no amp cut) = " << hist->GetEntries() << std::endl;
    int low_integral_bin = hist->GetXaxis()->FindBin(thresholds[i_run]);
    int high_integral_bin = hist->GetNbinsX()-1;
    double integralWAmpCut = hist->Integral(low_integral_bin, high_integral_bin);
    std::cout << "Actual Count (w amp cut) = " << integralWAmpCut << std::endl;

    hist->Rebin(16);
    hist->Draw();
    pdfname.str("");
    pdfname << basepdfname << "h" + ge_channel + "_Amplitude_Run" << run_numbers[i_run] << ".pdf";
    //    c_spectrum->SaveAs(pdfname.str().c_str());

    TLine* threshold_line = new TLine(thresholds[i_run], 0, thresholds[i_run], hist->GetMaximum());
    threshold_line->SetLineColor(kRed);
    threshold_line->Draw("LSAME");
    pdfname.str("");
    pdfname << basepdfname << "h" + ge_channel + "_Amplitude_Run" << run_numbers[i_run] << "_wThresholdLine.pdf";
    //    c_spectrum->SaveAs(pdfname.str().c_str());

    hist = (TH1F*) (file->Get(histname.c_str()));
    if (!hist) {
      std::cout << "Couldn't get hist" << std::endl;
      continue;
    }
    hist = (TH1F*) hist->Clone(basename.str().c_str());
    hist->GetXaxis()->SetRangeUser(0, 500);
    hist->SetStats(false);
    hist->Draw();
    pdfname.str("");
    pdfname << basepdfname << "h" + ge_channel + "_Amplitude_Run" << run_numbers[i_run] << "_ZoomIn.pdf";
    //    c_spectrum->SaveAs(pdfname.str().c_str());

    threshold_line->Draw("LSAME");
    pdfname.str("");
    pdfname << basepdfname << "h" + ge_channel + "_Amplitude_Run" << run_numbers[i_run] << "_ZoomIn_wThresholdLine.pdf";
    //    c_spectrum->SaveAs(pdfname.str().c_str());
    
    
    c_consecTDiff->cd();
    foldername = "PlotTAP_ConsecutiveTDiff"; // just happens to be the same
    histname = foldername + "/h" + ge_channel + "#MaxBinAPGenerator#{no_time_shift=true}_ConsecutiveTDiff";
    //    histname = foldername + "/h" + ge_channel + "#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= true}_ConsecutiveTDiff";

    hist = (TH1F*) (file->Get(histname.c_str()));
    if (!hist) {
      std::cout << "Couldn't get hist" << std::endl;
      continue;
    }
    hist = (TH1F*) hist->Clone(basename.str().c_str());
    
    hist->SetDirectory(0);
    std::string histtitle = "Time Between Consecutive Pulses in " + ge_channel + "";
    hist->SetTitle(histtitle.c_str());
    hist->SetYTitle("Normalised to Unit Area");
    hist->SetStats(false);
    hist->SetLineColor(colours[i_run]);
    hist->Sumw2();
    //    hist->SetLineWidth(2);
    hist->Rebin(5);
    hist->Scale(1.0 / hist->Integral());
    //    hist->GetXaxis()->SetRangeUser(0, 25e6);
    hist->GetYaxis()->SetRangeUser(1e-6, 10);

    if (hDiffs[i_run]==0) {
      hDiffs[i_run] = (TH1F*) hist->Clone("hDiff");
      hDiffs[i_run]->SetYTitle("Without Amplitude Cut - With Amplitude Cut");
      hDiffs[i_run]->SetDirectory(0);
    }

    TF1* expo_fit = new TF1("expo_fit", "TMath::Exp(-[0]-[1]*x)", 20e3, 100e6);
    expo_fit->SetParameters(5, 1e-6);
    hist->Fit(expo_fit, "RME0");
    expo_fit->SetLineColor(colours[i_run]);

    hist->Draw("HIST E SAME");
    expo_fit->Draw("LSAME");

    leglabel.str("");
    leglabel << "Run " << run_numbers[i_run] << " ";
    /*    if (absolute_eff_run[i_run]) {
      leglabel << "(Eu at target centre";
    }
    else {
      leglabel << "(Active target";
    }
    */
    leglabel << ", Hit Rate = " << std::fixed << std::setprecision(1) << expo_fit->GetParameter(1)*1e9 << " #pm " << expo_fit->GetParError(1)*1e9 << " Hz";
    leg->AddEntry(hist, leglabel.str().c_str(), "lp");


    c_consecTDiffWAmpCut->cd();
    foldername = "PlotTAP_ConsecutiveTDiff"; // just happens to be the same
    histname = foldername + "/h" + ge_channel + "#MaxBinAPGenerator#{no_time_shift=true}_ConsecutiveTDiffWAmpCut";
    //    histname = foldername + "/h" + ge_channel + "#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= true}_ConsecutiveTDiffWAmpCut";
    hist = (TH1F*) (file->Get(histname.c_str()));
    if (!hist) {
      std::cout << "Couldn't get hist" << std::endl;
      continue;
    }
    hist = (TH1F*) hist->Clone(basename.str().c_str());

    
    hist->SetDirectory(0);
    histtitle = "Time Between Consecutive Pulses in " + ge_channel + " (with amp cut)";
    hist->SetTitle(histtitle.c_str());
    hist->SetYTitle("Normalised to Unit Area");
    hist->SetStats(false);
    hist->SetLineColor(colours[i_run]);
    hist->Sumw2();
    //    hist->SetLineWidth(2);
    hist->Rebin(5);
    hist->Scale(1.0 / hist->Integral());
    //    hist->GetXaxis()->SetRangeUser(0, 25e6);
    hist->GetYaxis()->SetRangeUser(1e-6, 10);

    TF1* expo_fitWAmpCut = new TF1("expo_fitWAmpCut", "TMath::Exp(-[0]-[1]*x)", 20e3, 100e6);
    expo_fitWAmpCut->SetParameters(5, 1e-6);
    hist->Fit(expo_fitWAmpCut, "RME0");
    expo_fitWAmpCut->SetLineColor(colours[i_run]);

    hist->Draw("HIST E SAME");
    expo_fitWAmpCut->Draw("LSAME");

    leglabel.str("");
    leglabel << "Run " << run_numbers[i_run];
    /*<< " (Eu at ";
    if (absolute_eff_run[i_run]) {
      leglabel << "target centre";
    }
    else {
      leglabel << "Ge face";
    }
    */
    leglabel << ", Hit Rate = " << std::fixed << std::setprecision(1) << expo_fitWAmpCut->GetParameter(1)*1e9 << " #pm " << expo_fitWAmpCut->GetParError(1)*1e9 << " Hz";
    legWAmpCut->AddEntry(hist, leglabel.str().c_str(), "lp");

    if (hDiffs[i_run]!=0) {

      if (hDiffs[i_run]->GetEntries() == hist->GetEntries()) {
	continue; // because there's no point
      }
	  
      c_diff->cd();
      hDiffs[i_run]->Add(hist, -1);

      if (i_run==1) {
	hDiffs[i_run]->Draw("HIST");
      }
      else {
	hDiffs[i_run]->Draw("HIST SAME");
      }
      c_diff->Update();
    }
    
    file->Close();
  }

  c_consecTDiff->cd();
  leg->Draw();
  pdfname.str("");
  pdfname << basepdfname << "h" + ge_channel + "_ConsecTDiff_HitRates.pdf";
  //  c_consecTDiff->SaveAs(pdfname.str().c_str());

  c_consecTDiffWAmpCut->cd();
  legWAmpCut->Draw();
  pdfname.str("");
  pdfname << basepdfname << "h" + ge_channel + "_ConsecTDiffWAmpCut_HitRates.pdf";
  //  c_consecTDiffWAmpCut->SaveAs(pdfname.str().c_str());
}
