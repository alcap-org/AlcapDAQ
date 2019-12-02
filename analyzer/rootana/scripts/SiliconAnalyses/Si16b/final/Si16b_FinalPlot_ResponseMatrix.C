void Si16b_FinalPlot_ResponseMatrix() {

  std::string infilename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse.root";
  std::string particle = "proton";
  //  std::string particle = "deuteron";
  //  std::string particle = "triton";
  //  std::string particle = "alpha";
  std::string inhistname = "ResponseMatrix_" + particle + "_PSel/hResponseMatrix";
  
  TFile* infile = new TFile(infilename.c_str(), "READ");
  TH2F* hResponseMatrix = (TH2F*) infile->Get(inhistname.c_str());

  TCanvas* c_Response = new TCanvas("c_Response", "c_Response");

  std::string histtitle;
  histtitle = "Si16b MC, Right Arm, " + particle + " Response Matrix";
  hResponseMatrix->SetTitle(histtitle.c_str());
  hResponseMatrix->SetStats(false);
  //  gStyle->SetOptFit(1);
  hResponseMatrix->GetYaxis()->SetTitleOffset(1.3);
  hResponseMatrix->Draw("COLZ");

  if (particle == "alpha") {
    double trust_observed = 10000;
    double y_max = 30000;
    TLine* observed_line = new TLine(trust_observed, 0, trust_observed, y_max);
    observed_line->SetLineWidth(2);
    observed_line->SetLineColor(kBlack);
    observed_line->Draw("LSAME");

    double trust_true = 18000;
    double bin_width = 1000;
    double x_max = 30000;
    TBox* good_box = new TBox(trust_observed, trust_true, x_max, trust_true+bin_width);
    good_box->SetLineWidth(2);
    good_box->SetLineColor(kSpring);
    //    good_box->SetFillColor(kSpring);
    good_box->SetFillStyle(0);
    good_box->Draw("");

    TBox* bad_box = new TBox(0, trust_true, trust_observed, trust_true+bin_width);
    bad_box->SetLineWidth(2);
    bad_box->SetLineColor(kRed);
    //    bad_box->SetFillColor(kSpring);
    bad_box->SetFillStyle(0);
    bad_box->Draw("");
    //    TLine* true_line = new TLine(0, trust_true, trust_observed, trust_true);
    //    true_line->SetLineWidth(2);
    //    true_line->SetLineColor(kRed);
    //    true_line->Draw("LSAME");
  }

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.45, "AlCap Monte Carlo");
}
