void Ti50_FinalPlot_TwoLayer_EvdE() {

  std::string infilename = "~/data/results/Ti50/plots.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  std::string arm = "Right";
  std::string dirname;
  if (arm == "Right") {
    dirname = "all_SiR_timecut"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  }
  else if (arm == "Left") {
    dirname = "all_SiL_timecut"; std::string layer1 = "SiL1"; std::string layer2 = "SiL3"; std::string layer3 = "SiL2";
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
    std::string tcutgname = this_particle + "_SiR_timecut/" + this_particle + "_cut_two_layer";
    TCutG* tCutG = (TCutG*) infile->Get(tcutgname.c_str());
    if (!tCutG) {
      std::cout << "Error: Can't get " << tcutgname << std::endl;
      return;
    }
    tCutG->SetLineWidth(2);
    tCutG->SetLineColor(kRed);
    tCutG->Draw("SAME");
  }
}
