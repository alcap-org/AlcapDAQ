void Si16b_FinalPlot_ResponseMatrix(std::string savedir = "") {

  bool demo = false;
  //  std::string infilename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_covError_1.root";
    std::string infilename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_JohnVetoEff_1MTests_1.root";

    const int n_particles = 1;//4;
    std::string particles[n_particles] = {"proton"};//, "deuteron", "triton", "alpha"};

  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    //    std::string inhistname = "ResponseMatrix_" + particle + "_TCutG_2sig_layerCoinc500ns_tGT0ns/hResponseMatrix";
    std::string inhistname = "ResponseMatrix_" + particle + "_TCutG_2sig_layerCoinc500ns_tGT0ns_BinW1MeV/hResponseMatrix";
  
    TFile* infile = new TFile(infilename.c_str(), "READ");
    TH2F* hResponseMatrix = (TH2F*) infile->Get(inhistname.c_str());

    TCanvas* c_Response = new TCanvas();
    
    std::string histtitle;
    histtitle = "Si16b MC, Right Arm, " + particle + " Response Matrix";
    hResponseMatrix->SetTitle(histtitle.c_str());
    hResponseMatrix->SetStats(false);
    //  gStyle->SetOptFit(1);
    hResponseMatrix->GetYaxis()->SetTitleOffset(1.3);
    hResponseMatrix->Draw("COLZ");

    alcaphistogram(hResponseMatrix);
    alcapSimulation(hResponseMatrix);
    alcaplabel(particle, hResponseMatrix);
    hResponseMatrix->SetDrawOption("COLZ");
    
    if (particle == "alpha" && demo == true) {
      double trust_observed = 10;
      double y_max = 30;
      TLine* observed_line = new TLine(trust_observed, 0, trust_observed, y_max);
      observed_line->SetLineWidth(2);
      observed_line->SetLineColor(kBlack);
      observed_line->Draw("LSAME");
      
      double trust_true = 18;
      double bin_width = 1;
      double x_max = 30;
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

    if (savedir != "") {
      std::string savename = savedir + "AlCapData_Si16bDataset_ResponseMatrix_" + particle;
      
      std::string pngname = savename + ".png";
      c_Response->SaveAs(pngname.c_str());
    }
  }

  //  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.55, 0.45, "AlCap Monte Carlo");
}
