void MissingStrip15() {

  int run_number = 10404;

  TCanvas* c_TAP_energy = new TCanvas("c_TAP_energy", "c_TAP_energy");

  std::stringstream filename;
  filename.str("");
  filename << "out" << run_number << "_si16-scan_E-400-2000keV.root";
  TFile* file = new TFile(filename.str().c_str(), "READ");
  TH1F* hTAP_Energy_SiL1_15 = (TH1F*) file->Get("PlotTAP_Energy/hSiL1-15-S#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= false}_Energy");
  hTAP_Energy_SiL1_15->SetLineColor(kRed);
  TH1F* hTAP_Energy_SiL1_14 = (TH1F*) file->Get("PlotTAP_Energy/hSiL1-14-S#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= false}_Energy");
  TH1F* hTAP_Energy_SiL1_13 = (TH1F*) file->Get("PlotTAP_Energy/hSiL1-13-S#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= false}_Energy");
  hTAP_Energy_SiL1_13->SetLineColor(kBlack);

  hTAP_Energy_SiL1_14->Draw("HIST");
  hTAP_Energy_SiL1_13->Draw("HIST SAMES");
  hTAP_Energy_SiL1_15->Draw("HIST SAMES");

  TLegend* leg = new TLegend(0.49,0.64,0.69,0.88);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(hTAP_Energy_SiL1_13, "TAPs, SiL1-13-S", "l");
  leg->AddEntry(hTAP_Energy_SiL1_14, "TAPs, SiL1-14-S", "l");
  leg->AddEntry(hTAP_Energy_SiL1_15, "TAPs, SiL1-15-S", "l");
  leg->Draw();

  TCanvas* c_TAP_amplitude = new TCanvas("c_TAP_amplitude", "c_TAP_amplitude");

  TH1F* hTAP_Amplitude_SiL1_15 = (TH1F*) file->Get("PlotTAP_Amplitude/hSiL1-15-S#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= false}_Amplitude");
  hTAP_Amplitude_SiL1_15->SetLineColor(kRed);
  hTAP_Amplitude_SiL1_15->Rebin(2);
  TH1F* hTAP_Amplitude_SiL1_14 = (TH1F*) file->Get("PlotTAP_Amplitude/hSiL1-14-S#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= false}_Amplitude");
  hTAP_Amplitude_SiL1_14->Rebin(2);
  TH1F* hTAP_Amplitude_SiL1_13 = (TH1F*) file->Get("PlotTAP_Amplitude/hSiL1-13-S#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= false}_Amplitude");
  hTAP_Amplitude_SiL1_13->SetLineColor(kBlack);
  hTAP_Amplitude_SiL1_13->Rebin(2);

  hTAP_Amplitude_SiL1_14->Draw("HIST");
  hTAP_Amplitude_SiL1_13->Draw("HIST SAMES");
  hTAP_Amplitude_SiL1_15->Draw("HIST SAMES");

  leg->Draw();

  TCanvas* c_alcapana_heights = new TCanvas("c_alcapana_heights", "c_alcapana_heights");
  filename.str("");
  filename << "/gpfs/home/edmonds_a/data/hist/v6/hist" << run_number << ".root";
  TFile* hist_file = new TFile(filename.str().c_str(), "READ");

  TH1I* hAlcapana_Heights_SiL1_15 = (TH1I*) hist_file->Get("hSIS3300_B5C7_Heights");
  hAlcapana_Heights_SiL1_15->SetLineColor(kRed);
  hAlcapana_Heights_SiL1_15->Rebin(2);
  TH1I* hAlcapana_Heights_SiL1_14 = (TH1I*) hist_file->Get("hSIS3300_B5C6_Heights");
  hAlcapana_Heights_SiL1_14->Rebin(2);
  TH1I* hAlcapana_Heights_SiL1_13 = (TH1I*) hist_file->Get("hSIS3300_B5C5_Heights");
  hAlcapana_Heights_SiL1_13->SetLineColor(kBlack);
  hAlcapana_Heights_SiL1_13->Rebin(2);  

  hAlcapana_Heights_SiL1_14->Draw("HIST");
  hAlcapana_Heights_SiL1_13->Draw("HIST SAMES");
  hAlcapana_Heights_SiL1_15->Draw("HIST SAMES");

  TLegend* leg2 = new TLegend(0.49,0.64,0.69,0.88);
  leg2->SetBorderSize(0);
  leg2->SetTextSize(0.03);
  leg2->SetFillColor(kWhite);
  leg2->AddEntry(hAlcapana_Heights_SiL1_13, "Alcapana Output, SiL1-13-S", "l");
  leg2->AddEntry(hAlcapana_Heights_SiL1_14, "Alcapana Output, SiL1-14-S", "l");
  leg2->AddEntry(hAlcapana_Heights_SiL1_15, "Alcapana Output, SiL1-15-S", "l");
  leg2->Draw();
}
