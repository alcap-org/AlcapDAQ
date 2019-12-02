void Al50_FinalPlot_MuonSpot_EvdE() {

  // using John's data  
  std::string infilename = "~jrquirk/data/R15b/Al50/psel.3layer.pidpow2.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  const int n_arms = 2;
  std::string arms[n_arms] = {"R", "L"};
  std::string histtitles[n_arms] = {"Al50 Dataset, Right Arm (SiR1 && SiR2 && !SiR3)",
				    "Al50 Dataset, Left Arm (SiL1 && SiL3)"};
  
  const int n_particles = 5;
  std::string particles[n_particles] = {"P", "D", "T", "A", "U"};
  
  double min_x = 0;
  double max_x = 25000;
  double x_width = 10;
  int n_x_bins = (max_x - min_x) / x_width;

  double min_y = 0;
  double max_y = 25000;
  double y_width = 10;
  int n_y_bins = (max_y - min_y) / y_width;

  std::string draw = "e1:e1+e2";
  std::string cut = "e3<=0 && abs(t1-t2)<200";
  
  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    std::string arm = arms[i_arm];
    std::string histtitle = histtitles[i_arm];
    
    std::string canvasname = "c_EvdE_Si" + arm;
    TCanvas* c_EvdE = new TCanvas(canvasname.c_str(), canvasname.c_str());

    std::string histname = "hEvdE_Si" + arm;
    TH2F* hEvdE = new TH2F(histname.c_str(), histtitle.c_str(), n_x_bins,min_x,max_x, n_y_bins,min_y,max_y);
    hEvdE->SetXTitle("E_{1} + E_{2} [keV]");
    hEvdE->SetYTitle("E_{1} [keV]");

    for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
      std::string particle = particles[i_particle];
      std::string intreename = "PID_" + arm + particle;

      TTree* tree = (TTree*) infile->Get(intreename.c_str());

      std::string drawcmd = draw + ">>+" + histname;
      std::string cutcmd = "(" + cut + ")";
      tree->Draw(drawcmd.c_str(), cutcmd.c_str(), "goff");
    }

    hEvdE->SetStats(false);
    hEvdE->GetXaxis()->SetRangeUser(0, 20000);
    hEvdE->GetYaxis()->SetRangeUser(0, 3000);
    //  hEvdE->GetYaxis()->SetRangeUser(0, 10000);
    hEvdE->Draw("COLZ");

    TLatex* latex = new TLatex();
    latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
  }
}
