void Si16PunchThroughPlot() {
  const int n_si_channels = 16;

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->Divide(4, 4);
  TFile* file = new TFile("out09743.root", "READ");

  std::stringstream histname;
  for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
    c1->cd(i_si_channel+1);
    c1->GetPad(i_si_channel+1)->SetLogy();
    histname.str("");
    histname << "PlotTAP_Amplitude/hSiL1-" << i_si_channel+1 << "-S#MaxBinAPGenerator#any_Amplitude";
    TH1F* hAmplitude_All = (TH1F*) file->Get(histname.str().c_str());
    histname.str("");
    histname << "PlotTAP_Amplitude_PunchThroughMuons_Si16/hSiL1-" << i_si_channel+1 << "-S#MaxBinAPGenerator#any_Amplitude_PunchThroughMuons_Si16";
    TH1F* hAmplitude_PunchThrough = (TH1F*) file->Get(histname.str().c_str());
    histname.str("");
    histname << "PlotTAP_Amplitude_PunchThroughMuons_Si16/hSiL1-" << i_si_channel+1 << "-S#MaxBinAPGenerator#any_Amplitude_NotPunchThroughMuons_Si16";
    TH1F* hAmplitude_NotPunchThrough = (TH1F*) file->Get(histname.str().c_str());
    
    hAmplitude_All->Rebin(8);
    hAmplitude_PunchThrough->Rebin(8);
    hAmplitude_NotPunchThrough->Rebin(8);
    
    THStack* hBoth = new THStack("stack", "");
    hBoth->Add(hAmplitude_PunchThrough);
    hBoth->Add(hAmplitude_NotPunchThrough);
    
    hAmplitude_All->SetLineColor(kBlack);
    hAmplitude_All->SetLineWidth(2);
    
    hAmplitude_PunchThrough->SetLineColor(kRed);
    hAmplitude_PunchThrough->SetLineWidth(2);
    hAmplitude_NotPunchThrough->SetLineColor(kBlue);
    hAmplitude_NotPunchThrough->SetLineWidth(2);
    
    hAmplitude_All->Draw("HIST");
    hAmplitude_PunchThrough->Draw("HIST SAME");
    hAmplitude_NotPunchThrough->Draw("HIST SAME");
    
    TLegend* leg = new TLegend(0.61,0.44,0.81,0.88);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.03);
    leg->SetFillColor(kWhite);
    
    std::stringstream leglabel;
    leglabel << "SiL1-4-S Pulses (all)";
    leg->AddEntry(hAmplitude_All, leglabel.str().c_str(), "l");
    
    leglabel.str("");
    leglabel << "SiL1-4-S Pulses (with <200 ns coincidence with SiL3-S)";
    leg->AddEntry(hAmplitude_PunchThrough, leglabel.str().c_str(), "l");
    
    leglabel.str("");
    leglabel << "SiL1-4-S Pulses (without <200 ns coincidence with SiL3-S)";
    leg->AddEntry(hAmplitude_NotPunchThrough, leglabel.str().c_str(), "l");
    leg->Draw();
    //  hBoth->Draw("SAME");
  }
  c1->SaveAs("~/plots/2016-12-16/PunchThroughPlot_AllChannels_Run09743_LogY.pdf");
}
