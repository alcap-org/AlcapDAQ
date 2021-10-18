void Si16b_FinalPlot_TwoLayer_EvdE_muInDet(std::string outdir = "") {

  std::string infilename = "~/data/mc/Si16b/MC_raw_spectra10404_muInDet-proton-SiR2.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  std::string inhistname = "hEvdE_SiR";
  //  std::string inhistname = dirname + "/hEvdE_TwoLayer_123";
  TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
  hEvdE->SetStats(false);
  //  hEvdE->GetXaxis()->SetRangeUser(0, 18000);
  //  hEvdE->GetYaxis()->SetRangeUser(0, 18000);
  hEvdE->GetXaxis()->SetTitleOffset(1.0);
  hEvdE->GetYaxis()->SetTitleOffset(1.0);
  hEvdE->GetXaxis()->SetLabelSize(0.04);
  hEvdE->GetYaxis()->SetLabelSize(0.04);


  TCanvas* c_EvdE = new TCanvas("c_EvdE", "c_EvdE");
  hEvdE->Draw("COLZ");

  // Draw the particle cuts
  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  Int_t colours[n_particles] = {kRed, kCyan, kMagenta, kSpring};
  
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string this_particle = particles[i_particle];
  
    std::string infile2name = "~/data/results/Si16b/plots_newPP_geq1TgtPulse_1.root";
    TFile* infile2 = new TFile(infile2name.c_str(), "READ");

    std::string tcutgname = this_particle + "_2sig_SiR_timecut0_10000ns_layerCoinc500ns/r_hLg_SiR_EvDeltaE_" + this_particle + "_2sigma_cutoff";
    TCutG* tCutG = (TCutG*) infile2->Get(tcutgname.c_str());
    if (!tCutG) {
      // try the other
      tcutgname = this_particle + "_SiR_timecut0_10000ns_layerCoinc/" + this_particle + "_cut_two_layer";
      tCutG = (TCutG*) infile2->Get(tcutgname.c_str());
      if (!tCutG) {
	std::cout << "Error: Can't get " << tcutgname << std::endl;
	continue;
      }
    }
    for (int i_point = 0; i_point < tCutG->GetN(); ++i_point) {
      double x = *(tCutG->GetX()+i_point);
      double y = *(tCutG->GetY()+i_point);
      tCutG->SetPoint(i_point, x*1000, y*1000);
    }
    tCutG->SetLineWidth(2);
    tCutG->SetLineColor(colours[i_particle]);
    tCutG->Draw("SAME");

    double integral = 0;
    for (int i_bin = 1; i_bin <= hEvdE->GetXaxis()->GetNbins(); ++i_bin) {
      for (int j_bin = 1; j_bin <= hEvdE->GetYaxis()->GetNbins(); ++j_bin) {
	double x = hEvdE->GetXaxis()->GetBinCenter(i_bin);
	double y = hEvdE->GetYaxis()->GetBinCenter(j_bin);
	if (tCutG->IsInside(x, y)) {
	  integral += hEvdE->GetBinContent(i_bin,j_bin);
	}
      }
    }
    std::cout << this_particle << " TCutG has " << integral << " entries" << std::endl;
  }
}
