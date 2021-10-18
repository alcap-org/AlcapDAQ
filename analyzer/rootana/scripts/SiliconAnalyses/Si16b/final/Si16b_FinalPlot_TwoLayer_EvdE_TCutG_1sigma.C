void Si16b_FinalPlot_TwoLayer_EvdE_TCutG_1sigma(std::string savedir = "", std::ostream& numbers_file = std::cout) {

  std::string infilename = "~/data/results/Si16b/plots_newPP_geq1TgtPulse_1.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  std::string dirname = "all_SiR_timecut0_10000ns_layerCoinc500ns";
  //  std::string dirname = "all_SiR_timecut400_10000ns_layerCoinc_wSiL1_14Coinc";
  std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  //  std::string dirname = "all_SiR_nolayercoinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";

  std::string inhistname = dirname + "/hEvdE_TwoLayer_12not3";
  //  std::string inhistname = dirname + "/hEvdE_TwoLayer_123";
  TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
  std::string histtitle = "Si16b Dataset, Right Arm (" + layer1 + " && " + layer2 + " && !" + layer3 + " && SiL1)";
  hEvdE->SetTitle(histtitle.c_str());
  hEvdE->SetStats(false);
  hEvdE->GetXaxis()->SetRangeUser(0, 20);
  hEvdE->GetYaxis()->SetRangeUser(0, 3);
  hEvdE->GetYaxis()->SetTitleOffset(0.9);
  hEvdE->GetXaxis()->SetTitleOffset(0.9);


  TCanvas* c_EvdE = new TCanvas("c_EvdE", "c_EvdE");
  hEvdE->Draw("COLZ");
  TLatex* latex = new TLatex();
  latex->SetTextColor(kRed);
  latex->DrawLatex(1.000, 1.200, "#mu^{-}");
  latex->DrawLatex(1.600, 2.100, "p");
  latex->DrawLatex(2.200, 2.500, "d");
  latex->DrawLatex(2.800, 2.900, "t");
  latex->DrawLatex(8.000, 7.500, "#alpha");

  alcaphistogram(hEvdE);
  alcapPreliminary(hEvdE);
  alcaplabel("1#sigma PID", hEvdE);
  hEvdE->SetDrawOption("COLZ");

  TCanvas* c_EvdE_incAlpha = new TCanvas("c_EvdE_incAlpha", "c_EvdE_incAlpha");
  TH2F* hEvdE_incAlpha = (TH2F*) hEvdE->Clone("hEvdE_incAlpha");
  hEvdE_incAlpha->Rebin2D(5, 5);
  hEvdE_incAlpha->GetXaxis()->SetRangeUser(0, 20);
  hEvdE_incAlpha->GetYaxis()->SetRangeUser(0, 10);
  hEvdE_incAlpha->Draw("COLZ");

  latex->SetTextColor(kRed);
  latex->DrawLatex(1.000, 1.200, "#mu^{-}");
  latex->DrawLatex(1.600, 2.100, "p");
  latex->DrawLatex(2.200, 2.500, "d");
  latex->DrawLatex(2.800, 2.900, "t");
  latex->DrawLatex(8.000, 7.500, "#alpha");

  alcaphistogram(hEvdE_incAlpha);
  alcapPreliminary(hEvdE_incAlpha);
  alcaplabel("1#sigma PID", hEvdE_incAlpha);
  hEvdE_incAlpha->SetDrawOption("COLZ");

  const int n_canvases = 2;
  TCanvas* canvases[n_canvases] = {c_EvdE, c_EvdE_incAlpha};

  for (int i_canvas = 0; i_canvas < n_canvases; ++i_canvas) {
    canvases[i_canvas]->cd();
    // Draw the particle cuts
    const int n_particles = 4;
    std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
    for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
      std::string this_particle = particles[i_particle];
      
      std::string tcutgname = this_particle + "_1sig_SiR_timecut0_10000ns_layerCoinc500ns/r_hLg_SiR_EvDeltaE_" + this_particle + "_1sigma_cutoff";
      TCutG* tCutG = (TCutG*) infile->Get(tcutgname.c_str());
      if (!tCutG) {
	// try the other
	tcutgname = this_particle + "_1sig_SiR_timecut0_10000ns_layerCoinc500ns/" + this_particle + "_cut_two_layer";
	tCutG = (TCutG*) infile->Get(tcutgname.c_str());
	if (!tCutG) {
	  std::cout << "Error: Can't get " << tcutgname << std::endl;
	  continue;
	}
      }
      tCutG->SetLineWidth(2);
      tCutG->SetLineColor(kRed);
      tCutG->Draw("SAME");
    }
  }

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_Si16bDataset_TwoLayer_EvdE_TCutG_1sigma";
    std::string pngname = savename + ".png";
    c_EvdE->SaveAs(pngname.c_str());
    
    savename = savedir + "AlCapData_Si16bDataset_TwoLayer_EvdE_incAlpha_TCutG_1sigma";
    pngname = savename + ".png";
    c_EvdE_incAlpha->SaveAs(pngname.c_str());
  }
}
