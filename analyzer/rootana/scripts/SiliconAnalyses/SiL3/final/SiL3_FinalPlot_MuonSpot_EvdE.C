void SiL3_FinalPlot_MuonSpot_EvdE() {

  const int n_plots = 3;
  std::string filenames[n_plots] = {"~/data/results/SiL3/plots_geq0TgtPulse.root", "~/data/results/SiL3/plots_geq1TgtPulse.root", "~/data/results/SiL3/plots_geq0TgtPulse.root"};
  std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  std::string dirnames[n_plots] = {"all_SiR", "all_SiR", "all_SiR_timecut"}; 
  std::string histtitles[n_plots] = {"SiL3 Dataset, Right Arm (" + layer1 + " && " + layer2 + " && #geq 0 Target Hits)",
				     "SiL3 Dataset, Right Arm (" + layer1 + " && " + layer2 + " && #geq 1 Target Hit)",
				     "SiL3 Dataset, Right Arm (" + layer1 + " && " + layer2 + " && #geq 0 Target Hits && t > 200 ns)"};

  std::stringstream canvasname;
  for (int i_plot = 0; i_plot < n_plots; ++i_plot) {
    std::string infilename = filenames[i_plot];
    std::string histtitle = histtitles[i_plot];
    std::string dirname = dirnames[i_plot];

    canvasname.str("");
    canvasname << "c" << i_plot;
    TCanvas* c = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());

    TFile* infile = new TFile(infilename.c_str(), "READ");
    if (infile->IsZombie()) {
      std::cout << "ERROR: Can't open file " << infilename << std::endl;
      return;
    }
    
    std::string inhistname = dirname + "/hEvdE_TwoLayer_12";
    TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
    if (!hEvdE) {
      std::cout << "ERROR: Can't find histogram " << inhistname << std::endl;
      return;
    }
    hEvdE->SetTitle(histtitle.c_str());
    hEvdE->SetStats(false);
    hEvdE->GetXaxis()->SetRangeUser(0, 20000);
    hEvdE->GetYaxis()->SetRangeUser(0, 3000);
    hEvdE->Draw("COLZ");
    
  
    TLatex* latex = new TLatex();
    latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
  }

  
  /*
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
  */
}
