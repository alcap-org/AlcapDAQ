void Ti50_FinalPlot_EvdE() {

  std::string infilename = "~/data/results/Ti50/plots.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  const int n_arms = 2;
  std::string arms[n_arms] = {"SiR", "SiL"};

  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};

  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {

    std::string this_arm = arms[i_arm];

    std::string canvasname = "c_EvdE_two_layer_" + this_arm;
    TCanvas* c_EvdE_two_layer = new TCanvas(canvasname.c_str(), canvasname.c_str());
    std::string inhistname = "all_" + this_arm + "_timecut/hEvdE_TwoLayer";
    TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
    std::string histtitle = "Ti50 Dataset, " + this_arm + ", Two Layer Analysis";
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
  
    canvasname = "c_EvdE_three_layer_12_" + this_arm;
    TCanvas* c_EvdE_three_layer_12 = new TCanvas(canvasname.c_str(), canvasname.c_str());
    inhistname = "all_" + this_arm + "_timecut/hEvdE_ThreeLayer_12";
    TH2F* hEvdE_ThreeLayer_12 = (TH2F*) infile->Get(inhistname.c_str());
    histtitle = "Ti50 Dataset, " + this_arm + ", Three Layer Analysis";
    hEvdE_ThreeLayer_12->SetTitle(histtitle.c_str());
    hEvdE_ThreeLayer_12->SetStats(false);
    hEvdE_ThreeLayer_12->GetXaxis()->SetRangeUser(0, 20000);
    hEvdE_ThreeLayer_12->GetYaxis()->SetRangeUser(0, 3000);
    hEvdE_ThreeLayer_12->Draw("COLZ");

    latex->SetTextColor(kBlack);
    latex->DrawLatexNDC(0.25, 0.75, "AlCap Preliminary");

    canvasname = "c_EvdE_three_layer_123_" + this_arm;
    TCanvas* c_EvdE_three_layer_123 = new TCanvas(canvasname.c_str(), canvasname.c_str());
    inhistname = "all_" + this_arm + "_timecut/hEvdE_ThreeLayer_123";
    TH2F* hEvdE_ThreeLayer_123 = (TH2F*) infile->Get(inhistname.c_str());
    histtitle = "Ti50 Dataset, " + this_arm + ", Three Layer Analysis";
    hEvdE_ThreeLayer_123->SetTitle(histtitle.c_str());
    hEvdE_ThreeLayer_123->SetStats(false);
    //  hEvdE_ThreeLayer_123->GetXaxis()->SetRangeUser(0, 10000);
    //  hEvdE_ThreeLayer_123->GetYaxis()->SetRangeUser(0, 3000);
    hEvdE_ThreeLayer_123->Draw("COLZ");

    latex->SetTextColor(kBlack);
    latex->DrawLatexNDC(0.25, 0.75, "AlCap Preliminary");
  
    // Draw the particle cuts
    for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
      std::string this_particle = particles[i_particle];
    
      c_EvdE_two_layer->cd();
      std::string tcutgname = this_particle + "_" + this_arm + "_timecut/" + this_particle + "_cut_two_layer";
      TCutG* tCutG = (TCutG*) infile->Get(tcutgname.c_str());
      tCutG->SetLineWidth(2);
      tCutG->SetLineColor(kRed);
      tCutG->Draw("SAME");


      c_EvdE_three_layer_12->cd();
      tcutgname = this_particle + "_" + this_arm + "_timecut/" + this_particle + "_cut_two_layer";
      TCutG* tCutG_ThreeLayer_12 = (TCutG*) infile->Get(tcutgname.c_str());
      tCutG_ThreeLayer_12->SetLineWidth(2);
      tCutG_ThreeLayer_12->SetLineColor(kRed);
      tCutG_ThreeLayer_12->Draw("SAME");

      if (i_particle < 1) { // only have the three layer cut for protons
	c_EvdE_three_layer_123->cd();
	tcutgname = this_particle + "_" + this_arm + "_timecut/" + this_particle + "_cut_three_layer";
	TCutG* tCutG_ThreeLayer_123 = (TCutG*) infile->Get(tcutgname.c_str());
	tCutG_ThreeLayer_123->SetLineWidth(2);
	tCutG_ThreeLayer_123->SetLineColor(kRed);
	tCutG_ThreeLayer_123->Draw("SAME");
      }
    }
  }
}
