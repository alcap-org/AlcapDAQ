void Ti50_FinalPlot_EvdE() {

  std::string infilename = "~/data/results_old/Ti50/plots_test.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  TCanvas* c_EvdE_two_layer = new TCanvas("c_EvdE_two_layer", "c_EvdE_two_layer");
  
  std::string inhistname = "hEvdE_TwoLayer_all_SiL_timecut";
  //  std::string tcutgname = "proton_cut_two_layer";
  TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
  //  TCutG* tCutG = (TCutG*) infile->Get(tcutgname.c_str());

  hEvdE->SetTitle("Ti50 Dataset, Left Arm");
  hEvdE->SetStats(false);
  hEvdE->GetXaxis()->SetRangeUser(0, 20000);
  hEvdE->GetYaxis()->SetRangeUser(0, 3000);
  hEvdE->Draw("COLZ");

  //  tCutG->SetLineWidth(2);
  //  tCutG->SetLineColor(kRed);
  //  tCutG->Draw("SAME");

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");

  latex->SetTextColor(kRed);
  //  latex->DrawLatex(1000, 1200, "#mu^{-}");
  //  latex->DrawLatex(1600, 2100, "p");
  //  latex->DrawLatex(2400, 2500, "d");
  //  latex->DrawLatex(3000, 2800, "t");

  TCanvas* c_EvdE_three_layer_12 = new TCanvas("c_EvdE_three_layer_12", "c_EvdE_three_layer_12");

  inhistname = "hEvdE_ThreeLayer_12_all_SiL_timecut";
  //  tcutgname = "proton_cut_two_layer";
  TH2F* hEvdE_ThreeLayer_12 = (TH2F*) infile->Get(inhistname.c_str());
  //  TCutG* tCutG_ThreeLayer_12 = (TCutG*) infile->Get(tcutgname.c_str());

  hEvdE_ThreeLayer_12->SetTitle("Ti50 Dataset, Left Arm");
  hEvdE_ThreeLayer_12->SetStats(false);
  hEvdE_ThreeLayer_12->GetXaxis()->SetRangeUser(0, 20000);
  hEvdE_ThreeLayer_12->GetYaxis()->SetRangeUser(0, 3000);
  hEvdE_ThreeLayer_12->Draw("COLZ");

  //  tCutG_ThreeLayer_12->SetLineWidth(2);
  //  tCutG_ThreeLayer_12->SetLineColor(kRed);
  //  tCutG_ThreeLayer_12->Draw("SAME");

  latex->SetTextColor(kBlack);
  latex->DrawLatexNDC(0.25, 0.75, "AlCap Preliminary");

  TCanvas* c_EvdE_three_layer_123 = new TCanvas("c_EvdE_three_layer_123", "c_EvdE_three_layer_123");

  inhistname = "hEvdE_ThreeLayer_123_all_SiL_timecut";
  //  tcutgname = "proton_cut_three_layer";
  TH2F* hEvdE_ThreeLayer_123 = (TH2F*) infile->Get(inhistname.c_str());
  //  TCutG* tCutG_ThreeLayer_123 = (TCutG*) infile->Get(tcutgname.c_str());

  hEvdE_ThreeLayer_123->SetTitle("Ti50 Dataset, Left Arm");
  hEvdE_ThreeLayer_123->SetStats(false);
  //  hEvdE_ThreeLayer_123->GetXaxis()->SetRangeUser(0, 10000);
  //  hEvdE_ThreeLayer_123->GetYaxis()->SetRangeUser(0, 3000);
  hEvdE_ThreeLayer_123->Draw("COLZ");

  //  tCutG_ThreeLayer_123->SetLineWidth(2);
  //  tCutG_ThreeLayer_123->SetLineColor(kRed);
  //  tCutG_ThreeLayer_123->Draw("SAME");

  latex->SetTextColor(kBlack);
  latex->DrawLatexNDC(0.25, 0.75, "AlCap Preliminary");
}
