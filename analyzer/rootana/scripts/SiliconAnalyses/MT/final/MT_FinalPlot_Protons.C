void MT_FinalPlot_Protons() {

  const int n_settings = 2;
  std::string settings[n_settings] = {"SF1-05", "SF1-025"};

  std::string arm = "SiL";
  std::string side = "";
  if (arm == "SiL") { side = "Left"; }
  else if (arm == "SiR") { side = "Right"; }

  std::string particle = "proton";
  
  std::string dir = particle + "_" + arm + "/";
  
  std::string canvasname, histtitle;
  std::stringstream axistitle;
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

    histtitle = "MT Dataset, " + side + " Arm, (" + particle + ")";
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

    canvasname = "c_Energy_12_" + setting;
    TCanvas* c_Energy_12 = new TCanvas(canvasname.c_str(), canvasname.c_str());

    //    inhistname = dir + "hEnergy_12";
    TH1D* hEnergy_12 = hEvdE->ProjectionX("hEnergy_12");

    histtitle = "MT Dataset, " + side + " Arm, (" + particle + ")";
    hEnergy_12->SetTitle(histtitle.c_str());
    //    hEnergy_12->SetStats(false);
    hEnergy_12->Rebin(10);
    axistitle.str("");
    axistitle << "Counts / " << hEnergy_12->GetXaxis()->GetBinWidth(1) << " keV";
    hEnergy_12->SetYTitle(axistitle.str().c_str());
    hEnergy_12->GetXaxis()->SetRangeUser(0, 20000);
    hEnergy_12->Draw("HIST E");

    latex->SetTextColor(kBlack);
    latex->DrawLatexNDC(0.25, 0.75, "AlCap Preliminary");

    canvasname = "c_Time_TwoLayer_12_" + setting;
    TCanvas* c_Time_TwoLayer_12 = new TCanvas(canvasname.c_str(), canvasname.c_str());

    inhistname = dir + "hTime_12";
    //  tcutgname = "proton_cut_two_layer";
    TH1F* hTime_12 = (TH1F*) infile->Get(inhistname.c_str());
    //  TCutG* tCutG_TwoLayer_123 = (TCutG*) infile->Get(tcutgname.c_str());

    histtitle = "MT Dataset, " + side + " Arm, (" + particle + ")";
    hTime_12->SetTitle(histtitle.c_str());
    //    hTime_12->SetStats(false);
    hTime_12->Rebin(10);
    axistitle.str("");
    axistitle << "Counts / " << hTime_12->GetXaxis()->GetBinWidth(1) << " ns";
    hTime_12->SetYTitle(axistitle.str().c_str());
    hTime_12->GetXaxis()->SetRangeUser(0, 20000);
    //    hTime_12->GetYaxis()->SetRangeUser(0, 3000);
    hTime_12->Draw("HIST E");


    latex->SetTextColor(kBlack);
    latex->DrawLatexNDC(0.25, 0.75, "AlCap Preliminary");
  }
}
