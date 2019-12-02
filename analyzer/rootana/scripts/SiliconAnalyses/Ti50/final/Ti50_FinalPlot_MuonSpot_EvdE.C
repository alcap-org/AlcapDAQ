void Ti50_FinalPlot_MuonSpot_EvdE() {

  std::string infilename = "~/data/results/Ti50/plots.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  std::string arm = "Right";
  std::string dirname;
  if (arm == "Right") {
    dirname = "all_SiR"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  }
  else if (arm == "Left") {
    dirname = "all_SiL"; std::string layer1 = "SiL1"; std::string layer2 = "SiL3"; std::string layer3 = "SiL2";
  }
  else {
    std::cout << "Arm can be either Right or Left. Not " << arm << std::endl;
    return;
  }

  const int n_both_points = 5;
  double both_x[n_both_points] = {825, 1700, 2300, 1500, 825};
  double both_y[n_both_points] = {625, 1500, 950, 300, 625};
  TCutG* both = new TCutG("both", n_both_points, both_x, both_y);
  both->SetLineColor(kBlack);
  both->SetLineWidth(2);
  both->SetFillStyle(0);

  const int n_lower_points = 5;
  double lower_x[n_lower_points] = {both_x[0], 1100, 1700, both_x[3], both_x[4]};
  double lower_y[n_lower_points] = {both_y[0], 880, 440, both_y[3], both_y[4]};
  TCutG* lower = new TCutG("lower", n_lower_points, lower_x, lower_y);
  lower->SetLineColor(kRed);
  lower->SetLineWidth(2);
  lower->SetFillStyle(0);

  const int n_upper_points = 5;
  double upper_x[n_upper_points] = {lower_x[1], both_x[1], both_x[2], lower_x[2], lower_x[1]};
  double upper_y[n_upper_points] = {lower_y[1], both_y[1], both_y[2], lower_y[2], lower_y[1]};
  TCutG* upper = new TCutG("upper", n_upper_points, upper_x, upper_y);
  upper->SetLineColor(kBlue);
  upper->SetLineWidth(2);
  upper->SetFillStyle(0);

  TCanvas* c_EvdE = new TCanvas("c_EvdE", "c_EvdE");
  std::string inhistname = dirname + "/hEvdE_TwoLayer_12not3";
  TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
  std::string histtitle;
  if (arm == "Right") {
    histtitle = "Ti50 Dataset, Right Arm (" + layer1 + " && " + layer2 + " && !" + layer3 + ")";
  }
  else if (arm == "Left") {
    histtitle = "Ti50 Dataset, Left Arm (" + layer1 + " && " + layer2 + ")";
  }

  hEvdE->SetTitle(histtitle.c_str());
  hEvdE->SetStats(false);
  hEvdE->GetXaxis()->SetRangeUser(0, 20000);
  hEvdE->GetYaxis()->SetRangeUser(0, 3000);
  //  hEvdE->GetYaxis()->SetRangeUser(0, 10000);
  hEvdE->Draw("COLZ");

  double integral_both = 0;
  double integral_lower = 0;
  double integral_upper = 0;
  for (int i_bin = 1; i_bin <= hEvdE->GetXaxis()->GetNbins(); ++i_bin) {
    for (int j_bin = 1; j_bin <= hEvdE->GetYaxis()->GetNbins(); ++j_bin) {
      
      double bin_x = hEvdE->GetXaxis()->GetBinCenter(i_bin);
      double bin_y = hEvdE->GetXaxis()->GetBinCenter(j_bin);
      double bin_content = hEvdE->GetBinContent(i_bin, j_bin);
      
      // Test if this bin is within the bounds of the TCutG
      if (both->IsInside(bin_x, bin_y)) {
	integral_both += bin_content;
      }
      if (lower->IsInside(bin_x, bin_y)) {
	integral_lower += bin_content;
      }
      if (upper->IsInside(bin_x, bin_y)) {
	integral_upper += bin_content;
      }
    }
  }
  
  std::cout << "Si" << arm << ": both = " << integral_both
	    << ", lower = " << integral_lower << " (" << (integral_lower / integral_both)*100 << "%)"
	    << ", upper = " << integral_upper << " (" << (integral_upper / integral_both)*100 << "%)"
	    << ", lower / upper = " << integral_lower / integral_upper
	    << std::endl;
  
  both->Draw("LSAME");
  lower->Draw("LSAME");
  upper->Draw("LSAME");

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
}
