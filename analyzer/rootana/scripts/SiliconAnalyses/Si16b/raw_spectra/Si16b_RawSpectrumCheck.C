void Si16b_RawSpectrumCheck() {

  std::string infilename = "~/data/results/Si16b_passive/raw_spectra.root";
  std::string indirname = "alpha";
  std::string folded_histname = indirname + "/hRawSpectrum";
  
  TFile* infile = new TFile(infilename.c_str(), "READ");
  TH1F* hFoldedSpectrum = (TH1F*) infile->Get(folded_histname.c_str());

  TCanvas* c_EvdE = new TCanvas("c_EvdE", "c_EvdE");

  //  hFoldedSpectrum->SetTitle(outhisttitle.c_str());
  hFoldedSpectrum->Rebin(50);
  hFoldedSpectrum->SetStats(false);
  hFoldedSpectrum->SetLineColor(kBlack);
  hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 15000);
  hFoldedSpectrum->GetYaxis()->SetTitleOffset(1.3);
  
  hFoldedSpectrum->Draw("HIST E");
  c_EvdE->Update();

  double start_time = 1000;
  double end_time = 5000;
  double time_step = 4000;
  std::stringstream time_slice;

  TCanvas* c_FlatBkgs = new TCanvas("c_FlatBkgs", "c_FlatBkgs");
  c_FlatBkgs->SetLogy();

  TCanvas* c_Totals = new TCanvas("c_Totals", "c_Totals");
  c_Totals->SetLogy();

  for (double min_time = start_time; min_time < end_time; min_time += time_step) {
    double max_time = min_time + time_step;
    time_slice.str("");
    time_slice << "TimeSlice" << (int)min_time << "_" << (int)max_time;  

    std::string canvasname = "c_SiL3_" + time_slice.str();
    TCanvas* c_EnergyTime = new TCanvas(canvasname.c_str(), canvasname.c_str());
    c_EnergyTime->SetLogy();

    indirname = "SiL3_all_" + time_slice.str();
    folded_histname = indirname + "/hRawSpectrum";

    TH1F* hFoldedSpectrum_EnergyTime = (TH1F*) infile->Get(folded_histname.c_str());
    //  hFoldedSpectrum_EnergyTime->Rebin(50);
    //  hFoldedSpectrum_EnergyTime->SetStats(false);
    hFoldedSpectrum_EnergyTime->SetLineColor(kBlack);
    hFoldedSpectrum_EnergyTime->GetXaxis()->SetRangeUser(0, 20000);
    hFoldedSpectrum_EnergyTime->GetYaxis()->SetTitleOffset(1.3);
    hFoldedSpectrum_EnergyTime->Draw("HIST E");
    
    std::string flatbkg_histname = indirname + "/hFlatBkg";
    TH1F* hFlatBkg = (TH1F*) infile->Get(flatbkg_histname.c_str());
    //  hFlatBkg->Rebin(50);
    //  hFlatBkg->SetStats(false);
    hFlatBkg->SetLineColor(kRed);
    hFlatBkg->GetXaxis()->SetRangeUser(0, 20000);
    hFlatBkg->GetYaxis()->SetTitleOffset(1.3);
    
    hFlatBkg->Draw("HIST E SAMES");

    TH1F* hTotal = (TH1F*) hFoldedSpectrum_EnergyTime->Clone("hTotal");
    hTotal->Add(hFlatBkg, -1);
    hTotal->SetLineColor(kBlue);
    hTotal->Draw("HIST E SAMES");

    c_FlatBkgs->cd();
    std::cout << time_slice.str() << ": n entries = " << hFlatBkg->GetEntries() << ", peak = " << hFlatBkg->GetMaximum() << std::endl;
    hFlatBkg->Draw("HIST E SAMES");

    c_Totals->cd();
    hTotal->Draw("HIST E SAMES");
    hFoldedSpectrum->Scale(1.0/10);
    hFoldedSpectrum->Draw("HIST SAMES");
  }
}
