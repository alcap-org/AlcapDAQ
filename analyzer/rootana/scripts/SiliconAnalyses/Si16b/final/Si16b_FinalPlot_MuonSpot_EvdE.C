void Si16b_FinalPlot_MuonSpot_EvdE() {

  
  std::string infilename = "~/data/results/Si16b/plots.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  //  std::string dirname = "all_SiR_wSiL1Coinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiL1";
  std::string dirname = "all_SiR"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  //  std::string dirname = "muspot_both_SiR"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  
  TCanvas* c_EvdE = new TCanvas("c_EvdE", "c_EvdE");
  std::string inhistname = dirname + "/hEvdE_TwoLayer_12not3";
  //  std::string inhistname = dirname + "/hSingleDetAxes_TwoLayer_12";
  TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
  std::string histtitle = "Si16b Dataset, Right Arm (" + layer1 + " && " + layer2 + " && !" + layer3 + ")";
  hEvdE->SetTitle(histtitle.c_str());
  hEvdE->SetStats(false);
  hEvdE->GetXaxis()->SetRangeUser(0, 20000);
  hEvdE->GetYaxis()->SetRangeUser(0, 3000);
  hEvdE->Draw("COLZ");

  
  both->Draw("LSAME");
  lower->Draw("LSAME");
  upper->Draw("LSAME");

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
}
