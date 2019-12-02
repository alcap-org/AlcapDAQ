void ConsecutiveTDiffComparisons() {

  const int n_runs = 1;
  double tblock = 0.098; //s
  int run_numbers[n_runs] = {10404};
  int nblock[n_runs] = {4548};
  Int_t colours[n_runs] = {kBlack};
  int tlive[n_runs] = {0};

  const int n_channels = 1;
  std::string channels[n_channels] = {"SiR1-2-S"};
  double thresholds[n_channels] = {300};
  double n_expected[n_channels] = {0};
  double n_actual[n_channels] = {0};

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
  //  std::string basepdfname = "~/plots/2018-08-13/";
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    tlive[i_run] = tblock * nblock[i_run];
    for (int i_channel = 0; i_channel < n_channels; ++i_channel) {
      std::string channel = channels[i_channel];
      
      basename.str("");
      basename << "out" << std::setw(5) << std::setfill('0') << run_numbers[i_run] << "_consecutiveTDiff";

      std::string filename = "~/data/out/local/" + basename.str();
      filename += ".root";
      TFile* file = new TFile(filename.c_str(), "READ");
      if (file->IsZombie()) {
	continue;
      }

      c_spectrum->cd();
      foldername = "PlotTAP_Amplitude"; // just happens to be the same
      histname = foldername + "/h" + channel + "#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= false}_Amplitude";
      //    histname = foldername + "/h" + ge_channel + "#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= true}_Amplitude";
      TH1F* hSpec = (TH1F*) (file->Get(histname.c_str()));
      if (!hSpec) {
	std::cout << "Couldn't get hSpec" << std::endl;
	continue;
      }
      hSpec = (TH1F*) hSpec->Clone(basename.str().c_str());
	

      std::cout << "Actual Count (no amp cut) = " << hSpec->GetEntries() << std::endl;
      int low_integral_bin = hSpec->GetXaxis()->FindBin(thresholds[i_run]);
      int high_integral_bin = hSpec->GetNbinsX()-1;
      double integralWAmpCut = hSpec->Integral(low_integral_bin, high_integral_bin);
      std::cout << "Actual Count (w amp cut) = " << integralWAmpCut << std::endl;
      n_actual[i_channel] = integralWAmpCut;

      hSpec->Rebin(16);
      hSpec->Draw();
      hSpec->SetDirectory(0);
      pdfname.str("");
      //    pdfname << basepdfname << "h" + ge_channel + "_Amplitude_Run" << run_numbers[i_run] << ".pdf";
      //    c_spectrum->SaveAs(pdfname.str().c_str());

      TLine* threshold_line = new TLine(thresholds[i_run], 0, thresholds[i_run], hSpec->GetMaximum());
      threshold_line->SetLineColor(kRed);
      threshold_line->Draw("LSAME");
      pdfname.str("");
      //    pdfname << basepdfname << "h" + ge_channel + "_Amplitude_Run" << run_numbers[i_run] << "_wThresholdLine.pdf";
      //    c_spectrum->SaveAs(pdfname.str().c_str());

      
      c_consecTDiff->cd();
      foldername = "PlotTAP_ConsecutiveTDiff"; // just happens to be the same
      histname = foldername + "/h" + channel + "#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= false}_ConsecutiveTDiff";
      //    histname = foldername + "/h" + ge_channel + "#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= true}_ConsecutiveTDiff";

      int tdiff_rebin = 5;
      TH1F* hTDiff = (TH1F*) (file->Get(histname.c_str()));
      if (!hTDiff) {
	std::cout << "Couldn't get hTDiff" << std::endl;
	continue;
      }
      hTDiff = (TH1F*) hTDiff->Clone(basename.str().c_str());
      
      hTDiff->SetDirectory(0);
      std::string histtitle = "Time Between Consecutive Pulses in " + channel + "";
      hTDiff->SetTitle(histtitle.c_str());
      hTDiff->SetYTitle("Normalised to Unit Area");
      hTDiff->SetStats(false);
      hTDiff->SetLineColor(colours[i_run]);
      hTDiff->Sumw2();
      //    hTDiff->SetLineWidth(2);
      hTDiff->Rebin(tdiff_rebin);
      hTDiff->Scale(1.0 / hTDiff->Integral());
      //    hTDiff->GetXaxis()->SetRangeUser(0, 25e6);
      hTDiff->GetYaxis()->SetRangeUser(1e-6, 10);
      
      TF1* expo_fit = new TF1("expo_fit", "TMath::Exp(-[0]-[1]*x)", 20e3, 100e6);
      expo_fit->SetParameters(5, 1e-6);
      hTDiff->Fit(expo_fit, "RME0");
      expo_fit->SetLineColor(colours[i_run]);

      hTDiff->Draw("HIST E SAME");
      expo_fit->Draw("LSAME");

      leglabel.str("");
      leglabel << "Run " << run_numbers[i_run] << " ";

      leglabel << ", Hit Rate = " << std::fixed << std::setprecision(1) << expo_fit->GetParameter(1)*1e9 << " #pm " << expo_fit->GetParError(1)*1e9 << " Hz";
      leg->AddEntry(hTDiff, leglabel.str().c_str(), "lp");


      c_consecTDiffWAmpCut->cd();
      foldername = "PlotTAP_ConsecutiveTDiff"; // just happens to be the same
      histname = foldername + "/h" + channel + "#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= false}_ConsecutiveTDiffWAmpCut";
      //    histname = foldername + "/h" + ge_channel + "#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= true}_ConsecutiveTDiffWAmpCut";
      TH1F* hTDiffAmpCut = (TH1F*) (file->Get(histname.c_str()));
      if (!hTDiffAmpCut) {
	std::cout << "Couldn't get hTDiffAmpCut" << std::endl;
	continue;
      }
      hTDiffAmpCut = (TH1F*) hTDiffAmpCut->Clone(basename.str().c_str());

    
      hTDiffAmpCut->SetDirectory(0);
      histtitle = "Time Between Consecutive Pulses in " + channel + " (with amp cut)";
      hTDiffAmpCut->SetTitle(histtitle.c_str());
      hTDiffAmpCut->SetYTitle("Normalised to Unit Area");
      hTDiffAmpCut->SetStats(false);
      hTDiffAmpCut->SetLineColor(colours[i_run]);
      hTDiffAmpCut->Sumw2();
      //    hTDiffAmpCut->SetLineWidth(2);
      hTDiffAmpCut->Rebin(tdiff_rebin);
      hTDiffAmpCut->Scale(1.0 / hTDiffAmpCut->Integral());
      //    hTDiffAmpCut->GetXaxis()->SetRangeUser(0, 25e6);
      hTDiffAmpCut->GetYaxis()->SetRangeUser(1e-6, 10);

      TF1* expo_fitWAmpCut = new TF1("expo_fitWAmpCut", "TMath::Exp(-[0]-[1]*x)", 20e3, 100e6);
      expo_fitWAmpCut->SetParameters(5, 1e-6);
      hTDiffAmpCut->Fit(expo_fitWAmpCut, "RME0");
      expo_fitWAmpCut->SetLineColor(colours[i_run]);
      
      hTDiffAmpCut->Draw("HIST E SAME");
      expo_fitWAmpCut->Draw("LSAME");
      
      leglabel.str("");
      leglabel << "Run " << run_numbers[i_run];

      leglabel << ", Hit Rate = " << std::fixed << std::setprecision(1) << expo_fitWAmpCut->GetParameter(1)*1e9 << " #pm " << expo_fitWAmpCut->GetParError(1)*1e9 << " Hz";
      legWAmpCut->AddEntry(hTDiffAmpCut, leglabel.str().c_str(), "lp");
      n_expected[i_channel] = expo_fitWAmpCut->GetParameter(1)*1e9 * tlive[i_run];

      std::cout << "Deadtime = actual / expected = " << n_actual[i_channel] << " / " << n_expected[i_channel] << " = " << n_actual[i_channel] / n_expected[i_channel] << std::endl;
    }
    file->Close();
  }

  c_consecTDiff->cd();
  leg->Draw();
  pdfname.str("");
  //  pdfname << basepdfname << "h" + ge_channel + "_ConsecTDiff_HitRates.pdf";
  //  c_consecTDiff->SaveAs(pdfname.str().c_str());

  c_consecTDiffWAmpCut->cd();
  legWAmpCut->Draw();
  pdfname.str("");
  //  pdfname << basepdfname << "h" + ge_channel + "_ConsecTDiffWAmpCut_HitRates.pdf";
  //  c_consecTDiffWAmpCut->SaveAs(pdfname.str().c_str());
}
