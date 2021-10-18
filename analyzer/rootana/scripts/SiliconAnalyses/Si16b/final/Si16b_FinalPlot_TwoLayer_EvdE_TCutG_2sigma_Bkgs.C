void Si16b_FinalPlot_TwoLayer_EvdE_TCutG_2sigma_Bkgs(std::string savedir = "", std::ostream& numbers_file = std::cout) {

  std::string infilename = "~/data/results/Si16b/plots_newPP_geq0TgtPulse_1.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  // Construct the sum of all selected particles
  TH2F* hSumEvdE = 0;
  TH1F* hSumTime = 0;
  const int n_particles = 4;
  Int_t colours[n_particles] = {kRed, kCyan, kMagenta, kSpring};
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string this_particle = particles[i_particle];

    std::string dirname = this_particle + "_2sig_SiR_timecut0_10000ns_layerCoinc500ns";
    std::string inhistname = dirname + "/hEvdE_TwoLayer_12not3";
    TH2F* hist = (TH2F*) infile->Get(inhistname.c_str());
    if (i_particle==0) {
      hSumEvdE = (TH2F*) hist->Clone("hSumEvdE");
    }
    else {
      hSumEvdE->Add(hist);
    }

    inhistname = dirname + "/hThickTime_TwoLayer_12not3";
    TH1F* hTime = (TH1F*) infile->Get(inhistname.c_str());
    if (i_particle==0) {
      hSumTime = (TH1F*) hTime->Clone("hSumTime");
    }
    else {
      hSumTime->Add(hTime);
    }
  }

  std::string dirname = "all_SiR_timecut0_10000ns_layerCoinc500ns";
  std::string inhistname = dirname + "/hEvdE_TwoLayer_12not3";
  TH2F* hEvdEAll = (TH2F*) infile->Get(inhistname.c_str());

  TCanvas* c_EvdE = new TCanvas();
  TH2F* hBkgEvdE = (TH2F*) hEvdEAll->Clone("hBkgEvdE");
  hBkgEvdE->Add(hSumEvdE, -1);
  hBkgEvdE->SetStats(false);
  hBkgEvdE->GetXaxis()->SetRangeUser(0, 20);
  hBkgEvdE->GetYaxis()->SetRangeUser(0, 20);
  hBkgEvdE->GetYaxis()->SetTitleOffset(0.9);
  hBkgEvdE->GetXaxis()->SetTitleOffset(0.9);
  hBkgEvdE->Draw("COLZ");

  gStyle->SetOptFit(1);
  TCanvas* c_time = new TCanvas();
  c_time->SetLogy();
  inhistname = dirname + "/hThickTime_TwoLayer_12not3";
  TH1F* hTimeAll = (TH1F*) infile->Get(inhistname.c_str());
  TH1F* hBkgTime = (TH1F*) hTimeAll->Clone("hBkgTime");
  hBkgTime->Add(hSumTime, -1);
  hBkgTime->Rebin(10);
  //  std::cout << "hBkgEvdE = " << hBkgEvdE->Integral() << " hTime Integral = " << hBkgTime->Integral() << std::endl;
  //  std::cout << "hBkgTime: " << hBkgTime->GetXaxis()->GetXmin() << " --> " << hBkgTime->GetXaxis()->GetXmax() << " (" << hBkgTime->GetNbinsX() << " bins)" << std::endl;
  //  std::cout << "hSumTime: " << hSumTime->GetXaxis()->GetXmin() << " --> " << hSumTime->GetXaxis()->GetXmax() << " (" << hSumTime->GetNbinsX() << " bins)" << std::endl;
  hBkgTime->GetXaxis()->SetRangeUser(0, 10000);
  hBkgTime->SetStats(true);
  TF1* fit_fn = new TF1("fit_fn", "[0]*TMath::Exp(-x/[1]) + [2]", 0, 10000);
  fit_fn->SetParName(0, "N_0");
  fit_fn->SetParName(1, "#tau");
  fit_fn->SetParName(2, "offset");
  fit_fn->SetParameters(1000, 100, 10);
  hBkgTime->Fit(fit_fn, "RL");
  hBkgTime->Draw("HIST E");
  hBkgTime->GetFunction("fit_fn")->Draw("LSAME");

  // if (savedir != "") {
  //   std::string savename = savedir + "AlCapData_Si16bDataset_TwoLayer_EvdE_TCutG_2sigma_Bkgs";
  //   std::string pngname = savename + ".png";
  //   c_EvdE->SaveAs(pngname.c_str());
    
  //   savename = savedir + "AlCapData_Si16bDataset_TwoLayer_EvdE_incAlpha_TCutG_2sigma_Bkgs";
  //   pngname = savename + ".png";
  //   c_EvdE_incAlpha->SaveAs(pngname.c_str());
  // }
}
