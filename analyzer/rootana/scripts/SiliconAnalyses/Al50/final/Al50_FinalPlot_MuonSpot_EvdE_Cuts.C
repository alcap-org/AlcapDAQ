void Al50_FinalPlot_MuonSpot_EvdE_Cuts() {

  TFile* cutfile = new TFile("~/data/results/Si16b/plots_newPP.root", "READ");
  const int n_cuts = 2;
  std::string cutnames[n_cuts] = {"lower", "upper"};
  TCutG* cuts[n_cuts] = {0};
  for (int i_cut = 0; i_cut < n_cuts; ++i_cut) {

    std::string cutname = "muspot_" + cutnames[i_cut] + "_SiR/" + cutnames[i_cut] + "_two_layer";
    std::cout << cutname << std::endl;
    cuts[i_cut] = (TCutG*) cutfile->Get(cutname.c_str());    
    //    cut->Draw("LSAME");
  }

  
  // using John's data  
  std::string infilename = "~jrquirk/data/R15b/Al50/psel.3layer.pidpow2.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  const int n_arms = 2;
  std::string arms[n_arms] = {"R", "L"};
  std::string histtitles[n_arms] = {"Al50 Dataset, Right Arm (SiR1 && SiR2 && !SiR3)",
				    "Al50 Dataset, Left Arm (SiL1 && SiL3)"};
  
  const int n_particles = 1;
  std::string particles[n_particles] = {"U"};
  
  double min_x = 0;
  double max_x = 25000;
  double x_width = 10;
  int n_x_bins = (max_x - min_x) / x_width;

  double min_y = 0;
  double max_y = 25000;
  double y_width = 10;
  int n_y_bins = (max_y - min_y) / y_width;

  double min_time = -2000;
  double max_time = 2000;
  double time_width = 10;
  int n_time_bins = (max_time - min_time) / time_width;

  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    std::string arm = arms[i_arm];
    std::string histtitle = histtitles[i_arm];

    TH2F* hEvdEs[n_cuts] = {0};
    TH1F* hThinTimes[n_cuts] = {0};
    for (int i_cut = 0; i_cut < n_cuts; ++i_cut) {
      std::string histname = "hEvdE_Si" + arm + "_" + cutnames[i_cut];
      TH2F* hEvdE = new TH2F(histname.c_str(), histtitle.c_str(), n_x_bins,min_x,max_x, n_y_bins,min_y,max_y);
      hEvdE->SetXTitle("E_{1} + E_{2} [keV]");
      hEvdE->SetYTitle("E_{1} [keV]");
      hEvdEs[i_cut] = hEvdE;

      histname = "hThinTime_Si" + arm + "_" + cutnames[i_cut];
      TH1F* hThinTime = new TH1F(histname.c_str(), histtitle.c_str(), n_time_bins,min_time,max_time);
      hThinTime->SetXTitle("t_{1} [ns]");
      hThinTime->SetLineColor(cuts[i_cut]->GetLineColor());
      hThinTimes[i_cut] = hThinTime;
    }


    for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
      std::string particle = particles[i_particle];
      std::string intreename = "PID_" + arm + particle;

      TTree* tree = (TTree*) infile->Get(intreename.c_str());

      double e1 = 0; tree->SetBranchAddress("e1", &e1);
      double e2 = 0; tree->SetBranchAddress("e2", &e2);
      double e3 = 0; tree->SetBranchAddress("e3", &e3);
      double t1 = 0; tree->SetBranchAddress("t1", &t1);
      double t2 = 0; tree->SetBranchAddress("t2", &t2);
      int n_entries = tree->GetEntries();
      for (int i_entry = 0; i_entry < n_entries; ++i_entry) {
	tree->GetEntry(i_entry);

	if (i_entry % 10000 == 0) {
	  std::cout << i_entry << " / " << n_entries << std::endl;
	}

	for (int i_cut = 0; i_cut < n_cuts; ++i_cut) {
	  TCutG* cut = cuts[i_cut];
	  if (cut->IsInside(e1+e2, e1)) {
	    if (e3<=0 && std::fabs(t2-t1)<200) {
	      hEvdEs[i_cut]->Fill(e1+e2, e1);
	      hThinTimes[i_cut]->Fill(t1);
	    }
	  }
	}
      }
    }

    //    std::string canvasname = "c_Si" + arm;
    //    TCanvas* c_ = new TCanvas(canvasname.c_str(), canvasname.c_str());
    for (int i_cut = 0; i_cut < n_cuts; ++i_cut) {
      std::string canvasname = "c_EvdE_Si" + arm + "_" + cutnames[i_cut];
      TCanvas* c_EvdE = new TCanvas(canvasname.c_str(), canvasname.c_str());
      TH2F* hEvdE = hEvdEs[i_cut];
      hEvdE->SetStats(false);
      hEvdE->GetXaxis()->SetRangeUser(0, 10000);
      hEvdE->GetYaxis()->SetRangeUser(0, 2000);
      hEvdE->Draw("COLZ");

      TLatex* latex = new TLatex();
      latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
      
      //      hThinTimes[i_cut]->Draw("HIST E SAME");
    }
    //    TLatex* latex = new TLatex();
    //    latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
  }
}
