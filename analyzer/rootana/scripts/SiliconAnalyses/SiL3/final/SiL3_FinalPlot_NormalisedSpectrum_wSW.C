void SiL3_FinalPlot_NormalisedSpectrum_wSW() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
    
  std::string SW_filename = "~/data/results/SiL3/SW-plots.root";
  //  std::string SW_filename = "~/data/results/SiL3/SW-plots-above1400keV.root";
  TFile* SW_file = new TFile(SW_filename.c_str(), "READ");
  TGraphErrors* SW_gre = (TGraphErrors*) SW_file->Get("Graph");
  SW_gre->Draw("APE");
  SW_gre->SetTitle("Charged Particle Emission");
  SW_gre->GetXaxis()->SetRangeUser(0,26000);
  SW_gre->GetXaxis()->SetTitle("Energy [keV]");
  SW_gre->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per keV");

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(SW_gre, "Sobottka-Wills", "pl");
  SW_gre->GetFunction("tdr_fit")->SetLineColor(kBlack);
  leg->AddEntry(SW_gre->GetFunction("tdr_fit"), "S-W Fit", "l");

  std::string filename = "~/data/results/SiL3/unfold_geq2TgtPulse.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  
  const int n_slices = 1;
  double min_time_slices[n_slices] = {2000};
  double max_time_slices[n_slices] = {4000};
  Int_t colours[n_slices] = {kRed};
  std::string leglabels[n_slices] = {"SiL3 (active target)"};

  int rebin_factor = 5;
  std::stringstream time_slice_str;
  for (int i_slice = 0; i_slice < n_slices; ++i_slice) {
    double i_min_time_slice = min_time_slices[i_slice];
    double i_max_time_slice = max_time_slices[i_slice];

    time_slice_str.str("");
    time_slice_str << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;

    Int_t i_colour = colours[i_slice];

    std::string i_histname = "FinalNormalisation_" + time_slice_str.str() + "/hNormalisedSpectrum";

    TH1F* spectrum = (TH1F*) file->Get(i_histname.c_str());
    spectrum->Sumw2();
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << i_histname << std::endl;
      return;
    }

    spectrum->Rebin(rebin_factor);
    spectrum->Scale(1.0/rebin_factor);
    spectrum->SetStats(false);
    spectrum->SetLineColor(i_colour);
    spectrum->Draw("HIST E SAMES");
    leg->AddEntry(spectrum, leglabels[i_slice].c_str(), "l");
  }

  leg->Draw();
}
