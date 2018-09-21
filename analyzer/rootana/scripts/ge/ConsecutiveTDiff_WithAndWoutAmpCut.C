void ConsecutiveTDiff_WithAndWoutAmpCut() {

  const int n_runs = 2;
  int run_numbers[n_runs] = {9685, 10319};//, 9040, 9740, 10404};
  Int_t colours[n_runs] = {kRed, kBlue};//, kGray+1, kMagenta, kCyan};
  bool absolute_eff_run[n_runs] = {true, true};//, false, false, false};

  std::stringstream basename, histtitle, leglabel;
  std::string foldername, histname;
  std::string basepdfname = "~/plots/2018-08-13/";
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    TCanvas* c = new TCanvas();
    c->SetLogy();

    TLegend* leg = new TLegend(0.60,0.55,0.80,0.85);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.03);
    leg->SetFillColor(kWhite);


    basename.str("");
    basename << "out" << std::setw(5) << std::setfill('0') << run_numbers[i_run] << "_ge_calib";

    std::string filename = "~/data/out/local/" + basename.str() + ".root";
    TFile* file = new TFile(filename.c_str(), "READ");

    foldername = "PlotTAP_ConsecutiveTDiff"; // just happens to be the same
    histname = foldername + "/hGeLoGain#MaxBinAPGenerator#{no_time_shift=true}_ConsecutiveTDiff";
    TH1F* hWoutAmpCut = (TH1F*) (file->Get(histname.c_str())->Clone(basename.str().c_str()));
    
    hWoutAmpCut->SetDirectory(0);
    histtitle.str("");
    histtitle << "Time Between Consecutive Pulses in GeLoGain (Run " << run_numbers[i_run] << ")";
    hWoutAmpCut->SetTitle(histtitle.str().c_str());
    hWoutAmpCut->SetStats(false);
    hWoutAmpCut->SetLineColor(colours[i_run]);
    hWoutAmpCut->Sumw2();
    //    hWoutAmpCut->SetLineWidth(2);
    hWoutAmpCut->Rebin(20);
    //    hWoutAmpCut->Scale(1.0 / hWoutAmpCut->Integral());
    //    hWoutAmpCut->GetXaxis()->SetRangeUser(0, 25e6);
    //    hWoutAmpCut->GetYaxis()->SetRangeUser(1e-6, 10);

    histtitle.str("");
    histtitle << "Fractional difference with and without amplitude cut (Run " << run_numbers[i_run] << ")";
    
    TF1* expo_fit = new TF1("expo_fit", "TMath::Exp(-[0]-[1]*x)", 20e3, 100e6);
    expo_fit->SetParameters(5, 1e-6);
    hWoutAmpCut->Fit(expo_fit, "RME0");
    expo_fit->SetLineColor(colours[i_run]);
    
    hWoutAmpCut->Draw("HIST E");
    expo_fit->Draw("LSAME");

    leglabel.str("");
    leglabel << "w/o Amp Cut, hit rate = " << std::fixed << std::setprecision(1) << expo_fit->GetParameter(1)*1e9 << " #pm " << expo_fit->GetParError(1)*1e9 << " s^{-1}";
    leg->AddEntry(hWoutAmpCut, leglabel.str().c_str(), "lp");   


    foldername = "PlotTAP_ConsecutiveTDiff"; // just happens to be the same
    histname = foldername + "/hGeLoGain#MaxBinAPGenerator#{no_time_shift=true}_ConsecutiveTDiffWAmpCut";
    TH1F* hWAmpCut = (TH1F*) (file->Get(histname.c_str())->Clone(basename.str().c_str()));
    
    hWAmpCut->SetDirectory(0);
    hWAmpCut->SetTitle("Time Between Consecutive Pulses in GeLoGain (with amp cut)");
    hWAmpCut->SetStats(false);
    hWAmpCut->SetLineColor(colours[i_run]);
    //    hWAmpCut->SetLineStyle(kDashed);
    hWAmpCut->Sumw2();
    //    hWAmpCut->SetLineWidth(2);
    hWAmpCut->Rebin(20);
    //    hWAmpCut->Scale(1.0 / hWAmpCut->Integral());
    //    hWAmpCut->GetXaxis()->SetRangeUser(0, 25e6);
    //    hWAmpCut->GetYaxis()->SetRangeUser(1e-6, 10);

    TH1F* hDiff = (TH1F*) hWAmpCut->Clone("hDiff");
    hDiff->SetTitle(histtitle.str().c_str());
    hDiff->SetDirectory(0);
    hDiff->Sumw2();
    hDiff->SetYTitle("With Amp Cut / Without Amp Cut");
    //    hDiff->Add(hWAmpCut, -1);
    hDiff->Divide(hWoutAmpCut);

    TF1* expo_fitWAmpCut = new TF1("expo_fitWAmpCut", "TMath::Exp(-[0]-[1]*x)", 20e3, 100e6);
    expo_fitWAmpCut->SetParameters(5, 1e-6);
    hWAmpCut->Fit(expo_fitWAmpCut, "RME0");
    expo_fitWAmpCut->SetLineColor(colours[i_run]);
    //    expo_fitWAmpCut->SetLineStyle(kDashed);

    hWAmpCut->Draw("HIST E SAME");
    expo_fitWAmpCut->Draw("LSAME");

    leglabel.str("");
    leglabel << "w/ Amp Cut, hit rate = " << std::fixed << std::setprecision(1) << expo_fitWAmpCut->GetParameter(1)*1e9 << " #pm " << expo_fitWAmpCut->GetParError(1)*1e9 << " s^{-1}";
    leg->AddEntry(hWAmpCut, leglabel.str().c_str(), "lp");   

    hDiff->Draw("HIST E");
    
    file->Close();

    //    leg->Draw();
  }
}
