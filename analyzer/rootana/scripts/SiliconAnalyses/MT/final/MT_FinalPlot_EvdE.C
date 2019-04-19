void MT_FinalPlot_EvdE() {

  const int n_settings = 2;
  std::string settings[n_settings] = {"SF1-05", "SF1-025"};

  std::string arm = "SiR";
  std::string side = "";
  if (arm == "SiL") { side = "Left"; }
  else if (arm == "SiR") { side = "Right"; }

  std::string dir = "proton_" + arm + "/";
  
  std::string canvasname, histtitle;
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = settings[i_setting];

    std::string infilename = "~/data/results/MT/plots_" + setting + ".root";
    TFile* infile = new TFile(infilename.c_str(), "READ");

    canvasname = "c_EvdE_TwoLayer_12_" + setting;
    TCanvas* c_EvdE_TwoLayer_12 = new TCanvas(canvasname.c_str(), canvasname.c_str());
  
    std::string inhistname = dir + "hEvdE_TwoLayer_12";
    //  std::string tcutgname = "proton_cut_two_layer";
    TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
    //  TCutG* tCutG = (TCutG*) infile->Get(tcutgname.c_str());

    histtitle = "MT Dataset, " + side + " Arm";
    hEvdE->SetTitle(histtitle.c_str());
    //    hEvdE->SetStats(false);
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

    canvasname = "c_EvdE_TwoLayer_123_" + setting;
    TCanvas* c_EvdE_TwoLayer_123 = new TCanvas(canvasname.c_str(), canvasname.c_str());

    inhistname = dir + "hEvdE_TwoLayer_123";
    //  tcutgname = "proton_cut_two_layer";
    TH2F* hEvdE_TwoLayer_123 = (TH2F*) infile->Get(inhistname.c_str());
    //  TCutG* tCutG_TwoLayer_123 = (TCutG*) infile->Get(tcutgname.c_str());

    histtitle = "MT Dataset, " + side + " Arm";
    hEvdE_TwoLayer_123->SetTitle(histtitle.c_str());
    //    hEvdE_TwoLayer_123->SetStats(false);
    hEvdE_TwoLayer_123->GetXaxis()->SetRangeUser(0, 20000);
    hEvdE_TwoLayer_123->GetYaxis()->SetRangeUser(0, 3000);
    hEvdE_TwoLayer_123->Draw("COLZ");

    //  tCutG_TwoLayer_123->SetLineWidth(2);
    //  tCutG_TwoLayer_123->SetLineColor(kRed);
    //  tCutG_TwoLayer_123->Draw("SAME");

    latex->SetTextColor(kBlack);
    latex->DrawLatexNDC(0.25, 0.75, "AlCap Preliminary");

    canvasname = "c_EvdE_TwoLayer_12not3_" + setting;
    TCanvas* c_EvdE_TwoLayer_12not3 = new TCanvas(canvasname.c_str(), canvasname.c_str());

    inhistname = dir + "hEvdE_TwoLayer_12not3";
    //  tcutgname = "proton_cut_three_layer";
    TH2F* hEvdE_TwoLayer_12not3 = (TH2F*) infile->Get(inhistname.c_str());
    //  TCutG* tCutG_TwoLayer_12not3 = (TCutG*) infile->Get(tcutgname.c_str());

    histtitle = "MT Dataset, " + side + " Arm";
    hEvdE_TwoLayer_12not3->SetTitle(histtitle.c_str());
    //    hEvdE_TwoLayer_12not3->SetStats(false);
    hEvdE_TwoLayer_12not3->GetXaxis()->SetRangeUser(0, 20000);
    hEvdE_TwoLayer_12not3->GetYaxis()->SetRangeUser(0, 3000);
    hEvdE_TwoLayer_12not3->Draw("COLZ");

    //  tCutG_TwoLayer_12not3->SetLineWidth(2);
    //  tCutG_TwoLayer_12not3->SetLineColor(kRed);
    //  tCutG_TwoLayer_12not3->Draw("SAME");

    latex->SetTextColor(kBlack);
    latex->DrawLatexNDC(0.25, 0.75, "AlCap Preliminary");
  }
}
