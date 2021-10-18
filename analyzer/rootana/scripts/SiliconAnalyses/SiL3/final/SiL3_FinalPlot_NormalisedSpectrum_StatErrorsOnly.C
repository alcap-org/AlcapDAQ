void SiL3_FinalPlot_NormalisedSpectrum_StatErrorsOnly() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();
    
  std::string filename = "~/data/results/SiL3/unfold_geq2TgtPulse_newPP20us_1.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  
  const int n_slices = 1;
  double min_time_slices[n_slices] = {2000};
  double max_time_slices[n_slices] = {4000};
  Int_t colours[n_slices] = {kRed};
  std::string leglabels[n_slices] = {"SiL3 (active target)"};
  
  /*  const int n_slices = 5;
  double min_time_slices[n_slices] = {2000, 2000, 2500, 3000, 3500};
  double max_time_slices[n_slices] = {4000, 2500, 3000, 3500, 4000};
  Int_t colours[n_slices] = {kBlack, kBlue, kRed, kSpring, kMagenta};
  std::string leglabels[n_slices] = {"2000 ns < t < 4000 ns", "2000 ns < t < 2500 ns", "2500 ns < t < 3000 ns", "3000 ns < t < 3500 ns", "3500 ns < t < 4000 ns"};
  */
  
  TLegend* leg = new TLegend(0.25,0.65,0.65,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);
  
  int rebin_factor = 1;
  std::stringstream time_slice_str, leglabel;
  for (int i_slice = 0; i_slice < n_slices; ++i_slice) {
    double i_min_time_slice = min_time_slices[i_slice];
    double i_max_time_slice = max_time_slices[i_slice];

    time_slice_str.str("");
    time_slice_str << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;

    Int_t i_colour = colours[i_slice];

    std::string i_dirname = "FinalNormalisation_" + time_slice_str.str() + "_allRecoil_100keVBins";
    std::string i_histname = i_dirname + "/hNormalisedSpectrum";

    TH1F* spectrum = (TH1F*) file->Get(i_histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << i_histname << std::endl;
      return;
    }
    spectrum->Sumw2();

    spectrum->SetTitle("SiL3 Dataset, Active Target Analysis, Charged Particle Emission Spectrum");
    spectrum->Rebin(rebin_factor);
    spectrum->Scale(1.0/rebin_factor);
    spectrum->SetStats(false);
    spectrum->SetLineColor(i_colour);
    spectrum->GetXaxis()->SetRangeUser(0,30);
    spectrum->Draw("HIST E SAMES");

    /*
    std::string i_fitname = i_dirname + "/spectral_fit";
    TF1* spectral_fit = (TF1*) file->Get(i_fitname.c_str());
    spectral_fit->SetLineColor(i_colour);
    spectral_fit->Draw("LSAME");
    */
    
    leglabel.str("");
    /*    leglabel << "#splitline{" << leglabels[i_slice] << "}{"
	     << std::fixed << std::setprecision(2) << "(T_{th} = " << spectral_fit->GetParameter(1)/1000 << " #pm " << spectral_fit->GetParError(1)/1000 << " MeV, "
	     << std::fixed << std::setprecision(2) << " #alpha = " << spectral_fit->GetParameter(2) << " #pm " << spectral_fit->GetParError(2) << ", "
	     << std::fixed << std::setprecision(2) << " T_{0} = " << spectral_fit->GetParameter(3)/1000 << " #pm " << spectral_fit->GetParError(3)/1000 << " MeV)}";
    */
    leglabel << leglabels[i_slice];
    std::cout << leglabel.str() << std::endl;
    leg->AddEntry(spectrum, leglabel.str().c_str(), "l");
  }

  leg->Draw();
}
