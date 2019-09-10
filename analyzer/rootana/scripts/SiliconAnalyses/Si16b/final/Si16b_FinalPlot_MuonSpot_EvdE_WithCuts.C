void Si16b_FinalPlot_MuonSpot_EvdE_WithCuts() {
  
  std::string infilename = "~/data/results/Si16b/plots_newPP.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  std::string dirname = "all_SiR"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  std::string histname = "hEvdE_TwoLayer_12not3";
  //  std::string histname = "hSingleDetAxes_TwoLayer_12";
  std::string histtitle = "Si16b Dataset, Right Arm (" + layer1 + " && " + layer2 + " && !" + layer3 + " && abs(t_{2} - t_{1}) < 200 ns)";

  //  std::string dirname = "all_SiR_nolayercoinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  //  std::string histname = "hEvdE_TwoLayer_12not3";
  //  std::string histtitle = "Si16b Dataset, Right Arm (" + layer1 + " && " + layer2 + " && !" + layer3 + ", no layer coinc cut)";

  //  std::string dirname = "all_SiR_wSiL1Coinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiL1";
  //  std::string histname = "hEvdE_TwoLayer_12not3";
  //  std::string histtitle = "Si16b Dataset, Right Arm (" + layer1 + " && " + layer2 + " && !" + layer3 + ")";

  //  std::string dirname = "all_SiR_wSiL1Coinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiL1";
  //  std::string histname = "hEvdE_TwoLayer_123";
  //  std::string histtitle = "Si16b Dataset, Right Arm (" + layer1 + " && " + layer2 + " && " + layer3 + ")";

  TCanvas* c_EvdE = new TCanvas("c_EvdE", "c_EvdE");
  std::string inhistname = dirname + "/" + histname;
  //  std::string inhistname = dirname + "/hSingleDetAxes_TwoLayer_12";
  TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
  hEvdE->SetTitle(histtitle.c_str());
  hEvdE->SetStats(false);
  hEvdE->GetXaxis()->SetRangeUser(0, 10000);
  hEvdE->GetYaxis()->SetRangeUser(0, 2000);
  hEvdE->Draw("COLZ");

  const int n_cuts = 3;
  std::string cutnames[n_cuts] = {"both", "lower", "upper"};

  for (int i_cut = 0; i_cut < n_cuts; ++i_cut) {

    std::string cutname = "muspot_" + cutnames[i_cut] + "_SiR/" + cutnames[i_cut] + "_two_layer";
    std::cout << cutname << std::endl;
    TCutG* cut = (TCutG*) infile->Get(cutname.c_str());    
    //    cut->Draw("LSAME");
  }

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
}
