void Si16b_RoughPlots() {
  bool save = true;
  
  std::string si16b_filename = "~/data/results/Si16b/plots.root";
  TFile* si16b_file = new TFile(si16b_filename.c_str(), "READ");

  std::string ti50_filename = "~/data/results/Ti50/plots.root";
  TFile* ti50_file = new TFile(ti50_filename.c_str(), "READ");

  std::string sil3_filename = "~/data/results/SiL3/plots.root";
  TFile* sil3_file = new TFile(sil3_filename.c_str(), "READ");

  
  TCanvas* c0 = new TCanvas("c0", "c0");
  int rebin_factor = 10;
  double min_energy = 1000;
  double max_energy = 10000;
  double min_time_axis = -1000;
  double max_time_axis = 10000;

  TH2F* hSi16bBkg = (TH2F*) si16b_file->Get("SiR23/hEnergyTime");

  int min_energy_bin = hSi16bBkg->GetYaxis()->FindBin(min_energy);
  int max_energy_bin = hSi16bBkg->GetYaxis()->FindBin(max_energy)-1;

  std::stringstream histtitle;
  histtitle << "SiR2 Time (!SiR1 && SiR3), " << min_energy << " keV < SiR2.E < " << max_energy << " keV";
  TH1D* hSi16bBkgTime = hSi16bBkg->ProjectionX("Si16b", min_energy_bin, max_energy_bin);
  hSi16bBkgTime->SetTitle(histtitle.str().c_str());
  hSi16bBkgTime->Rebin(rebin_factor);
  hSi16bBkgTime->GetXaxis()->SetRangeUser(min_time_axis,max_time_axis);
  
  double min_lin_fit_time = 8000;
  double max_lin_fit_time = 10000;
  TF1* si16b_lin_fit = new TF1("si16b_lin_fit", "[0]", min_lin_fit_time, max_lin_fit_time);
  hSi16bBkgTime->Fit(si16b_lin_fit, "R0");
  double si16b_lin = si16b_lin_fit->GetParameter(0);
  
  TH2F* hTi50Bkg = (TH2F*) ti50_file->Get("SiR23/hEnergyTime");
  TH1D* hTi50BkgTime = hTi50Bkg->ProjectionX("Ti50", min_energy_bin, max_energy_bin);
  hTi50BkgTime->SetTitle(histtitle.str().c_str());
  hTi50BkgTime->Rebin(rebin_factor);
  hTi50BkgTime->GetXaxis()->SetRangeUser(min_time_axis,max_time_axis);
  hTi50BkgTime->SetLineColor(kRed);

  TF1* ti50_lin_fit = new TF1("ti50_lin_fit", "[0]", min_lin_fit_time, max_lin_fit_time);
  hTi50BkgTime->Fit(ti50_lin_fit, "R0");
  double ti50_lin = ti50_lin_fit->GetParameter(0);
  
  
  hTi50BkgTime->Scale(si16b_lin / ti50_lin);
  hTi50BkgTime->Draw("HIST E");
  hSi16bBkgTime->Draw("HIST E SAMES");

  TLegend* leg = new TLegend(0.35,0.55,0.9,0.75);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(hSi16bBkgTime, "Si16b", "l");
  std::stringstream leglabel;
  leglabel << "Ti50 (normalised at " << min_lin_fit_time << " ns < t < " << max_lin_fit_time << " ns)";
  leg->AddEntry(hTi50BkgTime, leglabel.str().c_str(), "l");
  leg->Draw();




  //  TH2F* hFull = (TH2F*) si16b_file->Get("SiR2/hEnergyTime");
  //  TH2F* hFull = (TH2F*) si16b_file->Get("SiL3/hEnergyTime");
  TH2F* hFull = (TH2F*) sil3_file->Get("Target/hEnergyTime");
  //  TH2F* hBkg = (TH2F*) si16b_file->Get("SiR23/hEnergyTime");

  double min_full_energy = 1000;
  double max_full_energy = 10000;
  int min_full_energy_bin = hFull->GetYaxis()->FindBin(min_full_energy);
  int max_full_energy_bin = hFull->GetYaxis()->FindBin(max_full_energy)-1;

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy(1);


  TH1F* hFullTime = (TH1F*) hFull->ProjectionX("hFullTime", min_full_energy_bin, max_full_energy_bin);
  hFullTime->Sumw2();
  hFullTime->Rebin(rebin_factor);
  hFullTime->GetXaxis()->SetRangeUser(min_time_axis,max_time_axis);
  hFullTime->SetMinimum(0.1);
  hFullTime->SetMaximum(5e5);
  hFullTime->Draw("HIST E");

  TF1* full_lin_fit = new TF1("full_lin_fit", "[0]", min_lin_fit_time, max_lin_fit_time);
  hFullTime->Fit(full_lin_fit, "R");
  double full_lin = full_lin_fit->GetParameter(0);

  TH1D* hBkgTime = (TH1D*) hSi16bBkgTime->Clone("hBkgTime");
  hBkgTime->Sumw2();
  hBkgTime->Scale(full_lin / si16b_lin);
  hBkgTime->SetLineColor(kRed);
  hBkgTime->Draw("HIST E SAMES");
  
  TH1F* hSigTime = (TH1F*) hFullTime->Clone("hSigTime");
  hSigTime->Add(hBkgTime, -1);
  hSigTime->SetLineColor(kBlack);
  hSigTime->Draw("HIST E SAMES");



  /*
  TCanvas* c2 = new TCanvas("c2", "c2");
  c2->SetLogy(1);

  double min_time = 1000;
  double max_time = 4000;
  int min_time_bin = hFull->GetXaxis()->FindBin(min_energy);
  int max_time_bin = hFull->GetXaxis()->FindBin(max_energy)-1;

  TH1F* hFullEnergy = (TH1F*) hFull->ProjectionY("hFullEnergy", min_time_bin, max_time_bin);
  hFullEnergy->Rebin(rebin_factor);
  //  hFullEnergy->GetXaxis()->SetRangeUser(-1000,10000);
  hFullEnergy->Draw("HIST E");

  //  double min_lin_fit_energy = 8000;
  //  double max_lin_fit_energy = 10000;
  //  TF1* full_lin_fit = new TF1("full_lin_fit", "[0]", min_lin_fit_energy, max_lin_fit_energy);
  //  hFullEnergy->Fit(full_lin_fit, "R");
  //  double full_lin = full_lin_fit->GetParameter(0);

  TH1F* hBkgEnergy = (TH1F*) hBkg->ProjectionY("hBkgEnergy", min_time_bin, max_time_bin);
  hBkgEnergy->Rebin(rebin_factor);
  hBkgEnergy->SetLineColor(kRed);
  hBkgEnergy->Draw("HIST E SAMES");

  //  TF1* bkg_lin_fit = new TF1("bkg_lin_fit", "[0]", min_lin_fit_energy, max_lin_fit_energy);
  //  hBkgEnergy->Fit(bkg_lin_fit, "R");
  //  double bkg_lin = bkg_lin_fit->GetParameter(0);

  hBkgEnergy->Scale(full_lin / bkg_lin);

  TH1F* hSigEnergy = (TH1F*) hFullEnergy->Clone("hSigEnergy");
  hSigEnergy->Add(hBkgEnergy, -1);
  hSigEnergy->SetLineColor(kBlack);
  hSigEnergy->Draw("HIST E SAME");
  */
}
