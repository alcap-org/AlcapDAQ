void Si16b_FinalPlot_Systematic_Miscalibration(std::string savedir = "") {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
    
  std::string filename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_2.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  std::string Particles[n_particles] = {"Proton", "Deuteron", "Triton", "Alpha"};
  
  const int n_settings = 3;
  std::string positions[n_settings] = {"", "Down", "Up"};
  Int_t colours[n_settings] = {kBlack, kRed, kBlue};
  std::string leglabels[n_settings] = {"nominal", "k = 0.975", "k = 1.025"};

  //  TH1F* hSpectra[n_particles][n_settings] = {{0}, {0}, {0}, {0}};
  TH1F* hSpectra[n_particles][n_settings] = {{0}};
  //  TH1F* hSpectra[n_particles][n_settings] = {{0}};
  
  TLegend* leg = new TLegend(0.65,0.65,0.85,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  TLegend* leg2 = new TLegend(0.45,0.65,0.55,0.85);
  leg2->SetBorderSize(0);
  leg2->SetTextSize(0.035);
  leg2->SetFillColor(kWhite);

  
  int rebin_factor = 1;
  double x_max = 25.000;
  std::stringstream leglabel;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    std::string Particle = Particles[i_particle];

    TCanvas* c_all_spectra = new TCanvas();
    
    for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
      std::string setting = "MisCalib" + positions[i_setting];
      Int_t i_colour = colours[i_setting];
    
      std::string i_dirname = "FinalNormalisation_" + particle + "_TCutG_2sig_layerCoinc500ns_tGT0ns_BinW500keV_" + setting;
      std::string i_histname = i_dirname + "/hNormalisedSpectrum";
      //      std::string i_dirname = "ResponseMatrix_" + particle + "_TCutG_" + setting;
      //      std::string i_histname = i_dirname + "/hInputSpectrum";
      //      std::string i_histname = i_dirname + "/hCorrectedSpectrum";

      std::cout << i_dirname << std::endl;
      TH1F* spectrum = (TH1F*) file->Get(i_histname.c_str());
      if (!spectrum) {
	std::cout << "Error: Problem getting spectrum " << i_histname << std::endl;
	return;
      }
      spectrum->Sumw2();

      spectrum->SetTitle("Si16b Dataset, Miscalibration");
      spectrum->Rebin(rebin_factor);
      spectrum->Scale(1.0/rebin_factor);
      spectrum->SetStats(false);
      spectrum->SetLineColor(i_colour);
      spectrum->GetXaxis()->SetRangeUser(0,x_max);
      spectrum->Draw("HIST E SAMES");
      spectrum->SetMaximum(27e-3);

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
      std::string setting = "MisCalib" + positions[i_setting];
      Int_t i_colour = colours[i_setting];
      std::string i_dirname = "FinalNormalisation_" + particle + "_TCutG_2sig_layerCoinc500ns_tGT0ns_BinW500keV_" + setting + "_SystPlot";
      std::string i_histname = i_dirname + "/hSystematic";
      
      std::cout << i_dirname << std::endl;
      TH1F* hSyst = (TH1F*) file->Get(i_histname.c_str());
      if (!hSyst) {
	std::cout << "Error: Problem getting hSyst " << i_histname << std::endl;
	return;
      }

      hSyst->GetXaxis()->SetRangeUser(0,x_max);
      hSyst->SetLineColor(i_colour);
      hSyst->Draw("HIST E SAME");

      if (particle == "proton") {
	for (int i_bin = 1; i_bin <= hSyst->GetNbinsX(); ++i_bin) {
	  double E = hSyst->GetBinCenter(i_bin);
	  std::cout << E << " MeV: " << hSyst->GetBinContent(i_bin) << std::endl;
	}
      }
      
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
      if (i_particle == 0) {
	leg2->AddEntry(hSyst, leglabel.str().c_str(), "l");
      }
    }
    leg2->Draw();

    if (savedir != "") {
      std::string savename = savedir + "AlCapData_Si16bDataset_NormalisedSpectrum_Miscalibrations_" + Particle;
      std::string pdfname = savename + ".pdf";
      c_all_spectra->SaveAs(pdfname.c_str());
      std::string pngname = savename + ".png";
      c_all_spectra->SaveAs(pngname.c_str());

      savename = savedir + "AlCapData_Si16bDataset_Systematic_Miscalibrations_" + Particle;
      pdfname = savename + ".pdf";
      c_all_systs->SaveAs(pdfname.c_str());
      pngname = savename + ".png";
      c_all_systs->SaveAs(pngname.c_str());
    }

  }
}
