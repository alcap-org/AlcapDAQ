void SiL3_FinalPlot_DecayElectronCorrection_KFactors_MeV() {

  TFile* file_no_kfactor = new TFile("~/data/mc/SiL3/decayCorr_1M_Geom-P1_muplus_MeV.root", "READ");
  TH1F* hEDep_muplus_no_kfactor = (TH1F*) file_no_kfactor->Get("hEDep_muplus");

  hEDep_muplus_no_kfactor->SetStats(false);
  hEDep_muplus_no_kfactor->SetXTitle("Energy [MeV]");
  hEDep_muplus_no_kfactor->GetXaxis()->SetRangeUser(0,30);
  hEDep_muplus_no_kfactor->Draw("HIST E");

  TFile* file_kfactor_0_9 = new TFile("~/data/mc/SiL3/decayCorr_1M_Geom-P1_muplus_KFactor0-9_MeV.root", "READ");
  TH1F* hEDep_muplus_kfactor_0_9 = (TH1F*) file_kfactor_0_9->Get("hEDep_muplus");
  hEDep_muplus_kfactor_0_9->SetStats(false);
  hEDep_muplus_kfactor_0_9->SetLineColor(kRed);
  hEDep_muplus_kfactor_0_9->GetXaxis()->SetRangeUser(0,30);
  hEDep_muplus_kfactor_0_9->Draw("HIST E SAME");

  TFile* file_kfactor_0_85 = new TFile("~/data/mc/SiL3/decayCorr_1M_Geom-P1_muplus_KFactor0-85_MeV.root", "READ");
  TH1F* hEDep_muplus_kfactor_0_85 = (TH1F*) file_kfactor_0_85->Get("hEDep_muplus");
  hEDep_muplus_kfactor_0_85->SetStats(false);
  hEDep_muplus_kfactor_0_85->SetLineColor(kMagenta);
  hEDep_muplus_kfactor_0_85->GetXaxis()->SetRangeUser(0,30);
  hEDep_muplus_kfactor_0_85->Draw("HIST E SAME");

  hEDep_muplus_no_kfactor->SetMaximum(0.11);

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(hEDep_muplus_no_kfactor, "No Scale Factor", "l");
  leg->AddEntry(hEDep_muplus_kfactor_0_9, "Scale Factor = 0.9", "l");
  leg->AddEntry(hEDep_muplus_kfactor_0_85, "Scale Factor = 0.85", "l");
  leg->Draw();
}
