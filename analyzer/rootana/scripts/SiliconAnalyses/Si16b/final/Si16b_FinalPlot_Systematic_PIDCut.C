void Si16b_FinalPlot_Systematic_PIDCut(std::string savedir = "") {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
    
  std::string filename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_1.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  std::string Particles[n_particles] = {"Proton", "Deuteron", "Triton", "Alpha"};
  
  const int n_settings = 3;
  int sigmas[n_settings] = {3, 2, 1};
  Int_t colours[n_settings] = {kBlack, kRed, kBlue};
  std::string leglabels[n_settings] = {};//"1 Sigma", "2 Sigmas"};

  TH1F* hSpectra[n_particles][n_settings] = {{0}, {0}, {0}, {0}};
  //  TH1F* hSpectra[n_particles][n_settings] = {{0}};
  
  TLegend* leg = new TLegend(0.60,0.65,0.85,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  TLegend* leg2 = new TLegend(0.25,0.15,0.35,0.30);
  leg2->SetBorderSize(0);
  leg2->SetTextSize(0.035);
  leg2->SetFillColor(kWhite);

  int rebin_factor = 1;
  double x_max = 25;
  std::stringstream leglabel;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    std::string Particle = Particles[i_particle];

    TCanvas* c_all_spectra = new TCanvas();
    
    for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
      std::string setting = "PID" + std::to_string(sigmas[i_setting]) + "sigma";
      Int_t i_colour = colours[i_setting];
      leglabels[i_setting] = std::to_string(sigmas[i_setting]) + "#sigma";
    
      std::string i_dirname = "FinalNormalisation_" + particle + "_TCutG_2sig_layerCoinc500ns_tGT0ns_" + setting + "_SystPlot";
      std::string i_histname = i_dirname + "/syst_hist";

      std::cout << i_dirname << std::endl;
      TH1F* spectrum = (TH1F*) file->Get(i_histname.c_str());
      if (!spectrum) {
	std::cout << "Error: Problem getting spectrum " << i_histname << std::endl;
	return;
      }
      spectrum->Sumw2();

      spectrum->SetTitle("Si16b Dataset, PID Cut");
      spectrum->Rebin(rebin_factor);
      spectrum->Scale(1.0/rebin_factor);
      spectrum->SetStats(false);
      spectrum->SetLineColor(i_colour);
      spectrum->GetXaxis()->SetRangeUser(0,x_max);
      spectrum->GetXaxis()->SetTitle("Energy [keV]");
      spectrum->GetXaxis()->SetTitleOffset(0.9);
      spectrum->Draw("HIST E SAMES");

      alcaphistogram(spectrum);
      if (i_setting == 0) {
	spectrum->SetDrawOption("HIST E1");
	alcapPreliminary(spectrum);
      }
      else {
	spectrum->SetDrawOption("HIST E1 SAME");
      }
      
      leglabel.str("");
      leglabel << leglabels[i_setting];
      std::cout << leglabel.str() << std::endl;
      if (i_particle == 0) {
	leg->AddEntry(spectrum, leglabel.str().c_str(), "l");
      }
    }

    leg->Draw();
       

    TCanvas* c_all_systs = new TCanvas();
    for (int i_setting = n_settings-1; i_setting >= 1; --i_setting) {

      std::string setting = "PID" + std::to_string(sigmas[i_setting]) + "sigma";
      Int_t i_colour = colours[i_setting];
      std::string i_dirname = "FinalNormalisation_" + particle + "_TCutG_2sig_layerCoinc500ns_tGT0ns_" + setting + "_SystPlot";
      std::string i_histname = i_dirname + "/hSystematic";

      std::cout << i_dirname << std::endl;
      TH1F* hSyst = (TH1F*) file->Get(i_histname.c_str());
      if (!hSyst) {
	std::cout << "Error: Problem getting hSyst " << i_histname << std::endl;
	return;
      }
      
      hSyst->SetLineColor(i_colour);
      hSyst->GetXaxis()->SetRangeUser(0, x_max);
      hSyst->Draw("HIST E SAME");

      alcaphistogram(hSyst);
      if (i_setting == n_settings-1) {
	hSyst->SetDrawOption("HIST E1");
	alcapPreliminary(hSyst);
      }
      else {
	hSyst->SetDrawOption("HIST E1 SAME");
      }
      
      leglabel.str("");
      leglabel << leglabels[i_setting];
      std::cout << leglabel.str() << std::endl;
      if (i_particle == 0) {
	leg2->AddEntry(hSyst, leglabel.str().c_str(), "l");
      }
    }

    leg2->Draw();      
    
    if (savedir != "") {
      std::string savename = savedir + "AlCapData_Si16bDataset_NormalisedSpectrum_PIDCut_" + Particle;
      std::string pdfname = savename + ".pdf";
      c_all_spectra->SaveAs(pdfname.c_str());
      std::string pngname = savename + ".png";
      c_all_spectra->SaveAs(pngname.c_str());

      savename = savedir + "AlCapData_Si16bDataset_Systematic_PIDCut_" + Particle;
      pdfname = savename + ".pdf";
      c_all_systs->SaveAs(pdfname.c_str());
      pngname = savename + ".png";
      c_all_systs->SaveAs(pngname.c_str());
    }

  }
}
