void Si16b_FinalPlot_TwoLayer_EvdE_Rotated(std::string savedir = "", std::ostream& numbers_file = std::cout) {

  std::string infilename = "~/data/results/Si16b/plots_newPP_geq1TgtPulse_3sigma_wRotated.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  std::string dirname = "all_SiR_timecut0_10000ns_layerCoinc";
  //  std::string dirname = "all_SiR_timecut400_10000ns_layerCoinc_wSiL1_14Coinc";
  std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  //  std::string dirname = "all_SiR_nolayercoinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";

  std::string inhistname = dirname + "/hRotatedEvdE_TwoLayer_12not3";
  //  std::string inhistname = dirname + "/hEvdE_TwoLayer_123";
  TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
  std::string histtitle = "Si16b Dataset, Right Arm (" + layer1 + " && " + layer2 + " && !" + layer3 + " && SiL1)";
  hEvdE->SetTitle(histtitle.c_str());
  hEvdE->SetStats(false);
  hEvdE->GetXaxis()->SetRangeUser(0, 1.5);
  hEvdE->GetYaxis()->SetRangeUser(0, 1.6);
  hEvdE->SetXTitle("Transformed X");
  hEvdE->SetYTitle("Transformed Y");
  hEvdE->GetYaxis()->SetTitleOffset(0.9);
  hEvdE->GetXaxis()->SetTitleOffset(0.9);

  TCanvas* c_EvdE = new TCanvas("c_EvdE", "c_EvdE");
  hEvdE->Draw();
  // TLatex* latex = new TLatex();
  // latex->SetTextColor(kRed);
  // latex->DrawLatex(1000, 1200, "#mu^{-}");
  // latex->DrawLatex(1600, 2100, "p");
  // latex->DrawLatex(2200, 2500, "d");
  // latex->DrawLatex(2800, 2900, "t");
  // latex->DrawLatex(8000, 7500, "#alpha");

  //  alcaphistogram(hEvdE);
  //  alcapPreliminary(hEvdE);
  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.5, 0.8, "AlCap #bf{#it{Preliminary}}");
  //  hEvdE->SetDrawOption("COLZ");

  // Draw the particle cuts
  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  Int_t colours[n_particles] = {kRed, kCyan, kMagenta, kSpring};
  TFile* cut_file = new TFile("~/data/results/Si16b/mark-si16b-cuts.root", "READ");

  TLegend* leg = new TLegend(0.60,0.50,0.85,0.70);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);

  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string this_particle = particles[i_particle];
      
    std::string tcutgname = "hLg_SiR_EvDeltaE_" + this_particle + "_2sigma";
    TCutG* tCutG = (TCutG*) cut_file->Get(tcutgname.c_str());
    tCutG->SetLineWidth(2);
    tCutG->SetLineColor(colours[i_particle]);
    tCutG->Draw("SAME");

    std::string leglabel = this_particle + " selection (#pm2#sigma)";
    leg->AddEntry(tCutG, leglabel.c_str(), "l");
  }
  leg->Draw();

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_Si16bDataset_TwoLayer_EvdE_Rotated";
    std::string pngname = savename + ".png";
    c_EvdE->SaveAs(pngname.c_str());
  }
}
