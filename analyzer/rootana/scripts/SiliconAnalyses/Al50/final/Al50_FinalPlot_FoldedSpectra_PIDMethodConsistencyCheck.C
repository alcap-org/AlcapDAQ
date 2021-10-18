void Al50_FinalPlot_FoldedSpectra_PIDMethodConsistencyCheck() {

  const int n_settings = 2;
  //  std::string filenames[n_settings] = {"mark-pid-folded.root", "john-results.root"};
  std::string filenames[n_settings] = {"mark-pid-folded-correct.root", "john-results.root"};
  std::string settings[n_settings] = {"Mark's PID", "John's PID"};
  //  std::string histnames[n_settings] = {"hpSiR_2", "hr_e_raw"};
  std::string histnames[n_settings] = {"hpSiR_1sig", "hr_e_raw"};
  Int_t colours[n_settings] = {kRed, kBlue};//, kCyan, kMagenta, kSpring};
  TH1F* hFoldedSpectra[n_settings] = {0};
  double scales[n_settings] = {0.68, 1};//{0.95, 1}; // not sure of John's yet
  
  std::stringstream axistitle;

  TLegend* leg = new TLegend(0.50,0.55,0.70,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  TCanvas* c_Spectra = new TCanvas();
  
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string infilename = "~/data/results/Al50/" + filenames[i_setting];
    TFile* infile = new TFile(infilename.c_str(), "READ");      

    std::string folded_histname = histnames[i_setting];
    TH1F* hFoldedSpectrum = (TH1F*) infile->Get(folded_histname.c_str());
    if (!hFoldedSpectrum) {
      std::cout << "Can't find histogram " << folded_histname << std::endl;
      continue;
    }

    //      hFoldedSpectrum->Rebin(rebin_factor);
    hFoldedSpectrum->Scale(1.0 / scales[i_setting]);
    
    std::string clonename = "clone_" + folded_histname;

    if (i_setting == 1) {
      TFile* corrfile = new TFile("~/data/results/Al50/john-results-pid.root", "READ");
      TH1F* hPIDCorr = (TH1F*) corrfile->Get("hrpidcorrerr");
      hFoldedSpectrum->Divide(hPIDCorr);
      
      hFoldedSpectrum->Rebin(2);
      TH1F* hConvert = (TH1F*) hFoldedSpectra[0]->Clone(clonename.c_str());
      for (int i_bin = 1; i_bin <= hConvert->GetNbinsX(); ++i_bin) {
	int original_bin = hFoldedSpectrum->FindBin(hConvert->GetBinCenter(i_bin)*1000);
	if (original_bin < hFoldedSpectrum->GetNbinsX()) {
	  hConvert->SetBinContent(i_bin, hFoldedSpectrum->GetBinContent(original_bin));
	  hConvert->SetBinError(i_bin, hFoldedSpectrum->GetBinError(original_bin));
	}
      }
      hFoldedSpectra[i_setting] = hConvert;
    }
    else {
      hFoldedSpectra[i_setting] = (TH1F*) hFoldedSpectrum->Clone(clonename.c_str());
    }

    hFoldedSpectra[i_setting]->SetStats(false);
    hFoldedSpectra[i_setting]->SetLineColor(colours[i_setting]);
    hFoldedSpectra[i_setting]->SetLineWidth(2);
    hFoldedSpectra[i_setting]->GetYaxis()->SetTitleOffset(1.);
    axistitle.str("");
    axistitle << "Raw Count / " << hFoldedSpectra[i_setting]->GetXaxis()->GetBinWidth(1) << " MeV";
    hFoldedSpectra[i_setting]->SetTitle("Al50 Dataset, Right Arm, Raw Spectrum");
    hFoldedSpectra[i_setting]->SetYTitle(axistitle.str().c_str());
    hFoldedSpectra[i_setting]->Draw("HIST E SAME");

    std::stringstream leglabel;
    std::string setting = settings[i_setting];
    if (i_setting == 0) {
      leglabel << setting << " (scaled by 1.0/" << std::fixed << std::setprecision(3) << scales[i_setting] << ")";
    }
    else {
      leglabel << setting << " (scaled by PID correction histogram)";
    }
	
    leg->AddEntry(hFoldedSpectra[i_setting], leglabel.str().c_str(), "l");
  }
  leg->Draw();

  std::string diff_histname = "hDiff";
  TH1F* hDiff = (TH1F*) hFoldedSpectra[0]->Clone(diff_histname.c_str());
  hDiff->Add(hFoldedSpectra[1], -1);
  hDiff->SetLineColor(kBlack);

  TLegend* leg_diff = new TLegend(0.65,0.55,0.85,0.85);
  leg_diff->SetBorderSize(0);
  leg_diff->SetTextSize(0.035);
  leg_diff->SetFillColor(kWhite);
  leg_diff->AddEntry(hDiff, "abs(Actual Difference)", "l");
    
  TCanvas* c = new TCanvas();

  std::string allowed_histname = "hAllowed";
  TH1F* hAllowed = (TH1F*) hDiff->Clone(allowed_histname.c_str());
  hAllowed->Reset();
  for (int i_bin = 1; i_bin <= hFoldedSpectra[0]->GetNbinsX(); ++i_bin) {
    std::cout << "Bin #" << i_bin << ": [0] = " << hFoldedSpectra[0]->GetBinContent(i_bin) << " +/- " << hFoldedSpectra[0]->GetBinError(i_bin) << ": [1] = " << hFoldedSpectra[1]->GetBinContent(i_bin) << " +/- " << hFoldedSpectra[1]->GetBinError(i_bin) << std::endl;

    double allowed_diff = std::sqrt(hFoldedSpectra[1]->GetBinError(i_bin)*hFoldedSpectra[1]->GetBinError(i_bin) - hFoldedSpectra[0]->GetBinError(i_bin)*hFoldedSpectra[0]->GetBinError(i_bin));// + 2*hFoldedSpectra[0]->GetBinError(i_bin)*hFoldedSpectra[1]->GetBinError(i_bin));
    if (std::isnan(allowed_diff)) {
      allowed_diff = 0;
    }
    std::cout << "Allowed Difference = " << allowed_diff << std::endl;
    hAllowed->SetBinContent(i_bin, allowed_diff);

    hDiff->SetBinContent(i_bin, std::fabs(hDiff->GetBinContent(i_bin)));
  }
  hAllowed->SetLineColor(kBlue);
  leg_diff->AddEntry(hAllowed, "Allowed Difference", "l");

  std::cout << "AE: " << hDiff << std::endl;
  std::cout << "AE: " << hDiff->GetName() << std::endl;
  hDiff->Draw();
  hAllowed->Draw("HIST SAME");
  leg_diff->Draw();
}
