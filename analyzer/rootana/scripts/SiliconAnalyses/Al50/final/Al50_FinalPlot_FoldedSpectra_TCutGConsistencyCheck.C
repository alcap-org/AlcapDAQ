void Al50_FinalPlot_FoldedSpectra_TCutGConsistencyCheck() {

  const int n_particles = 4;
  std::string particles[n_particles] = {"p", "d", "t", "a"};
  //  Int_t colours[n_particles] = {kRed};//, kCyan, kMagenta, kSpring};

  const int n_settings = 2;
  //  std::string settings[n_settings] = {"2", "1"};
  std::string settings[n_settings] = {"2sig", "1sig"};
  Int_t colours[n_settings] = {kRed, kBlue};//, kCyan, kMagenta, kSpring};
  TH1F* hFoldedSpectra[n_settings] = {0};
  double scales[n_settings] = {0.95, 0.68};
  
  int rebin_factor = 100;
  std::stringstream axistitle;

  TLegend* leg = new TLegend(0.50,0.55,0.70,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  //  std::string infilename = "~/data/results/Al50/mark-pid-folded.root";
  std::string infilename = "~/data/results/Al50/mark-pid-folded-correct.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    TCanvas* c_Spectra = new TCanvas();
    
    std::string particle = particles[i_particle];

    for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
      std::string setting = settings[i_setting];
      
      std::string folded_histname = "h" + particle + "SiR_" + setting;
      TH1F* hFoldedSpectrum = (TH1F*) infile->Get(folded_histname.c_str());
      if (!hFoldedSpectrum) {
	std::cout << "Can't find histogram " << folded_histname << std::endl;
	continue;
      }

      //      hFoldedSpectrum->Rebin(rebin_factor);
      hFoldedSpectrum->SetStats(false);
      hFoldedSpectrum->SetLineColor(colours[i_setting]);
      hFoldedSpectrum->SetLineWidth(2);
      hFoldedSpectrum->SetTitle(particle.c_str());
      hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 26);
      hFoldedSpectrum->GetYaxis()->SetTitleOffset(1.);
      axistitle.str("");
      axistitle << "Raw Count / " << hFoldedSpectrum->GetXaxis()->GetBinWidth(1) << " MeV";
      hFoldedSpectrum->SetTitle("Al50 Dataset, Right Arm, Raw Spectrum");
      hFoldedSpectrum->SetYTitle(axistitle.str().c_str());
      hFoldedSpectrum->Scale(1.0 / scales[i_setting]);
      hFoldedSpectrum->Draw("HIST E SAME");

      if (i_particle == 0) {
	std::stringstream leglabel;
	leglabel << setting << "sig (scaled by 1.0/" << std::fixed << std::setprecision(2) << scales[i_setting] << ")";
	leg->AddEntry(hFoldedSpectrum, leglabel.str().c_str(), "l");
      }
      std::string clonename = "clone_" + particle + "_" + setting;
      hFoldedSpectra[i_setting] = (TH1F*) hFoldedSpectrum->Clone(clonename.c_str());
    }
    leg->Draw();

    std::string diff_histname = "hDiff_" + particle;
    TH1F* hDiff = (TH1F*) hFoldedSpectra[0]->Clone(diff_histname.c_str());
    hDiff->Add(hFoldedSpectra[1], -1);
    hDiff->SetLineColor(kBlack);
    hDiff->SetTitle(particle.c_str());

    TLegend* leg_diff = new TLegend(0.65,0.55,0.85,0.85);
    leg_diff->SetBorderSize(0);
    leg_diff->SetTextSize(0.035);
    leg_diff->SetFillColor(kWhite);
    leg_diff->AddEntry(hDiff, "abs(Actual Difference)", "l");
    
    TCanvas* c = new TCanvas();

    std::string allowed_histname = "hAllowed_" + particle;
    TH1F* hAllowed = (TH1F*) hDiff->Clone(allowed_histname.c_str());
    hAllowed->Reset();
    for (int i_bin = 1; i_bin <= hFoldedSpectra[0]->GetNbinsX(); ++i_bin) {
      //      std::cout << "Bin #" << i_bin << ": 2sig = " << hFoldedSpectra[0]->GetBinContent(i_bin) << " +/- " << hFoldedSpectra[0]->GetBinError(i_bin) << ": 1sig = " << hFoldedSpectra[1]->GetBinContent(i_bin) << " +/- " << hFoldedSpectra[1]->GetBinError(i_bin) << std::endl;
      double allowed_diff = std::sqrt(hFoldedSpectra[0]->GetBinError(i_bin)*hFoldedSpectra[0]->GetBinError(i_bin) - hFoldedSpectra[1]->GetBinError(i_bin)*hFoldedSpectra[1]->GetBinError(i_bin) + 2*hFoldedSpectra[0]->GetBinError(i_bin)*hFoldedSpectra[1]->GetBinError(i_bin));
      //      std::cout << "Allowed Difference = " << allowed_diff << std::endl;
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
}
