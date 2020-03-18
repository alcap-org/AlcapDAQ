void Si16b_FinalPlot_XRaySpectrum_TimeCuts(std::string savedir = "", std::ostream& numbers_file = std::cout) {

  const int n_settings = 2;
  int time_cuts[n_settings] = {5000, 200};
  //  std::string time_cuts[n_settings] = {"5000ns", "200ns"};
  Int_t colours[n_settings] = {kBlue, kRed};
  //  std::string leglabels[n_settings] = {"|t| < 5000 ns", "|t| < 200 ns"};

  const int n_ge_channels = 2;
  std::string ge_channels[n_ge_channels] = {"GeLoGain", "GeHiGain"};

  numbers_file << "% From Si16b_FinalPlot_XRaySpectrum_TimeCuts.C" << std::endl;
  for (int i_ge_channel = 0; i_ge_channel < n_ge_channels; ++i_ge_channel) {
    TLegend* leg = new TLegend(0.50,0.55,0.85,0.85);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.03);
    leg->SetFillColor(kWhite);

    std::string ge_channel = ge_channels[i_ge_channel];
    TCanvas* c_XRaySpectrum = new TCanvas("c_XRaySpectrum", "c_XRaySpectrum");
  
    for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
      std::string time_cut = std::to_string(time_cuts[i_setting]) + "ns";
      std::string norm_file_name = "/home/edmonds/data/results/Si16b/normalisation_newPP_geq1TgtPulse.root";
      std::string dirname = "XRaySpectrum_" + ge_channel + "_2p1s_" + time_cut + "TimeCut";
      std::string norm_ws_name = dirname + "/ws";
      std::string full_spectrum_name = dirname + "/hGe_Spectrum";
    
      TFile* norm_file = new TFile(norm_file_name.c_str(), "READ");
      RooWorkspace* ws = (RooWorkspace*) norm_file->Get(norm_ws_name.c_str());
      TH1D* hXRaySpectrum = (TH1D*) norm_file->Get(full_spectrum_name.c_str());

      hXRaySpectrum->Rebin(4);
      hXRaySpectrum->SetLineColor(colours[i_setting]);
      std::string histtitle = "Si16b Dataset, Full X-Ray Spectrum, " + ge_channel;
      hXRaySpectrum->SetTitle(histtitle.c_str());
      hXRaySpectrum->SetStats(false);
      hXRaySpectrum->GetXaxis()->SetRangeUser(50, 900);

      std::stringstream axislabel;
      axislabel << "Counts / " << hXRaySpectrum->GetBinWidth(1) << " keV";
      hXRaySpectrum->SetYTitle(axislabel.str().c_str());
      hXRaySpectrum->GetYaxis()->SetTitleOffset(1.3);
  
      hXRaySpectrum->Draw("HIST E SAME");
      alcaphistogram(hXRaySpectrum);
      if (i_setting == 0) {
	hXRaySpectrum->SetDrawOption("HIST E1");
	alcapPreliminary(hXRaySpectrum);
      }
      else {
	hXRaySpectrum->SetDrawOption("HIST E1 SAME");
	alcaplabel(ge_channel, hXRaySpectrum);
      }
      hXRaySpectrum->SetLineWidth(1);

      std::string leglabel = "|t| < " + std::to_string(time_cuts[i_setting]) + " ns";
      leg->AddEntry(hXRaySpectrum, leglabel.c_str(), "l");

      if (i_setting == 1) {
	if (ge_channel == "GeHiGain") { // only want to write this out once
	  numbers_file << "\\newcommand\\SibNXRaysTimeCut{$|t|~\\SI{<" << time_cuts[i_setting] << "}{\\nano\\second}$}" << std::endl;
	}
	
	std::string treename = dirname + "/counttree";
	TTree* counttree = (TTree*) norm_file->Get(treename.c_str());
	double n_xrays = 0;
	double n_xrays_error = 0;
	counttree->SetBranchAddress("xray_count", &n_xrays);
	counttree->SetBranchAddress("xray_count_error", &n_xrays_error);
	counttree->GetEntry(0);
      
	numbers_file << "\\newcommand\\Sib" << ge_channel << "NXRaysTimeCut";
	numbers_file << std::fixed << std::setprecision(0) << "{$" << n_xrays << " \\pm " << n_xrays_error << "$}" << std::endl;
      }
      leg->Draw();
    }
    //  TLatex* latex = new TLatex();
    //  latex->DrawLatexNDC(0.17, 0.70, "AlCap Preliminary");

    if (savedir != "") {
      std::string savename = savedir + "AlCapData_Si16bDataset_XRaySpectrum_" + ge_channel + "_TimeCuts";

      std::string pdfname = savename + ".pdf";
      c_XRaySpectrum->SaveAs(pdfname.c_str());
      std::string pngname = savename + ".png";
      c_XRaySpectrum->SaveAs(pngname.c_str());
    }
  }
  numbers_file << std::endl;
}
