void Si16a_FinalPlot_MuonSpot_EvdE_WithCuts() {

  const int n_runs = 8;
  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743}; // want to do all runs individually

  for (int i_run = 0; i_run < n_runs; ++i_run) {
    std::stringstream run_str;
    run_str << "0" << run_numbers[i_run];

    std::string infilename = "~/data/results/Si16a/plots" + run_str.str() + ".root";
    TFile* infile = new TFile(infilename.c_str(), "READ");
    if (infile->IsZombie()) {
      std::cout << "Problem opening file " << infilename << std::endl;
      continue;
    }

    std::string dirname = "all_SiR"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
    std::string histname = "hEvdE_TwoLayer_12not3";
    //  std::string histname = "hSingleDetAxes_TwoLayer_12";
    std::string histtitle = "Si16a Dataset, Right Arm (" + layer1 + " && " + layer2 + " && !" + layer3 + " && abs(t_{2} - t_{1}) < 200 ns)";
    
    //  std::string dirname = "all_SiR_nolayercoinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
    //  std::string histname = "hEvdE_TwoLayer_12not3";
    //  std::string histtitle = "Si16a Dataset, Right Arm (" + layer1 + " && " + layer2 + " && !" + layer3 + ", no layer coinc cut)";

    //  std::string dirname = "all_SiR_wSiL1Coinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiL1";
    //  std::string histname = "hEvdE_TwoLayer_12not3";
    //  std::string histtitle = "Si16a Dataset, Right Arm (" + layer1 + " && " + layer2 + " && !" + layer3 + ")";

    //  std::string dirname = "all_SiR_wSiL1Coinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiL1";
    //  std::string histname = "hEvdE_TwoLayer_123";
    //  std::string histtitle = "Si16a Dataset, Right Arm (" + layer1 + " && " + layer2 + " && " + layer3 + ")";

    std::string canvasname = "c_EvdE_" + run_str.str();
    TCanvas* c_EvdE = new TCanvas(canvasname.c_str(), canvasname.c_str());
    std::string inhistname = dirname + "/" + histname;
    //  std::string inhistname = dirname + "/hSingleDetAxes_TwoLayer_12";
    TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
    if (!hEvdE) {
      std::cout << "Can't get histogram " << inhistname << std::endl;
      continue;
    }
    hEvdE->SetTitle(histtitle.c_str());
    hEvdE->SetStats(false);
    hEvdE->GetXaxis()->SetRangeUser(0, 10000);
    hEvdE->GetYaxis()->SetRangeUser(0, 2000);
    hEvdE->Draw("COLZ");
    /*
    const int n_cuts = 3;
    std::string cutnames[n_cuts] = {"both", "lower", "upper"};

    for (int i_cut = 0; i_cut < n_cuts; ++i_cut) {

      std::string cutname = "muspot_" + cutnames[i_cut] + "_SiR/" + cutnames[i_cut] + "_two_layer";
      std::cout << cutname << std::endl;
      TCutG* cut = (TCutG*) infile->Get(cutname.c_str());    
      //    cut->Draw("LSAME");
    }
    */
    
    //    TLatex* latex = new TLatex();
    //    latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
  }
}
