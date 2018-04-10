void Si16bVsSiL3_ProtonSpectra() {

  //////////////////////////////////
  // User parameters
  std::string si16b_filename = "~/data/results/Si16b_passive/final-plots_NewCuts.root";
  std::string sil3_filename = "~/data/results/SiL3_active/corrected-spectrum.root";

  TFile* si16b_file = new TFile(si16b_filename.c_str(), "READ");
  TH1F* hSi16b = (TH1F*) si16b_file->Get("hNormalisedSpectrum");

  TFile* sil3_file = new TFile(sil3_filename.c_str(), "READ");
  TH1F* hSiL3_0 = (TH1F*) sil3_file->Get("hTimeSlice_1000");
  TH1F* hSiL3_1 = (TH1F*) sil3_file->Get("hTimeSlice_1500");
  TH1F* hSiL3_2 = (TH1F*) sil3_file->Get("hTimeSlice_2000");
  TH1F* hSiL3_3 = (TH1F*) sil3_file->Get("hTimeSlice_2500");
  TH1F* hSiL3_4 = (TH1F*) sil3_file->Get("hTimeSlice_3000");
  TH1F* hSiL3_5 = (TH1F*) sil3_file->Get("hTimeSlice_3500");

  TCanvas* c_comparison = new TCanvas("c_comparison", "c_comparison");
  c_comparison->SetLogy();
  
  hSiL3_5->Draw("HIST E");
  hSiL3_4->Draw("HIST E SAME");
  hSiL3_3->Draw("HIST E SAME");
  hSiL3_2->Draw("HIST E SAME");
  hSiL3_1->Draw("HIST E SAME");
  hSiL3_0->Draw("HIST E SAME");
  hSi16b->Draw("HIST E SAME");

  TCanvas* c_integrals = new TCanvas("c_integrals", "c_integrals");
  const int n_hists = 7;
  TH1F* hists[n_hists] = {hSiL3_5, hSiL3_4, hSiL3_3, hSiL3_2, hSiL3_1, hSiL3_0, hSi16b};
  TH1F* hIntegralHists[n_hists];
  for (int i_hist = 0; i_hist < n_hists; ++i_hist) {
    TH1F* this_hist = hists[i_hist];
    std::cout << this_hist->GetName() << std::endl;

    std::string histname = this_hist->GetName();
    histname += "_Integral";
    hIntegralHists[i_hist] = new TH1F(histname.c_str(), "", 3, 0, 3);
    hIntegralHists[i_hist]->SetYTitle("Protons / Capture");
    hIntegralHists[i_hist]->GetXaxis()->SetBinLabel(1, "Integral (2 -- 15 MeV)");
    hIntegralHists[i_hist]->GetXaxis()->SetBinLabel(2, "Integral (3 -- 10 MeV)");
    hIntegralHists[i_hist]->GetXaxis()->SetBinLabel(3, "Integral (4 -- 8 MeV)");
    hIntegralHists[i_hist]->SetLineColor(this_hist->GetLineColor());
    hIntegralHists[i_hist]->SetLineWidth(this_hist->GetLineWidth());
    
    double integral_min = 2000;
    double integral_max = 15000;
    int integral_bin_min = this_hist->FindBin(integral_min);
    int integral_bin_max = this_hist->FindBin(integral_max);
    double integral, error;
    integral = this_hist->IntegralAndError(integral_bin_min, integral_bin_max, error, "width");
    hIntegralHists[i_hist]->SetBinContent(1, integral);
    hIntegralHists[i_hist]->SetBinError(1, error);
    std::cout << "Integral (" << integral_min << " -- " << integral_max << " keV): " << integral << " +- " << error << std::endl;
    
    integral_min = 3000;
    integral_max = 10000;
    integral_bin_min = this_hist->FindBin(integral_min);
    integral_bin_max = this_hist->FindBin(integral_max);
    integral = this_hist->IntegralAndError(integral_bin_min, integral_bin_max, error, "width");
    hIntegralHists[i_hist]->SetBinContent(2, integral);
    hIntegralHists[i_hist]->SetBinError(2, error);
    std::cout << "Integral (" << integral_min << " -- " << integral_max << " keV): " << integral << " +- " << error << std::endl;
    
    integral_min = 4000;
    integral_max = 8000;
    integral_bin_min = this_hist->FindBin(integral_min);
    integral_bin_max = this_hist->FindBin(integral_max);
    integral = this_hist->IntegralAndError(integral_bin_min, integral_bin_max, error, "width");
    hIntegralHists[i_hist]->SetBinContent(3, integral);
    hIntegralHists[i_hist]->SetBinError(3, error);
    std::cout << "Integral (" << integral_min << " -- " << integral_max << " keV): " << integral << " +- " << error << std::endl;

    hIntegralHists[i_hist]->Draw("HIST E SAME");
  }
}
