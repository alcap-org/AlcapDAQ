void SiTEDep_Run10477() {

  TFile* file = new TFile("~/data/out/local/out10477_mu-beam-pos-and-edep.root");
  //  TFile* file = new TFile("~/data/out/local/out10477_mu-beam-pos-and-edep_new-sit-calib.root");

  std::stringstream histname;
  TH1D* sum_hist;
  for (int i_channel = 1; i_channel <= 4; ++i_channel) {
    histname.str("");
    histname << "PlotTAP_EnergyTime/hSiT-" << i_channel << "-S#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= false}_EnergyTime";
    TH2F* hist2D = (TH2F*) file->Get(histname.str().c_str());
    TH1D* hist1D = hist2D->ProjectionX();
    
    if (i_channel == 1) {
      sum_hist = (TH1D*) hist1D->Clone("sum_hist");
    }
    else {
      sum_hist->Add(hist1D);
    }
  }
  sum_hist->Rebin(25);
  sum_hist->GetXaxis()->SetRangeUser(0, 1500);
  sum_hist->Draw();

  c1->SaveAs("~/plots/2018-05-07/AlCapData_Run10477_SiTEDep_SameBinning.root");
}
