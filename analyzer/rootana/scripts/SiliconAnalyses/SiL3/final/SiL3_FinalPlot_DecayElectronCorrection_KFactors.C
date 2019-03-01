void SiL3_FinalPlot_DecayElectronCorrection_KFactors() {

  TFile* file_no_kfactor = new TFile("~/data/mc/SiL3/corrections_1M_wRes_wThresh_50keVBins.root", "READ");
  TH1F* hEDep_muplus_no_kfactor = (TH1F*) file_no_kfactor->Get("hEDep_muplus");

  hEDep_muplus_no_kfactor->SetStats(false);
  hEDep_muplus_no_kfactor->GetXaxis()->SetRangeUser(0,5000);
  hEDep_muplus_no_kfactor->Draw("HIST E");

  TFile* file_kfactor_0_9 = new TFile("~/data/mc/SiL3/corrections_1M_wRes_wThresh_wKFactor0-9_50keVBins.root", "READ");
  TH1F* hEDep_muplus_kfactor_0_9 = (TH1F*) file_kfactor_0_9->Get("hEDep_muplus");
  hEDep_muplus_kfactor_0_9->SetStats(false);
  hEDep_muplus_kfactor_0_9->SetLineColor(kRed);
  hEDep_muplus_kfactor_0_9->GetXaxis()->SetRangeUser(0,5000);
  hEDep_muplus_kfactor_0_9->Draw("HIST E SAME");

  hEDep_muplus_no_kfactor->SetMaximum(0.11);

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(hEDep_muplus_no_kfactor, "No Scale Factor", "l");
  leg->AddEntry(hEDep_muplus_kfactor_0_9, "Scale Factor = 0.9", "l");
  leg->Draw();
}
