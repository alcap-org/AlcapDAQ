void Al50_FinalPlot_DeadLayerSystematic_DataVsMC() {

  const int n_settings = 8;
  //  std::string histnames[n_settings] = {"Thin_wNoThick_TimeSlice-200_200/hRawSpectrum", "muonbeam/hRawSpectrum"};
  //  std::string histnames[n_settings] = {"muonbeam_thickStop/hRawSpectrum", "muonbeam_thinStop/hRawSpectrum"};
  std::string filenames[n_settings] = {"~/data/results/Si16b/plots_newPP_geq1TgtPulse_1.root",
				       //				       "/data/R15bMC/respMatrix_100M_Al50_proton_100keVBins.root",
				       "~/g4sim/alcap/respMatrix_100M_Al50_proton_100keVBins_doubleSiR3DL_0-5umDL.root",
				       "~/g4sim/alcap/respMatrix_100M_Al50_proton_100keVBins_doubleSiR3DL_1umDL.root",
				       "~/g4sim/alcap/respMatrix_100M_Al50_proton_100keVBins_doubleSiR3DL_5umDL.root",
				       "~/g4sim/alcap/respMatrix_100M_Al50_proton_100keVBins_doubleSiR3DL_10umDL.root",
				       "~/g4sim/alcap/respMatrix_100M_Al50_proton_100keVBins_doubleSiR3DL_25umDL.root",
				       "~/g4sim/alcap/respMatrix_100M_Al50_proton_100keVBins_doubleSiR3DL_50umDL.root",
				       "~/g4sim/alcap/respMatrix_100M_Al50_proton_100keVBins_doubleSiR3DL_100umDL.root"
};

  std::string leglabels[n_settings] = {"Data",
				       "MC (0.5 #mum DL)",
				       "MC (1 #mum DL)",
				       "MC (5 #mum DL)",
				       "MC (10 #mum DL)",
				       "MC (25 #mum DL)",
				       "MC (50 #mum DL)",
				       "MC (100 #mum DL)"
  };

  Int_t colours[n_settings] = {kBlue,
  			       kRed,
  			       kBlack,
  			       kMagenta,
  			       kCyan,
			       kSpring+3,
			       kGray+2,
			       kGreen
  };
  //  Int_t markers[n_settings] = {kDot, kOpenTriangleUp, kOpenCircle, kOpenTriangleDown, kOpenCircle};
  //  int rebin_factors[n_settings] = {5, 5};
  //  std::string inhistnames[n_settings] = { "proton_SiR_timecut400_10000ns_layerCoinc_PSel/hEvdE_TwoLayer_12not3", "SiR_hEvdE_TwoLayer_stopped"};
  std::string inhistnames[n_settings] = { "all_SiR_timecut400_10000ns_layerCoinc500ns/hEvdE_ThreeLayer_123",
					  "SiR_EvdE_ThreeLayer_stopped",
					  "SiR_EvdE_ThreeLayer_stopped",
  					  "SiR_EvdE_ThreeLayer_stopped",
					  "SiR_EvdE_ThreeLayer_stopped",
					  "SiR_EvdE_ThreeLayer_stopped",
					  "SiR_EvdE_ThreeLayer_stopped",
					  "SiR_EvdE_ThreeLayer_stopped"
  };

  std::string tags[n_settings] = {"data", "MC1", "MC2", "MC3", "MC4", "MC5", "MC6", "MC7"};
  TH2F* hEvdEs[n_settings] = {0};
  
  TLegend* leg = new TLegend(0.20,0.65,0.40,0.85);
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
    hEvdE->SetLineWidth(2);
    //    hEvdE->GetXaxis()->SetTitle("Energy [keV]");
    //    hEvdE->Rebin(rebin_factors[i_setting]);
    //    hEvdE->Scale(1.0 / hEvdE->Integral());
    //    hEvdE->GetYaxis()->SetRangeUser(0,3);
      
    //    axislabel.str("");
    //    axislabel << "Counts / " << hEvdE->GetXaxis()->GetBinWidth(1) << " keV";
    //    hEvdE->GetYaxis()->SetTitle(axislabel.str().c_str());
    //    std::cout << "Drawing " << inhistname << std::endl;
    hEvdEs[i_setting] = hEvdE;
    //    c->cd(i_setting+1);
    //    hEvdE->Draw("P SAME");

    //    if (i_setting <= 2) {
      leglabel.str("");
      leglabel << leglabels[i_setting];
      leg->AddEntry(hEvdE, leglabel.str().c_str(), "l");
      //    }
  }

  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
  //  for (int i_setting = n_settings-1; i_setting >= 0; --i_setting) {
    if (i_setting != 0) {
      std::string profilename = "pfx_" + tags[i_setting];
      TProfile* prof = hEvdEs[i_setting]->ProfileX(profilename.c_str(), 1, -1);
      prof->SetLineColor(colours[i_setting]);
      prof->SetLineWidth(2);
      prof->SetMarkerStyle(kFullCircle);//markers[i_setting]);
      //      prof->SetMarkerSize(1.5);
      prof->Draw("SAME");
    }
    else {
      hEvdEs[i_setting]->Rebin2D(20,20);
      hEvdEs[i_setting]->Draw("COLZ");
    }
    //    TCanvas* c = new TCanvas();
    //    hEvdEs[i_setting]->Draw("COLZ");
  }
  leg->Draw();
}
