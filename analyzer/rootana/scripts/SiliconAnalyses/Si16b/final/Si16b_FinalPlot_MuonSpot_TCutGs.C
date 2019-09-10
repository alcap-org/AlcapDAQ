void Si16b_FinalPlot_MuonSpot_TCutGs() {

  const int n_both_points = 5;
  double both_x[n_both_points] = {825, 1700, 2300, 1500, 825};
  double both_y[n_both_points] = {625, 1500, 950, 300, 625};
  TCutG* both = new TCutG("both_two_layer", n_both_points, both_x, both_y);
  both->SetLineColor(kBlack);
  both->SetLineWidth(2);
  both->SetFillStyle(0);

  const int n_lower_points = 5;
  double lower_x[n_lower_points] = {both_x[0], 1100, 1700, both_x[3], both_x[4]};
  double lower_y[n_lower_points] = {both_y[0], 880, 440, both_y[3], both_y[4]};
  TCutG* lower = new TCutG("lower_two_layer", n_lower_points, lower_x, lower_y);
  lower->SetLineColor(kRed);
  lower->SetLineWidth(2);
  lower->SetFillStyle(0);

  const int n_upper_points = 5;
  double upper_x[n_upper_points] = {lower_x[1], both_x[1], both_x[2], lower_x[2], lower_x[1]};
  double upper_y[n_upper_points] = {lower_y[1], both_y[1], both_y[2], lower_y[2], lower_y[1]};
  TCutG* upper = new TCutG("upper_two_layer", n_upper_points, upper_x, upper_y);
  upper->SetLineColor(kBlue);
  upper->SetLineWidth(2);
  upper->SetFillStyle(0);

  TFile* outfile = new TFile("~/data/results/Si16b/cut_muon-spots.root", "RECREATE");
  both->Write();
  lower->Write();
  upper->Write();
  outfile->Write();
  outfile->Close();
}
