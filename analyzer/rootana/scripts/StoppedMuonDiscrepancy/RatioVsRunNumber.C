void RatioVsRunNumber() {

  int n_runs = 100;
  int first_run = 10362;
  int last_run = first_run+n_runs;

  TH1F* hRatioVsRunNumber = new TH1F("hRatioVsRunNumber", "Ratio of Ge Stopped Muon Count / SiL Stopped Muon Count for each Run", n_runs,first_run,last_run);
  hRatioVsRunNumber->SetXTitle("Run Number");
  hRatioVsRunNumber->SetYTitle("Ge / SiL Ratio");

  std::stringstream filename;
  for (int i_run = first_run; i_run < last_run; ++i_run) {
    filename.str("");
    filename << "/gpfs/home/edmonds_a/data/out/v16/stopped-muon-discrepancy-plots_run" << std::setw(5) << std::setfill('0') << i_run << "_eff10319.root";
    
    TFile* file = new TFile(filename.str().c_str(), "READ");
    if (file->IsZombie()) {
      std::cout << "Problem opening file " << filename.str() << std::endl;
      continue;
    }

    TH1F* hRatio = (TH1F*) file->Get("hRatio");

    int i_bin = hRatioVsRunNumber->GetXaxis()->FindBin(i_run);
    hRatioVsRunNumber->SetBinContent(i_bin, hRatio->GetBinContent(hRatio->GetXaxis()->FindBin("SiL1")));
    hRatioVsRunNumber->SetBinError(i_bin, hRatio->GetBinError(hRatio->GetXaxis()->FindBin("SiL1")));

    file->Close();
  }
  hRatioVsRunNumber->Draw("HIST E");
}
