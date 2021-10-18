void Al100_FinalPlot_UnfoldedSpectra_DifferentMuStopPos_SiLSiR(std::string savedir = "") {

  const int n_settings = 3;
  std::string settings[n_settings] = {"al100-unfolded.root", "unfolded.al100.higherbeamenergy.root", "al50-unfolded.root"};
  std::string labels[n_settings] = {"Al100 (nominal)", "Al100 (closer to back)", "Al50 (nominal)"};
  std::string tags[n_settings] = {"al100_nominal", "al100_midHighE", "al50_nominal"};
  TCanvas* canvases[n_settings] = {};
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    canvases[i_setting] = new TCanvas();
  }
  TH1F* hAverages[n_settings] = {0, 0, 0};
  double n_captured_muons[n_settings] = {134e6*0.609, 134e6*0.609, 163e6*0.609}; // taken fro mtable in paper
  
  const int n_particles = 1;
  std::string particles[n_particles] = {"proton"};//, "deuteron", "triton", "alpha"};

  const int n_arms = 2;
  std::string armnames[n_arms] = {"SiR", "SiL"};
  Int_t colours[n_arms] = {kRed, kBlue};

  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = settings[i_setting];
    //      Int_t colour = colours[i_setting];
    canvases[i_setting]->cd();

    TH1F* hAvg = 0;

    TLegend* leg = new TLegend(0.45, 0.5, 0.75, 0.7);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.04);
    leg->SetFillColor(kWhite);

    std::string infilename = "/data/ssd3/R15bAnalysis/hists/" + setting;
    TFile* infile = new TFile(infilename.c_str(), "READ");

    for (int i_arm = 0; i_arm < n_arms;  ++i_arm) {
      std::string armname = armnames[i_arm];
      Int_t colour = colours[i_arm];
      //  TCanvas* c = new TCanvas();
            
      for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
	std::string particle = particles[i_particle];

	std::string histname = "h" + particle + "_" + armname;
	std::cout << histname << std::endl;
	TH1D* hist = (TH1D*) infile->Get(histname.c_str());
	hist->SetDirectory(0);
	hist->SetLineColor(colour);
	hist->GetYaxis()->SetTitle("Unfolded Count / 0.5 MeV");
	hist->GetXaxis()->SetTitle("Unfolded Energy [MeV]");
	if (i_setting == 2) {
	  hist->GetYaxis()->SetRangeUser(0,250e3);
	}
	hist->GetXaxis()->SetRangeUser(0,20);
	hist->Draw("HIST E SAME");

	leg->AddEntry(hist, armname.c_str(), "l");

	if (!hAvg) {
	  hAvg = (TH1F*) hist->Clone("hAvg");
	  hAvg->SetLineColor(kBlack);
	}
	else {
	  hAvg->Add(hist);
	}
      }
    }
    hAvg->Scale(0.5);
    hAvg->Draw("HIST E SAME");
    leg->AddEntry(hAvg, "Avg.", "l");
    leg->Draw();
    std::string clonename = "hAvg_" + tags[i_setting];
    hAverages[i_setting] = (TH1F*) hAvg->Clone(clonename.c_str());

    TLatex* latex = new TLatex();
    std::string text = labels[i_setting];
    latex->SetTextAlign(22);
    latex->DrawLatexNDC(0.50, 0.80, text.c_str());
    latex->SetTextSize(0.03);
    latex->DrawLatexNDC(0.50, 0.75, "stat. errors only");

    // TCanvas* c2 = new TCanvas();
    // for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    //   TH1F* hDiff = (TH1F*) hAvg->Clone("hDiff");
    //   std::string histname = "ResponseMatrix_" + armnames[i_arm] + "/hCorrectedSpectrum";
    //   std::cout << histname << std::endl;
    //   TH1D* hist = (TH1D*) infile->Get(histname.c_str());
    //   hDiff->Divide(hist);
    //   hDiff->SetLineColor(colours[i_arm]);
    //   hDiff->SetYTitle("Avg. / Arm");
    //   hDiff->GetYaxis()->SetRangeUser(0.5, 1.5);
    //   hDiff->Draw("HIST E SAME");
    // }
    // latex->SetTextSize(0.04);
    // latex->DrawLatexNDC(0.50, 0.80, text.c_str());
  }

  TCanvas* c_all_avg = new TCanvas();
  Int_t set_colours[n_settings] = {kRed, kMagenta, kBlue};
  TLegend* leg2 = new TLegend(0.45, 0.5, 0.75, 0.7);
  leg2->SetBorderSize(0);
  leg2->SetTextSize(0.04);
  leg2->SetFillColor(kWhite);
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    hAverages[i_setting]->SetTitle("Averages");
    hAverages[i_setting]->SetTitle("Yield / Captured Muon / 0.5 MeV");
    hAverages[i_setting]->SetLineColor(set_colours[i_setting]);
    hAverages[i_setting]->Scale(1.0 / n_captured_muons[i_setting]);
    hAverages[i_setting]->GetYaxis()->SetRangeUser(0,0.002);
    hAverages[i_setting]->Draw("HIST E SAME");
    leg2->AddEntry(hAverages[i_setting], labels[i_setting].c_str(), "l");
  }
  TLatex* latex = new TLatex();
  latex->SetTextAlign(22);
  latex->DrawLatexNDC(0.50, 0.80, "SiL - SiR Averages");
  latex->SetTextSize(0.03);
  latex->DrawLatexNDC(0.50, 0.75, "stat. errors only");
  leg2->Draw();
}
