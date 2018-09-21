void Si16a_MuonBeamEnergy_Plot() {

  const int n_runs = 8;
  int run_numbers[n_runs] = {9737, 9736, 9739, 9740, 9741, 9735, 9742, 9743};
  Int_t colours[n_runs] = {kBlack, kRed, kBlue, kMagenta, kGreen, kYellow, kGray, kViolet};

  TFile* file = new TFile("~/data/results/Si16a_useful/muon_beam_energies.root", "READ");

  std::stringstream run_str;
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    run_str.str("");
    run_str << run_numbers[i_run];

    std::string histname = "hTotalMuonBeamEnergy_" + run_str.str();
    TH1F* hist = (TH1F*) file->Get(histname.c_str());
    hist->SetLineColor(colours[i_run]);
    hist->Scale(1.0 / hist->GetMaximum());
    hist->Draw("SAMES");
  }

}
