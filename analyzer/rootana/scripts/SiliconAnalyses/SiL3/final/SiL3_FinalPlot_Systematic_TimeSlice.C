void SiL3_FinalPlot_Systematic_TimeSlice(std::string savedir = "") {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
    
  std::string filename = "~/data/results/SiL3/systematics_geq2TgtPulse_newPP20us_1_KFactor0-85_test.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  // const int n_time_slices = 5;
  // double min_time_slices[n_time_slices] = {2000, 2000, 2500, 3000, 3500};
  // double max_time_slices[n_time_slices] = {4000, 2500, 3000, 3500, 4000};
  // const int n_time_slices = 3;
  // double min_time_slices[n_time_slices] = {2000, 2500, 3000};
  // double max_time_slices[n_time_slices] = {4000, 4000, 4000};
  const int n_time_slices = 2;
  double min_time_slices[n_time_slices] = {3000, 3500};
  double max_time_slices[n_time_slices] = {3500, 4000};

  Int_t colours[n_time_slices] = {kBlack, kBlue};//, kRed};//, kMagenta, kSpring};
  std::string leglabels[n_time_slices] = {};

  std::string bin_width = "100keVBins";//"500keVBins";

  TH1F* hSpectra[n_time_slices] = {0};
  //  TH1F* hSpectra[n_time_slices][n_settings] = {{0}};
  
  TLegend* leg = new TLegend(0.60,0.65,0.85,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  TLegend* leg_syst = new TLegend(0.20,0.55,0.45,0.75);
  leg_syst->SetBorderSize(0);
  leg_syst->SetTextSize(0.035);
  leg_syst->SetFillColor(kWhite);
  
  int rebin_factor = 1;
  std::stringstream leglabel, syst_time_slice;
  TCanvas* c_all_spectra = new TCanvas();
  c_all_spectra->SetLogy();
  TCanvas* c_all_systs = new TCanvas();
  for (int i_time_slice = 0; i_time_slice < n_time_slices; ++i_time_slice) {
    c_all_spectra->cd();
    double i_min_time_slice = min_time_slices[i_time_slice];
    double i_max_time_slice = max_time_slices[i_time_slice];
    
    syst_time_slice.str("");
    syst_time_slice << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;

    Int_t i_colour = colours[i_time_slice];
    
    std::string i_dirname = "FinalNormalisation_" + syst_time_slice.str() + "_TimeSlice3000_4000Syst_allRecoil_" + bin_width + "_SystPlot";
    //    std::string i_dirname = "DecayElectron_" + syst_time_slice.str() + "_TimeSlice3000_4000Syst_allRecoil_" + bin_width + "_SystPlot";
    std::string i_histname = i_dirname + "/syst_hist";
    //      std::string i_dirname = "ResponseMatrix_" + time_slice + "_TCutG_" + setting;
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
    spectrum->GetXaxis()->SetTitle("Energy [keV]");
    spectrum->Draw("HIST E SAMES");
    
    leglabel.str("");
    leglabel << i_min_time_slice << " ns < t < " << i_max_time_slice << " ns";
    std::cout << leglabel.str() << std::endl;
    leg->AddEntry(spectrum, leglabel.str().c_str(), "l");

    if (i_time_slice > 0) {
      c_all_systs->cd();
      i_dirname = "FinalNormalisation_" + syst_time_slice.str() + "_TimeSlice3000_4000Syst_allRecoil_" + bin_width + "_SystPlot";
      //      i_dirname = "DecayElectron_" + syst_time_slice.str() + "_TimeSlice3000_4000Syst_allRecoil_100keVBins_SystPlot";
      i_histname = i_dirname + "/hSystematic";
      
      std::cout << i_dirname << std::endl;
      TH1F* hSyst = (TH1F*) file->Get(i_histname.c_str());
      if (!hSyst) {
	std::cout << "Error: Problem getting hSyst " << i_histname << std::endl;
	return;
      }

      leg_syst->AddEntry(hSyst, leglabel.str().c_str(), "l");
      hSyst->SetLineColor(i_colour);
      hSyst->GetXaxis()->SetRangeUser(0,26000);
      hSyst->SetMinimum(-200);
      hSyst->SetMaximum(400);
      hSyst->GetXaxis()->SetTitle("Energy [keV]");
      hSyst->Draw("HIST E SAME");
    }
  }

  c_all_spectra->cd();
  leg->Draw();
  c_all_systs->cd();
  leg_syst->Draw();

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_SiL3Dataset_NormalisedSpectrum_TimeSlice";
    std::string pdfname = savename + ".pdf";
    c_all_spectra->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c_all_spectra->SaveAs(pngname.c_str());
    
    savename = savedir + "AlCapData_SiL3Dataset_Systematic_TimeSlice";
    pdfname = savename + ".pdf";
    c_all_systs->SaveAs(pdfname.c_str());
    pngname = savename + ".png";
    c_all_systs->SaveAs(pngname.c_str());
  }
}
