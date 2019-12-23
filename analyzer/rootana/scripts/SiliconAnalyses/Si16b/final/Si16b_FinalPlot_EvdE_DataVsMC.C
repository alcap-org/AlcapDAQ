void Si16b_FinalPlot_EvdE_DataVsMC() {

  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  Int_t colours[n_particles] = {kRed, kCyan, kMagenta, kSpring};
  Int_t notstopped_colours[n_particles] = {kBlue, kYellow-9, kViolet-6, kSpring-6};
  TH2F* hEvdEs[n_particles] = {0};
  TH2F* hEvdEs_notstopped[n_particles] = {0};

  //  std::string layers = "TwoLayer";
  std::string layers = "ThreeLayer";
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    std::string infilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5_" + particle + "_1000keVBins.root";
    TFile* infile = new TFile(infilename.c_str(), "READ");
    if (infile->IsZombie()) {
      continue;
    }

    std::string basehistname = "SiR_";
    if (layers=="TwoLayer") {
      basehistname += "h";
    }
    basehistname += "EvdE_" + layers;
    std::string inhistname = basehistname + "_stopped";
    TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
    if (!hEvdE) {
      std::cout << "Problem getting " << inhistname << std::endl;
      return;
    }
    hEvdE->SetStats(false);
    hEvdE->SetMarkerColor(colours[i_particle]);
    if (layers=="TwoLayer") {
      hEvdE->GetXaxis()->SetRangeUser(0, 20000);
      hEvdE->GetYaxis()->SetRangeUser(0, 3000);
      //      hEvdE->GetYaxis()->SetRangeUser(0, 10000);
    }
    else if (layers=="ThreeLayer") {
      hEvdE->GetXaxis()->SetRangeUser(0, 25000);
      hEvdE->GetYaxis()->SetRangeUser(0, 20000);
    }
    
    std::string histtitle = "Si16b Dataset, Data vs MC";
    hEvdE->SetTitle(histtitle.c_str());
    hEvdE->Draw("P SAME");

    inhistname = basehistname + "_notstopped";
    TH2F* hEvdE_notstopped = (TH2F*) infile->Get(inhistname.c_str());
    if (!hEvdE_notstopped) {
      std::cout << "Problem getting " << inhistname << std::endl;
      return;
    }
    hEvdE_notstopped->SetStats(false);
    hEvdE_notstopped->SetMarkerColor(notstopped_colours[i_particle]);
    if (layers=="TwoLayer") {
      hEvdE_notstopped->GetXaxis()->SetRangeUser(0, 20000);
      hEvdE_notstopped->GetYaxis()->SetRangeUser(0, 3000);
      //      hEvdE_notstopped->GetYaxis()->SetRangeUser(0, 10000);
    }
    else if (layers=="ThreeLayer") {
      hEvdE_notstopped->GetXaxis()->SetRangeUser(0, 25000);
      hEvdE_notstopped->GetYaxis()->SetRangeUser(0, 20000);
    }      
    hEvdE_notstopped->SetTitle(histtitle.c_str());
    //    hEvdE_notstopped->Draw("P SAME");
  }

  std::string infilename = "~/data/results/Si16b/plots_newPP_geq1TgtPulse.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  std::string dirname = "all_SiR_timecut0_10000ns_layerCoinc";
  std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  std::string inhistname = dirname + "/hEvdE_" + layers + "_12";
  if (layers == "TwoLayer") {
    inhistname += "not3";
  }
  else if (layers=="ThreeLayer") {
    inhistname += "3";
  }
  TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
  std::string histtitle = "Si16b Dataset, Right Arm (" + layer1 + " && " + layer2 + " && !" + layer3 + " && SiL1)";
  hEvdE->SetTitle(histtitle.c_str());
  hEvdE->SetStats(false);
  //  hEvdE->Rebin2D(5, 5);
  //  hEvdE->GetXaxis()->SetRangeUser(0, 20000);
  //  hEvdE->GetYaxis()->SetRangeUser(0, 3000);
  //  hEvdE->GetYaxis()->SetRangeUser(0, 10000);
  hEvdE->Draw("P SAME");

}
