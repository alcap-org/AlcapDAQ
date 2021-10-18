void SiL3_FinalPlot_Systematic_UnfoldingRegParameter(std::string savedir = "") {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
    
  std::string filename = "~/data/results/SiL3/systematics_geq2TgtPulse_newPP20us_1_newTimeSlice.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_reg_parameters = 3;
  int reg_parameters[n_reg_parameters] = {250, 200, 300};
  Int_t colours[n_reg_parameters] = {kBlack, kBlue, kRed};
  std::string leglabels[n_reg_parameters] = {"", "", ""};

  //   const int n_reg_parameters = 4;
  // std::string reg_parameters[n_reg_parameters] = {"allReg_Parameter", "lowAllReg_Parameter", "highAllReg_Parameter", "noReg_Parameter"};
  // Int_t colours[n_reg_parameters] = {kBlack, kBlue, kRed, kMagenta};
  // std::string leglabels[n_reg_parameters] = {"central value", "low", "high", "none"};//"1 Iteration", "2 Iterations"};


  TH1F* hSpectra[n_reg_parameters] = {0};
  //  TH1F* hSpectra[n_reg_parameters][n_settings] = {{0}};
  
  TLegend* leg = new TLegend(0.60,0.65,0.85,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  TLegend* leg_syst = new TLegend(0.20,0.25,0.45,0.45);
  leg_syst->SetBorderSize(0);
  leg_syst->SetTextSize(0.035);
  leg_syst->SetFillColor(kWhite);
  
  int rebin_factor = 1;
  std::stringstream leglabel;
  TCanvas* c_all_spectra = new TCanvas();
  c_all_spectra->SetLogy();
  TCanvas* c_all_systs = new TCanvas();
  for (int i_reg_parameter = 0; i_reg_parameter < n_reg_parameters; ++i_reg_parameter) {
    c_all_spectra->cd();
    int reg_parameter = reg_parameters[i_reg_parameter];
    Int_t i_colour = colours[i_reg_parameter];

    std::string setting = "KReg" + std::to_string(reg_parameter);
    
    std::string i_dirname = "FinalNormalisation_TimeSlice2000_4000_allRecoil_" + setting + "_500keVBins";
    std::string i_histname = i_dirname + "/hNormalisedSpectrum";
    //      std::string i_dirname = "ResponseMatrix_" + recoil + "_TCutG_" + setting;
    //      std::string i_histname = i_dirname + "/hInputSpectrum";
    //      std::string i_histname = i_dirname + "/hCorrectedSpectrum";
    
    std::cout << i_dirname << std::endl;
    TH1F* spectrum = (TH1F*) file->Get(i_histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << i_histname << std::endl;
      return;
    }
    spectrum->Sumw2();
    
    spectrum->SetTitle("SiL3 Dataset, Unfolding Reg Parameter");
    spectrum->Rebin(rebin_factor);
    spectrum->Scale(1.0/rebin_factor);
    spectrum->SetStats(false);
    spectrum->SetLineColor(i_colour);
    spectrum->GetXaxis()->SetRangeUser(0,26000);
    spectrum->Draw("HIST E SAMES");
    
    leglabel.str("");
    leglabel << "k_{reg} = " << reg_parameter;
    std::cout << leglabel.str() << std::endl;
    leg->AddEntry(spectrum, leglabel.str().c_str(), "l");

    if (i_reg_parameter>0) {
      c_all_systs->cd();
      i_dirname = "FinalNormalisation_TimeSlice2000_4000_allRecoil_" + setting + "_500keVBins_SystPlot";
      i_histname = i_dirname + "/hSystematic";
      
      std::cout << i_dirname << std::endl;
      TH1F* hSyst = (TH1F*) file->Get(i_histname.c_str());
      if (!hSyst) {
	std::cout << "Error: Problem getting hSyst " << i_histname << std::endl;
	return;
      }

      hSyst->SetLineColor(i_colour);
      hSyst->GetXaxis()->SetRangeUser(0,26000);
      hSyst->Draw("HIST E SAME");

      leg_syst->AddEntry(hSyst, leglabel.str().c_str(), "l");
    }
  }

  c_all_spectra->cd();
  leg->Draw();
  c_all_systs->cd();
  leg_syst->Draw();

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_SiL3Dataset_NormalisedSpectrum_UnfoldingRegParameter";
    std::string pdfname = savename + ".pdf";
    c_all_spectra->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c_all_spectra->SaveAs(pngname.c_str());
    
    savename = savedir + "AlCapData_SiL3Dataset_Systematic_UnfoldingRegParameter";
    pdfname = savename + ".pdf";
    c_all_systs->SaveAs(pdfname.c_str());
    pngname = savename + ".png";
    c_all_systs->SaveAs(pngname.c_str());
  }
}
