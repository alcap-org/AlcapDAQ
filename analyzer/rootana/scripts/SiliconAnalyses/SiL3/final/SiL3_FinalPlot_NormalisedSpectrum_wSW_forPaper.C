void SiL3_FinalPlot_NormalisedSpectrum_wSW_forPaper(std::string savedir = "") {

  TCanvas* c_log = new TCanvas("c_log", "c_log");
  c_log->SetLogy();
  TCanvas* c_lin = new TCanvas("c_lin", "c_lin");
  
  //  std::string SW_filename = "~/data/results/SiL3/SW-plots.root";
  //  std::string SW_filename = "~/data/results/SiL3/SW-plots-above1400keV.root";
  std::string SW_filename = "~/data/results/SiL3/SW-plots_betterErrors_MeV.root";
  TFile* SW_file = new TFile(SW_filename.c_str(), "READ");
  TGraphErrors* SW_gre = (TGraphErrors*) SW_file->Get("Graph");
  //  SW_gre->SetTitle("Charged Particle Emission (after decay electron correction)");
  //  SW_gre->SetTitle("Charged Particle Emission (after proton escape correction)");
  //  SW_gre->SetTitle("Charged Particle Emission (after deuteron escape correction)");
  SW_gre->SetTitle("Charged Particle Emission");
  SW_gre->GetXaxis()->SetRangeUser(0,26);
  SW_gre->GetXaxis()->SetTitle("Energy [MeV]");
  SW_gre->GetYaxis()->SetTitle("Particles / muon capture / MeV");
  SW_gre->GetXaxis()->SetTitleOffset(0.9);
  SW_gre->GetYaxis()->SetTitleOffset(0.9);
  SW_gre->SetMarkerColor(kRed);
  SW_gre->SetLineColor(kRed);
  //  SW_gre->GetFunction("tdr_fit")->SetLineColor(kBlack);
  SW_gre->GetFunction("tdr_fit")->SetBit(TF1::kNotDraw);

  c_log->cd();
  SW_gre->Draw("APE");

  c_lin->cd();
  TGraphErrors* SW_gre_lin = (TGraphErrors*) SW_gre->Clone("Graph_lin");
  SW_gre_lin->GetYaxis()->SetRangeUser(0, 0.035e-3);
  SW_gre_lin->Draw("APE");

  TLegend* leg = new TLegend(0.45,0.55,0.85,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(SW_gre, "Sobottka-Wills", "pl");
  //  leg->AddEntry(SW_gre->GetFunction("tdr_fit"), "S-W Fit", "l");

  std::string filename = "~/data/results/SiL3/systematics_geq2TgtPulse_newPP20us_1_newTimeSlice_test.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  
  const int n_slices = 1;
  double min_time_slices[n_slices] = {2000};
  double max_time_slices[n_slices] = {4000};

  const int n_recoils = 1;
  std::string recoils[n_recoils] = {"allRecoil"};//, "noRecoil"};
  std::string leglabels[n_recoils] = {"active Si target"};//AlCap (with recoil correction)", "AlCap (without recoil correction)"};
  Int_t colours[n_recoils] = {kBlack};//, kBlue};
  
  int rebin_factor = 1;
  std::stringstream time_slice_str;
  for (int i_slice = 0; i_slice < n_slices; ++i_slice) {
    double i_min_time_slice = min_time_slices[i_slice];
    double i_max_time_slice = max_time_slices[i_slice];

    time_slice_str.str("");
    time_slice_str << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;

    for (int i_recoil = 0; i_recoil < n_recoils; ++i_recoil) {
      std::string recoil = recoils[i_recoil];
      Int_t i_colour = colours[i_recoil];

      std::string i_dirname = "FinalSystPlot_" + time_slice_str.str() + "_" + recoil + "_500keVBins";
      //      std::string i_dirname = "FinalSystPlot_" + time_slice_str.str() + "_noRecoil_100keVBins";
      std::string i_statsystname = i_dirname + "/hFinalStatSyst";
      TGraphAsymmErrors* statsyst_spectrum = (TGraphAsymmErrors*) file->Get(i_statsystname.c_str());
      if (!statsyst_spectrum) {
	std::cout << "Error: Problem getting statsyst_spectrum " << i_statsystname << std::endl;
	return;
      }
      statsyst_spectrum->SetLineColor(i_colour);
      statsyst_spectrum->SetLineWidth(2);
      statsyst_spectrum->GetXaxis()->SetTitle("Energy [MeV]");
      statsyst_spectrum->GetYaxis()->SetTitle("Charged Particles / captured muon / MeV");
      statsyst_spectrum->GetXaxis()->SetRangeUser(0, 26);
      statsyst_spectrum->GetYaxis()->SetRangeUser(1e-8, 1e-3);
      c_log->cd();
      statsyst_spectrum->Draw("PE SAME");
      c_lin->cd();
      statsyst_spectrum->Draw("PE SAME");
      
      // std::string dirname = "FinalNormalisation_" + time_slice_str.str();
      // std::string i_histname = dirname + "_allRecoil/hNormalisedSpectrum";
      // //    std::string i_histname = dirname + "_noRecoil/hNormalisedSpectrum";
      
      // TH1F* spectrum = (TH1F*) file->Get(i_histname.c_str());
      // spectrum->Sumw2();
      // if (!spectrum) {
      //   std::cout << "Error: Problem getting spectrum " << i_histname << std::endl;
      //   return;
      // }
      
      // spectrum->Rebin(rebin_factor);
      // spectrum->Scale(1.0/rebin_factor);
      // spectrum->SetStats(false);
      // spectrum->SetLineColor(i_colour);
      // alcaphistogram(spectrum);
      // //    spectrum->SetLineWidth(2);
      // c_log->cd();
      // spectrum->Draw("HIST E SAMES");
      // alcapPreliminary(spectrum);
      // c_lin->cd();
      // spectrum->Draw("HIST E SAMES");
      // alcapPreliminary(spectrum);
      
      leg->AddEntry(statsyst_spectrum, leglabels[i_recoil].c_str(), "l");
    }
    
  /*
    std::string fitname = dirname + "/spectral_fit";
    TF1* fit = (TF1*) file->Get(fitname.c_str());
    if (fit) {
      fit->SetLineWidth(2);
      fit->SetLineColor(kCyan);
      fit->Draw("LSAME");
    }
  */
  }

  c_log->cd();
  leg->Draw();
  c_lin->cd();
  leg->Draw();

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_SiL3Dataset_ActiveTarget_NormalisedSpectrum_wSW_LogY_forPaper";
    std::string pdfname = savename + ".pdf";
    c_log->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c_log->SaveAs(pngname.c_str());

    savename = savedir + "AlCapData_SiL3Dataset_ActiveTarget_NormalisedSpectrum_wSW_LinY_forPaper";
    pdfname = savename + ".pdf";
    c_lin->SaveAs(pdfname.c_str());
    pngname = savename + ".png";
    c_lin->SaveAs(pngname.c_str());
  }

}
