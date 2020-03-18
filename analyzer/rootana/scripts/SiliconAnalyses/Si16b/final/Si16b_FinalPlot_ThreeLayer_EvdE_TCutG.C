void Si16b_FinalPlot_ThreeLayer_EvdE_TCutG(std::string savedir = "") {

  std::string infilename = "~/data/results/Si16b/plots_newPP_geq1TgtPulse_3sigma.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  std::string dirname = "all_SiR_timecut0_10000ns_layerCoinc";
  //  std::string dirname = "all_SiR_timecut400_10000ns_layerCoinc_wSiL1_14Coinc";
  std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  //  std::string dirname = "all_SiR_nolayercoinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";

  TCanvas* c_EvdE = new TCanvas("c_EvdE", "c_EvdE");
  std::string inhistname = dirname + "/hEvdE_ThreeLayer_123";
  TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
  if (!hEvdE) {
    std::cout << "Can't find histogram " << inhistname << std::endl;
    return;
  }
  std::string histtitle = "Si16b Dataset, Right Arm (" + layer1 + " && " + layer2 + " && " + layer3 + " && SiL1)";
  hEvdE->SetTitle(histtitle.c_str());
  hEvdE->SetStats(false);
  hEvdE->Rebin2D(10, 10);
  hEvdE->GetXaxis()->SetRangeUser(0, 25000);
  hEvdE->GetXaxis()->SetTitleOffset(0.9);
  //  hEvdE->GetYaxis()->SetRangeUser(0, 3000);
  hEvdE->GetYaxis()->SetRangeUser(0, 20000);
  hEvdE->GetYaxis()->SetTitleOffset(0.9);
  hEvdE->Draw("COLZ");

  TLatex* latex = new TLatex();
  latex->SetTextColor(kRed);
  //  latex->DrawLatex(1000, 1200, "#mu^{-}");
  latex->DrawLatex(15500, 16000, "p");
  latex->DrawLatex(22000, 18000, "d");
  //  latex->DrawLatex(2800, 2900, "t");
  //  latex->DrawLatex(8000, 7500, "#alpha");

  alcaphistogram(hEvdE);
  alcapPreliminary(hEvdE);
  hEvdE->SetDrawOption("COLZ");

  // Draw the particle cuts
  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string this_particle = particles[i_particle];
    
    c_EvdE->cd();
    //    std::string tcutgname = this_particle + "3L_SiR_timecut0_10000ns_layerCoinc/" + this_particle + "_cut_three_layer";
    std::string tcutgname = this_particle + "3L_SiR_timecut0_10000ns_layerCoinc/r_sir3_hLg_SiR_EvDeltaE_" + this_particle + "_3sigma_keV";
    TCutG* tCutG = (TCutG*) infile->Get(tcutgname.c_str());
    if (!tCutG) {
      std::cout << "Error: Can't get " << tcutgname << std::endl;
      continue;
    }
    tCutG->SetLineWidth(2);
    tCutG->SetLineColor(kRed);
    tCutG->Draw("SAME");
  }

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_Si16bDataset_ThreeLayer_EvdE_TCutG";

    std::string pngname = savename + ".png";
    c_EvdE->SaveAs(pngname.c_str());
  }

}
