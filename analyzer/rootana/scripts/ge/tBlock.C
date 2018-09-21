void tBlock() {

  const int n_runs = 4;
  int run_numbers[n_runs] = {9302, 9685, 10319, 9040};//, 9740, 10404};
  Int_t colours[n_runs] = {kBlack, kRed, kBlue, kGray+1};//, kMagenta, kCyan};
  double thresholds[n_runs] = {455, 455, 90, 0};

  std::stringstream canvasname, filename, histtitle;
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    int this_run = run_numbers[i_run];

    TLegend* leg = new TLegend(0.58,0.35,0.70,0.65);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.03);
    leg->SetFillColor(kWhite);
    
    
    canvasname.str("");
    canvasname << "c_" << this_run;
    TCanvas* c = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());
    c->SetLogy();

    filename.str("");
    filename << "~/data/hist/local/hist" << std::setw(5) << std::setfill('0') << this_run << ".root";
    TFile* file = new TFile(filename.str().c_str(), "READ");

    histtitle.str("");
    histtitle << "Run " << this_run;
    TH1F* hTDCTimes = (TH1F*) file->Get("hT404_RawTime");
    hTDCTimes->SetTitle(histtitle.str().c_str());
    hTDCTimes->SetYTitle("Raw Count");
    hTDCTimes->SetStats(false);
    //    hTDCTimes->GetXaxis()->SetRangeUser(95e6, 105e6);
    //    hTDCTimes->GetXaxis()->SetRangeUser(0, 5e6);
    hTDCTimes->Draw();

    TH1F* hWFDTimes = (TH1F*) file->Get("hD401_Times");
    hWFDTimes->SetLineColor(kRed);
    //    hWFDTimes->GetXaxis()->SetRangeUser(95e6, 105e6);
    //    hWFDTimes->GetXaxis()->SetRangeUser(0, 5e6);
    hWFDTimes->Draw("SAME");

    leg->AddEntry(hTDCTimes, "TDC Time TGe (hT404_RawTime)", "l");
    leg->AddEntry(hWFDTimes, "WFD Time GeLoGain (hD401_Times)", "l");
    leg->Draw();
  }
}
