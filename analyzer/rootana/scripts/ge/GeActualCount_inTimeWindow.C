void GeActualCount_inTimeWindow() {

  const int n_runs = 4;
  int run_numbers[n_runs] = {9302, 9685, 10319, 9040};//, 9740, 10404};
  Int_t colours[n_runs] = {kBlack, kRed, kBlue, kGray+1};//, kMagenta, kCyan};

  std::string ge_channel = "GeHiGain";
  //  double thresholds[n_runs] = {455, 455, 90, 0}; // for GeLoGain
  double thresholds[n_runs] = {980, 980, 180, 0}; // for GeHiGain

  double min_sync_pulse_cut = 10e3;
  double max_sync_pulse_cut = 96e6;
  
  std::stringstream canvasname, filename, histtitle;
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    int this_run = run_numbers[i_run];
    std::cout << "Run #" << this_run << std::endl;

    TLegend* leg = new TLegend(0.58,0.35,0.70,0.65);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.03);
    leg->SetFillColor(kWhite);
    
    
    canvasname.str("");
    canvasname << "c_" << this_run;
    TCanvas* c = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());
    c->SetLogy();

    filename.str("");
    filename << "~/data/out/local/out" << std::setw(5) << std::setfill('0') << this_run << "_ge_calib.root";
    TFile* file = new TFile(filename.str().c_str(), "READ");

    histtitle.str("");
    histtitle << "Run " << this_run;
    std::string histname = "PlotTAP_AmplitudeTime/h" + ge_channel + "#MaxBinAPGenerator#{no_time_shift=true}_AmplitudeTime";
    TH2F* hAmpTime = (TH2F*) file->Get(histname.c_str());
    if (!hAmpTime) {
      std::cout << "Couldn't get hAmpTime" << std::endl;
      continue;
    }

    TH1F* hAmp_Full = (TH1F*) hAmpTime->ProjectionX("hAmp_Full");
    std::cout << std::fixed;
    std::cout << "hAmp_Full: " << hAmp_Full->GetEntries() << std::endl;

    int min_amp_bin = hAmp_Full->GetXaxis()->FindBin(thresholds[i_run]);
    int max_amp_bin = hAmp_Full->GetNbinsX()-1;
    std::cout << "hAmp_Full (inc. threshold cut) = " << hAmp_Full->Integral(min_amp_bin, max_amp_bin) << std::endl;

    int min_time_cut_bin = hAmpTime->GetYaxis()->FindBin(min_sync_pulse_cut);
    int max_time_cut_bin = hAmpTime->GetYaxis()->FindBin(max_sync_pulse_cut)-1;
    TH1F* hAmp_wTimeCut = (TH1F*) hAmpTime->ProjectionX("hAmp_wTimeCut", min_time_cut_bin, max_time_cut_bin);
    double total_hit_count = hAmp_wTimeCut->GetEntries();
    double hit_count_above_threshold = hAmp_wTimeCut->Integral(min_amp_bin, max_amp_bin);
    std::cout << "hAmp_wTimeCut: " << total_hit_count << std::endl;
    std::cout << "hAmp_wTimeCut (inc. threshold cut) = " << hit_count_above_threshold << std::endl;

    TCanvas* c1 = new TCanvas();
    c1->SetLogy();
    double min_amp_line = hAmp_wTimeCut->GetXaxis()->GetBinLowEdge(min_amp_bin);
    double max_amp_line = hAmp_wTimeCut->GetXaxis()->GetBinUpEdge(max_amp_bin);

    hAmp_wTimeCut->Rebin(8);
    hAmp_wTimeCut->SetStats(false);
    std::stringstream histtitle;
    histtitle << "Run " << this_run;
    hAmp_wTimeCut->SetTitle(histtitle.str().c_str());
    hAmp_wTimeCut->SetXTitle("Amplitude [ADC]");
    std::stringstream axislabel;
    axislabel << "Count / " << hAmp_wTimeCut->GetBinWidth(1) << " ADC";
    hAmp_wTimeCut->SetYTitle(axislabel.str().c_str());

    TLine* min_line = new TLine(min_amp_line, 0, min_amp_line, hAmp_wTimeCut->GetMaximum());
    min_line->SetLineWidth(2);
    min_line->SetLineColor(kRed);

    TLine* max_line = new TLine(max_amp_line, 0, max_amp_line, hAmp_wTimeCut->GetMaximum());
    max_line->SetLineWidth(2);
    max_line->SetLineColor(kRed);

    hAmp_wTimeCut->Draw("HIST E");
    min_line->Draw("LSAME");
    max_line->Draw("LSAME");

    TLatex* latex = new TLatex();
    latex->SetTextSize(0.04);
    latex->SetTextAlign(22);

    std::stringstream text;
    text << min_sync_pulse_cut/1e3 << " #times 10^{3} ns < t < " << max_sync_pulse_cut/1e6 << " #times 10^{6} ns";
    latex->DrawLatexNDC(0.5, 0.8, text.str().c_str());
    
    text.str("");
    text << "Total Hit Count = " << total_hit_count;
    //    latex->DrawLatexNDC(0.5, 0.5, "AlCap Preliminary");
    latex->DrawLatexNDC(0.5, 0.7, text.str().c_str());

    text.str("");
    text << "Hit Count Above Threshold = " << hit_count_above_threshold;
    latex->DrawLatexNDC(0.5, 0.65, text.str().c_str());

  }
}
