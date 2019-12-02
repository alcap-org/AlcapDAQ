void SiL3_FinalPlot_XRaySpectrum() {

  const int n_settings = 2;
  std::string time_cuts[n_settings] = {"5000ns", "200ns"};
  Int_t colours[n_settings] = {kBlue, kRed};
  std::string leglabels[n_settings] = {"|t| < 5000 ns", "|t| < 200 ns"};

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);

  TCanvas* c_XRaySpectrum = new TCanvas("c_XRaySpectrum", "c_XRaySpectrum");
  
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string ge_channel = "GeHiGain";
    std::string norm_file_name = "/home/edmonds/data/results/SiL3/normalisation_geq0TgtPulse_newPP20us.root";
    std::string dirname = "XRaySpectrum_" + ge_channel + "_2p1s_" + time_cuts[i_setting] + "TimeCut";
    std::string norm_ws_name = dirname + "/ws";
    std::string full_spectrum_name = dirname + "/hGe_Spectrum";
    
    TFile* norm_file = new TFile(norm_file_name.c_str(), "READ");
    RooWorkspace* ws = (RooWorkspace*) norm_file->Get(norm_ws_name.c_str());
    TH1D* hXRaySpectrum = (TH1D*) norm_file->Get(full_spectrum_name.c_str());

    hXRaySpectrum->Rebin(4);
    hXRaySpectrum->SetLineColor(colours[i_setting]);
    std::string histtitle = "SiL3 Dataset, Full X-Ray Spectrum, " + ge_channel;
    hXRaySpectrum->SetTitle(histtitle.c_str());
    hXRaySpectrum->SetStats(false);
    hXRaySpectrum->GetXaxis()->SetRangeUser(50, 1200);

    std::stringstream axislabel;
    axislabel << "Counts / " << hXRaySpectrum->GetBinWidth(1) << " keV";
    hXRaySpectrum->SetYTitle(axislabel.str().c_str());
    hXRaySpectrum->GetYaxis()->SetTitleOffset(1.3);
  
    hXRaySpectrum->Draw("HIST E SAME");

    leg->AddEntry(hXRaySpectrum, leglabels[i_setting].c_str(), "l");
  }
  leg->Draw();
  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.17, 0.70, "AlCap Preliminary");
}
