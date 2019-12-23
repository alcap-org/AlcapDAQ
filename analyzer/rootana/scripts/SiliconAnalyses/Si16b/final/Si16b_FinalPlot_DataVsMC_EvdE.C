void Si16b_FinalPlot_DataVsMC_EvdE() {

  const int n_settings = 6;
  //  std::string histnames[n_settings] = {"Thin_wNoThick_TimeSlice-200_200/hRawSpectrum", "muonbeam/hRawSpectrum"};
  //  std::string histnames[n_settings] = {"muonbeam_thickStop/hRawSpectrum", "muonbeam_thinStop/hRawSpectrum"};
  std::string filenames[n_settings] = {"~/data/results/Si16b/plots_newPP.root",
				       "~/data/mc/Si16b/respMatrix_10M_Geom-P5_proton_1000keVBins_frontDL.root",
				       "~/data/mc/Si16b/respMatrix_10M_Geom-P5_proton_1000keVBins_frontDL.root",
				       "~/data/mc/Si16b/respMatrix_10M_Geom-P5_deuteron.root",
				       "~/data/mc/Si16b/respMatrix_10M_Geom-P5_deuteron.root",
				       "~/data/mc/Si16b/respMatrix_10M_Geom-P5_triton.root"};
  std::string leglabels[n_settings] = {"Data",
				       "MC (stopped proton)", "MC (punch-through proton)",
				       "MC (stopped deuteron)", "MC (punch-through deuteron)",
				       "MC (stopped triton)"};
  Int_t colours[n_settings] = {kBlack,
			       kRed, kBlue,
			       kCyan, kMagenta,
			       kSpring};
  //  int rebin_factors[n_settings] = {5, 5};
  //  std::string inhistnames[n_settings] = { "proton_SiR_timecut400_10000ns_layerCoinc_PSel/hEvdE_TwoLayer_12not3", "SiR_hEvdE_TwoLayer_stopped"};
  std::string inhistnames[n_settings] = { "all_SiR_timecut400_10000ns_layerCoinc/hEvdE_TwoLayer_12not3",
					  "SiR_hEvdE_TwoLayer_stopped", "SiR_hEvdE_TwoLayer_notstopped",
					  "SiR_hEvdE_TwoLayer_stopped", "SiR_hEvdE_TwoLayer_notstopped",
					  "SiR_hEvdE_TwoLayer_stopped"};
  TH2F* hEvdEs[n_settings] = {0};
  
  TLegend* leg = new TLegend(0.40,0.45,0.80,0.75);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);

  TCanvas* c = new TCanvas();
  //  c->Divide(2);
  
  std::stringstream axislabel, leglabel;
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    //  for (int i_setting = n_settings-1; i_setting >= 0; --i_setting) {
    std::string infilename = filenames[i_setting];
    TFile* infile = new TFile(infilename.c_str(), "READ");
    if (infile->IsZombie()) {
      std::cout << "ERROR: Can't find file " << infilename << std::endl;
      return;
    }

    std::string inhistname = inhistnames[i_setting];
    TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
    if (!hEvdE) {
      std::cout << "ERROR: Can't find histogram " << inhistname << std::endl;
      return;
    }

    std::string histtitle;
    histtitle = "Si16b Dataset, " + leglabels[i_setting];
    hEvdE->SetTitle(histtitle.c_str());
    hEvdE->Sumw2();
    hEvdE->SetStats(false);
    hEvdE->SetMarkerColor(colours[i_setting]);
    hEvdE->SetMarkerStyle(7);
    hEvdE->SetLineColor(colours[i_setting]);
    hEvdE->SetLineStyle(1);
    //    hEvdE->GetXaxis()->SetTitle("Energy [keV]");
    //    hEvdE->Rebin(rebin_factors[i_setting]);
    //    hEvdE->Scale(1.0 / hEvdE->Integral());
    hEvdE->GetYaxis()->SetRangeUser(0,3000);
      
    //    axislabel.str("");
    //    axislabel << "Counts / " << hEvdE->GetXaxis()->GetBinWidth(1) << " keV";
    //    hEvdE->GetYaxis()->SetTitle(axislabel.str().c_str());
    //    std::cout << "Drawing " << inhistname << std::endl;
    hEvdEs[i_setting] = hEvdE;
    //    c->cd(i_setting+1);
    //    hEvdE->Draw("P SAME");

    leglabel.str("");
    leglabel << leglabels[i_setting];
    leg->AddEntry(hEvdE, leglabel.str().c_str(), "l");
  }

  //  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
  for (int i_setting = n_settings-1; i_setting >= 0; --i_setting) {
    hEvdEs[i_setting]->Draw("BOX SAME");
  }
  leg->Draw();
}
