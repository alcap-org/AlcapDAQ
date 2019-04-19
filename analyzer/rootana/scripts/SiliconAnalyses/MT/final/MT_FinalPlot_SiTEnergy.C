void MT_FinalPlot_SiTEnergy() {

  const int n_settings = 2;
  std::string settings[n_settings] = {"SF1-05", "SF1-025"};
  Int_t colours[n_settings] = {kBlue, kRed};
  std::string leglabels[n_settings] = {"SF = 1.05", "SF = 1.025"};
  
  TCanvas* c_SiTSpectrum = new TCanvas("c_SiTSpectrum", "c_SiTSpectrum");

  TLegend* leg = new TLegend(0.30,0.45,0.50,0.7);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = settings[i_setting];
    Int_t colour = colours[i_setting];

    std::string plots_file_name = "~/data/results/MT/plots_" + setting + ".root";
    std::string spectrum_name = "SiTEnergy/hSiTEnergy";
    TFile* plots_file = new TFile(plots_file_name.c_str(), "READ");
    TH1F* hSiTSpectrum = (TH1F*) plots_file->Get(spectrum_name.c_str());
 
    //    hSiTSpectrum->Rebin(4);
    hSiTSpectrum->SetLineColor(colour);
    hSiTSpectrum->SetTitle("MT Dataset, SiT Spectrum");
    hSiTSpectrum->SetStats(false);
    hSiTSpectrum->GetXaxis()->SetRangeUser(0, 5000);

    hSiTSpectrum->Scale(1.0 / hSiTSpectrum->Integral());
    
    std::stringstream axislabel;
    axislabel << "Unit Area / " << hSiTSpectrum->GetBinWidth(1) << " keV";
    hSiTSpectrum->SetYTitle(axislabel.str().c_str());
    hSiTSpectrum->GetYaxis()->SetTitleOffset(1.3);

    std::stringstream leglabel;
    leglabel << leglabels[i_setting];
    leg->AddEntry(hSiTSpectrum, leglabel.str().c_str(), "l");
    
    hSiTSpectrum->Draw("HIST SAME");
  }
  
  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.50, 0.75, "AlCap Preliminary");

  leg->Draw();
}
