void Ti50_FinalPlot_TwoLayer_EvdE() {

  std::string infilename = "~/data/results/Ti50/plots_newPP_3sigma_layerCoinc500ns.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  std::string layer1, layer2, layer3;
  std::string arm = "Right";
  //  std::string arm = "Left";
  std::string dirname;
  if (arm == "Right") {
    dirname = "all_SiR_timecut400_10000ns_layerCoinc"; layer1 = "SiR1"; layer2 = "SiR2"; layer3 = "SiR3";
  }
  else if (arm == "Left") {
    dirname = "all_SiL_timecut400_10000ns_layerCoinc"; layer1 = "SiL1"; layer2 = "SiL3"; layer3 = "SiL2";
  }
  else {
    std::cout << "Arm can be either Right or Left. Not " << arm << std::endl;
    return;
  }

  TCanvas* c_EvdE = new TCanvas("c_EvdE", "c_EvdE");
  std::string inhistname = dirname + "/hEvdE_TwoLayer_12not3";
  TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
  std::string histtitle;
  if (arm == "Right") {
    histtitle = "Ti50 Dataset, Right Arm (" + layer1 + " && " + layer2 + " && !" + layer3 + ")";
  }
  else if (arm == "Left") {
    histtitle = "Ti50 Dataset, Left Arm (" + layer1 + " && " + layer2 + ")";
  }

  hEvdE->SetTitle(histtitle.c_str());
  hEvdE->SetStats(false);
  hEvdE->GetXaxis()->SetRangeUser(0, 20000);
  hEvdE->GetYaxis()->SetRangeUser(0, 3000);
  //  hEvdE->GetYaxis()->SetRangeUser(0, 10000);
  hEvdE->Draw("COLZ");

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");

  latex->SetTextColor(kRed);
  //  latex->DrawLatex(1000, 1200, "#mu^{-}");
  latex->DrawLatex(1600, 2100, "p");
  latex->DrawLatex(2200, 2500, "d");
  latex->DrawLatex(2800, 2900, "t");
  

  // Draw the particle cuts
  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string this_particle = particles[i_particle];
    
    c_EvdE->cd();
    std::string tcutgname = this_particle + "_SiR_timecut0_10000ns_layerCoinc/r_hLg_SiR_EvDeltaE_" + this_particle + "_3sigma_keV";
    TCutG* tCutG = (TCutG*) infile->Get(tcutgname.c_str());
    if (!tCutG) {
      // try the other
      tcutgname = this_particle + "_SiR_timecut/" + this_particle + "_cut_two_layer";
      tCutG = (TCutG*) infile->Get(tcutgname.c_str());
      if (!tCutG) {
	std::cout << "Error: Can't get " << tcutgname << std::endl;
	return;
      }
    }
    tCutG->SetLineWidth(2);
    tCutG->SetLineColor(kRed);
    tCutG->Draw("SAME");
  }
}
