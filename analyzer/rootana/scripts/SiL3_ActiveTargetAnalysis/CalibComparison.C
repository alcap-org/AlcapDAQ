void CalibComparison() {

  TFile* old_calib_file = new TFile("scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run9040_Range-0-100MeV_Binning-100keV.root", "READ");
  TH2F* hEnergyVsTime_old_calib = (TH2F*) old_calib_file->Get("hTimeEnergy");

  TFile* new_calib_file = new TFile("scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run9040_Range-0-100MeV_Binning-100keV_NewCalib.root", "READ");
  TH2F* hEnergyVsTime_new_calib = (TH2F*) new_calib_file->Get("hTimeEnergy");

  double min_time = 2000;
  double max_time = 4000;
  int min_time_bin = hEnergyVsTime_old_calib->GetXaxis()->FindBin(min_time);
  int max_time_bin = hEnergyVsTime_old_calib->GetXaxis()->FindBin(max_time);
  TH1D* hEnergy_old_calib = hEnergyVsTime_old_calib->ProjectionY("hEnergy_old_calib", min_time_bin, max_time_bin);
  TH1D* hEnergy_new_calib = hEnergyVsTime_new_calib->ProjectionY("hEnergy_new_calib", min_time_bin, max_time_bin);

  hEnergy_old_calib->GetXaxis()->SetRangeUser(0, 50000);
  hEnergy_old_calib->Draw("HIST E");

  hEnergy_new_calib->GetXaxis()->SetRangeUser(0, 50000);
  hEnergy_new_calib->SetLineColor(kRed);
  hEnergy_new_calib->Draw("HIST E SAMES");
}
