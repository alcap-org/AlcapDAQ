void Si16b_FinalPlot_TwoLayer_EvdE_NoLayerCoinc() {

  std::string infilename = "~/data/results/Si16b/plots_newPP.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  std::string dirname = "all_SiR_nolayercoinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";

  TCanvas* c_EvdE = new TCanvas("c_EvdE", "c_EvdE");
  std::string inhistname = dirname + "/hEvdE_TwoLayer_12not3";
  TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
  std::string histtitle = "Si16b Dataset, Right Arm (" + layer1 + " && " + layer2 + " && !" + layer3 + ")";
  hEvdE->SetTitle(histtitle.c_str());
  hEvdE->SetStats(false);
  hEvdE->GetXaxis()->SetRangeUser(0, 20000);
  //  hEvdE->GetYaxis()->SetRangeUser(0, 3000);
  hEvdE->GetYaxis()->SetRangeUser(0, 10000);
  hEvdE->Draw("COLZ");

  TCanvas* c_EvdE_Zoom = new TCanvas("c_EvdE_Zoom", "c_EvdE_Zoom");
  TH1F* hEvdE_Zoom = hEvdE->Clone("hEvdE_Zoom");
  hEvdE_Zoom->GetXaxis()->SetRangeUser(0, 20000);
  hEvdE_Zoom->GetYaxis()->SetRangeUser(0, 3000);
  hEvdE_Zoom->Draw("COLZ");


  const int n_canvases = 2;
  TCanvas* canvases[n_canvases] = {c_EvdE, c_EvdE_Zoom};

  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  TCutG* tCutGs[n_particles] = {0, 0, 0, 0};

  for (int i_canvas = 0; i_canvas < n_canvases; ++i_canvas) {

    canvases[i_canvas]->cd();
    
    TLatex* latex = new TLatex();
    latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
    
    latex->SetTextColor(kRed);
    //  latex->DrawLatex(1000, 1200, "#mu^{-}");
    latex->DrawLatex(1600, 2100, "p");
    latex->DrawLatex(2200, 2500, "d");
    latex->DrawLatex(2800, 2900, "t");
    latex->DrawLatex(6500, 7000, "#alpha");
    
    // Draw the particle cuts
    for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
      std::string this_particle = particles[i_particle];
      
      if (tCutGs[i_particle] == 0) {
	std::string tcutgname = this_particle + "_SiR_nolayercoinc/" + this_particle + "_cut_two_layer";
	tCutGs[i_particle] = (TCutG*) infile->Get(tcutgname.c_str());
	if (!tCutGs[i_particle]) {
	  std::cout << "Error: Can't get " << tcutgname << std::endl;
	  return;
	}
	tCutGs[i_particle]->SetLineWidth(2);
	tCutGs[i_particle]->SetLineColor(kRed);
      }
      tCutGs[i_particle]->Draw("SAME");
    }
  }
}
