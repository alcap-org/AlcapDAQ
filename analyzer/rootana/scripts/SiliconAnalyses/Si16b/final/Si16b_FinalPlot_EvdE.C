void Si16b_FinalPlot_EvdE() {

  std::string infilename = "~/data/results/Si16b/plots.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  //  std::string dirname = "all_SiR_timecut"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  std::string dirname = "proton_SiR_timecut_wSiL1Coinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiL1";

  TCanvas* c_EvdE_two_layer = new TCanvas("c_EvdE_two_layer", "c_EvdE_two_layer");
  std::string inhistname = dirname + "/hEvdE_TwoLayer_12";
  TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
  std::string histtitle = "Si16b Dataset, Right Arm, Two Layer Plot (" + layer1 + " && " + layer2 + ")";
  hEvdE->SetTitle(histtitle.c_str());
  hEvdE->SetStats(false);
  hEvdE->GetXaxis()->SetRangeUser(0, 20000);
  hEvdE->GetYaxis()->SetRangeUser(0, 3000);
  //  hEvdE->GetYaxis()->SetRangeUser(0, 5000);
  hEvdE->Draw("COLZ");

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");

  latex->SetTextColor(kRed);
  latex->DrawLatex(1000, 1200, "#mu^{-}");
  latex->DrawLatex(1600, 2100, "p");
  latex->DrawLatex(2200, 2500, "d");
  latex->DrawLatex(2800, 2900, "t");
  

  TCanvas* c_EvdE_two_layer_123 = new TCanvas("c_EvdE_two_layer_123", "c_EvdE_two_layer_123");
  inhistname = dirname + "/hEvdE_TwoLayer_123";
  TH2F* hEvdE_TwoLayer_123 = (TH2F*) infile->Get(inhistname.c_str());
  histtitle = "Si16b Dataset, Right Arm, Two Layer Plot (" + layer1 + " && " + layer2 + " && " + layer3 + ")";
  hEvdE_TwoLayer_123->SetTitle(histtitle.c_str());
  hEvdE_TwoLayer_123->SetStats(false);
  hEvdE_TwoLayer_123->GetXaxis()->SetRangeUser(0, 20000);
  hEvdE_TwoLayer_123->GetYaxis()->SetRangeUser(0, 3000);
  hEvdE_TwoLayer_123->Draw("COLZ");

  latex->SetTextColor(kBlack);
  latex->DrawLatexNDC(0.25, 0.75, "AlCap Preliminary");

  TCanvas* c_EvdE_two_layer_12not3 = new TCanvas("c_EvdE_two_layer_12not3", "c_EvdE_two_layer_12not3");
  inhistname = dirname + "/hEvdE_TwoLayer_12not3";
  TH2F* hEvdE_TwoLayer_12not3 = (TH2F*) infile->Get(inhistname.c_str());
  histtitle = "Si16b Dataset, Right Arm, Two Layer Plot (" + layer1 + " && " + layer2 + " && !" + layer3 + ")";
  hEvdE_TwoLayer_12not3->SetTitle(histtitle.c_str());
  hEvdE_TwoLayer_12not3->SetStats(false);
  hEvdE_TwoLayer_12not3->GetXaxis()->SetRangeUser(0, 20000);
  hEvdE_TwoLayer_12not3->GetYaxis()->SetRangeUser(0, 3000);
  hEvdE_TwoLayer_12not3->Draw("COLZ");

  latex->SetTextColor(kBlack);
  latex->DrawLatexNDC(0.25, 0.75, "AlCap Preliminary");

  
  TCanvas* c_EvdE_three_layer_123 = new TCanvas("c_EvdE_three_layer_123", "c_EvdE_three_layer_123");
  inhistname = dirname + "/hEvdE_ThreeLayer_123";
  TH2F* hEvdE_ThreeLayer_123 = (TH2F*) infile->Get(inhistname.c_str());
  histtitle = "Si16b Dataset, Right Arm, Three Layer Plot (" + layer1 + " && " + layer2 + " && " + layer3 + ")";
  hEvdE_ThreeLayer_123->SetTitle(histtitle.c_str());
  hEvdE_ThreeLayer_123->SetStats(false);
  //  hEvdE_ThreeLayer_123->GetXaxis()->SetRangeUser(0, 10000);
  //  hEvdE_ThreeLayer_123->GetYaxis()->SetRangeUser(0, 3000);
  hEvdE_ThreeLayer_123->Draw("COLZ");

  latex->SetTextColor(kBlack);
  latex->DrawLatexNDC(0.25, 0.75, "AlCap Preliminary");
  
  // Draw the particle cuts
  //  const int n_particles = 4;
  //  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  const int n_particles = 1;
  std::string particles[n_particles] = {"proton"};
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string this_particle = particles[i_particle];
    
    c_EvdE_two_layer->cd();
    std::string tcutgname = this_particle + "_SiR_timecut/" + this_particle + "_cut_two_layer";
    TCutG* tCutG = (TCutG*) infile->Get(tcutgname.c_str());
    tCutG->SetLineWidth(2);
    tCutG->SetLineColor(kRed);
    tCutG->Draw("SAME");


    c_EvdE_two_layer_123->cd();
    //    tcutgname = this_particle + "_SiR_timecut/" + this_particle + "_cut_two_layer";
    //    TCutG* tCutG_ThreeLayer_12 = (TCutG*) infile->Get(tcutgname.c_str());
    //    tCutG_ThreeLayer_12->SetLineWidth(2);
    //    tCutG_ThreeLayer_12->SetLineColor(kRed);
    //    tCutG_ThreeLayer_12->Draw("SAME");
    tCutG->Draw("SAME");

    c_EvdE_two_layer_12not3->cd();
    tCutG->Draw("SAME");

    if (i_particle < 1) { // only have the three layer cut for protons
      c_EvdE_two_layer_123->cd();
      tcutgname = this_particle + "_SiR_timecut/" + this_particle + "_cut_three_layer";
      TCutG* tCutG_ThreeLayer_123 = (TCutG*) infile->Get(tcutgname.c_str());
      tCutG_ThreeLayer_123->SetLineWidth(2);
      tCutG_ThreeLayer_123->SetLineColor(kRed);
      tCutG_ThreeLayer_123->Draw("SAME");
    }
  }
}
