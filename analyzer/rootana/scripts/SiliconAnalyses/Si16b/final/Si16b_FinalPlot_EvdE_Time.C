void Si16b_FinalPlot_EvdE_Time() {

  std::string infilename = "~/data/results/Si16b/plots.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  //  std::string dirname = "all_SiR_timecut"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  //  std::string dirname = "proton_SiR_timecut"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  //  std::string dirname = "proton_SiR_timecut_wSiL1Coinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiL1";
  std::string dirname = "proton_SiR_timecut_wSiL1Coinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiL1";

  TLegend* leg = new TLegend(0.55,0.50,0.9,0.69);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  std::stringstream leglabel;
  
  TCanvas* c_Time = new TCanvas("c_Time", "c_Time");
  c_Time->SetLogy(1);
  std::string inhistname = dirname + "/hTime_12";
  TH1F* hTime = (TH1F*) infile->Get(inhistname.c_str());
  std::string cut = layer1 + " && " + layer2;
  std::string histtitle = "Si16b Dataset, Right Arm, Protons";
  hTime->SetTitle(histtitle.c_str());
  hTime->SetStats(false);
  //  hTime->GetYaxis()->SetRangeUser(0, 5000);
  hTime->Rebin(10);
  hTime->SetLineColor(kBlack);
  hTime->SetMarkerColor(kBlack);
  hTime->GetXaxis()->SetRangeUser(0, 20000);
  leglabel.str("");
  leglabel << cut;
  leg->AddEntry(hTime, leglabel.str().c_str(), "l");

  inhistname = dirname + "/hTime_123";
  TH1F* hTime_123 = (TH1F*) infile->Get(inhistname.c_str());
  cut = layer1 + " && " + layer2 + " && " + layer3;
  histtitle = "Si16b Dataset, Right Arm, Protons";
  hTime_123->SetTitle(histtitle.c_str());
  hTime_123->SetStats(false);
  //  hTime_123->GetXaxis()->SetRangeUser(-1000, 10000);
  hTime_123->Rebin(10);
  hTime_123->SetLineColor(kRed);
  hTime_123->SetMarkerColor(kRed);
  leglabel.str("");
  leglabel << cut;
  leg->AddEntry(hTime_123, leglabel.str().c_str(), "l");

  inhistname = dirname + "/hTime_12not3";
  TH1F* hTime_12not3 = (TH1F*) infile->Get(inhistname.c_str());
  cut = layer1 + " && " + layer2 + " && !" + layer3;
  histtitle = "Si16b Dataset, Right Arm, Protons";
  hTime_12not3->SetTitle(histtitle.c_str());
  hTime_12not3->SetStats(false);
  //  hTime_12not3->GetXaxis()->SetRangeUser(-1000, 10000);
  hTime_12not3->Rebin(10);
  hTime_12not3->SetLineColor(kBlue);
  hTime_12not3->SetMarkerColor(kBlue);
  leglabel.str("");
  leglabel << cut;
  leg->AddEntry(hTime_12not3, leglabel.str().c_str(), "l");

  hTime->Draw("P E");
  hTime_123->Draw("P E SAMES");
  hTime_12not3->Draw("P E SAMES");

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.75, "AlCap Preliminary");

  leg->Draw();

  TCanvas* c2 = new TCanvas("c2", "c2");
  TH1F* hFraction = (TH1F*) hTime->Clone("hFraction_woutSiL1Hit");
  hFraction->Reset();
  for (int i_bin = 1; i_bin <= hFraction->GetNbinsX(); ++i_bin) {
    double total_events_in_time_cut = hTime->Integral(i_bin, hFraction->GetNbinsX());
    double total_events_in_time_cut_woutSiL1 = hTime_12not3->Integral(i_bin, hFraction->GetNbinsX());

    if (total_events_in_time_cut > 0) {
      double fraction = total_events_in_time_cut_woutSiL1 / total_events_in_time_cut;

      std::cout << total_events_in_time_cut << " " << total_events_in_time_cut_woutSiL1 << " " << fraction << std::endl;
      hFraction->SetBinContent(i_bin, fraction);
    }
  }
  hFraction->SetXTitle("Time Cut");
  hFraction->SetYTitle("Fraction of Protons w/out SiL1 hit above time cut");
  hFraction->GetXaxis()->SetRangeUser(0, 20000);
  hFraction->Draw("HIST");
}
