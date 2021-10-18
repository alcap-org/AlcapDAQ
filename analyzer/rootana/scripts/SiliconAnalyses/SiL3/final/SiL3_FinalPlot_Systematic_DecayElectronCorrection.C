void SiL3_FinalPlot_Systematic_DecayElectronCorrection(std::string savedir = "") {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
    
  std::string filename = "~/data/results/SiL3/systematics_geq2TgtPulse_newPP20us_2.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_settings = 3;
  std::string settings[n_settings] = {"", "_misCalibUp", "_misCalibDown"};
  Int_t colours[n_settings] = {kBlack, kBlue, kRed};
  std::string leglabels[n_settings] = {"k-factor = 0.85", "k-factor = 0.875", "k-factor = 0.825"};

  std::string bin_width = "100keVBins";//"500keVBins";

  TH1F* hSpectra[n_settings] = {0};
  //  TH1F* hSpectra[n_settings][n_settings] = {{0}};
  
  TLegend* leg = new TLegend(0.60,0.65,0.85,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  TLegend* leg_syst = new TLegend(0.20,0.45,0.45,0.65);
  leg_syst->SetBorderSize(0);
  leg_syst->SetTextSize(0.035);
  leg_syst->SetFillColor(kWhite);

  int rebin_factor = 1;
  std::stringstream leglabel;
  TCanvas* c_all_spectra = new TCanvas();
  c_all_spectra->SetLogy();
  TCanvas* c_all_systs = new TCanvas();
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    c_all_spectra->cd();
    std::string setting = settings[i_setting];
    Int_t i_colour = colours[i_setting];
    
    std::string i_dirname = "FinalNormalisation_TimeSlice3000_4000_allRecoil_DecayElec" + setting + "_" + bin_width;
    std::string i_histname = i_dirname + "/hNormalisedSpectrum";
    //      std::string i_dirname = "ResponseMatrix_" + setting + "_TCutG_" + setting;
    //      std::string i_histname = i_dirname + "/hInputSpectrum";
    //      std::string i_histname = i_dirname + "/hCorrectedSpectrum";
    
    std::cout << i_dirname << std::endl;
    TH1F* spectrum = (TH1F*) file->Get(i_histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << i_histname << std::endl;
      return;
    }
    spectrum->Sumw2();
    
    spectrum->SetTitle("SiL3 Dataset, Particle Composition");
    spectrum->Rebin(rebin_factor);
    spectrum->Scale(1.0/rebin_factor);
    spectrum->SetStats(false);
    spectrum->SetLineColor(i_colour);
    spectrum->GetXaxis()->SetRangeUser(0,26000);
    spectrum->Draw("HIST E SAMES");
    
    leglabel.str("");
    leglabel << leglabels[i_setting];
    std::cout << leglabel.str() << std::endl;
    leg->AddEntry(spectrum, leglabel.str().c_str(), "l");
       
    c_all_systs->cd();
    i_dirname = "FinalNormalisation_TimeSlice3000_4000_allRecoil_DecayElec" + setting + "_" + bin_width + "_SystPlot";
    i_histname = i_dirname + "/hSystematic";

    std::cout << i_dirname << std::endl;

    //    if (i_setting>0) {
      TH1F* hSyst = (TH1F*) file->Get(i_histname.c_str());
      if (!hSyst) {
	std::cout << "Error: Problem getting hSyst " << i_histname << std::endl;
	return;
      }

      hSyst->SetLineColor(i_colour);
      hSyst->GetXaxis()->SetRangeUser(0,26000);
      hSyst->Draw("HIST E SAME");

      leg_syst->AddEntry(hSyst, leglabel.str().c_str(), "l");
      //    }
  }

  c_all_spectra->cd();
  leg->Draw();
  c_all_systs->cd();
  leg_syst->Draw();

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_SiL3Dataset_NormalisedSpectrum_MuonStopSettings";
    std::string pdfname = savename + ".pdf";
    c_all_spectra->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c_all_spectra->SaveAs(pngname.c_str());
    
    savename = savedir + "AlCapData_SiL3Dataset_Systematic_MuonStopSettings";
    pdfname = savename + ".pdf";
    c_all_systs->SaveAs(pdfname.c_str());
    pngname = savename + ".png";
    c_all_systs->SaveAs(pngname.c_str());
  }
}
