void Al50_FinalPlot_UnfoldedSpectra_DifferentMuStopPos(std::string savedir = "") {

  const int n_settings = 3;
  std::string settings[n_settings] = {"", "_midLowE", "_midHighE"};
  std::string labels[n_settings] = {"nominal", "closer to front", "closer to back"};
  Int_t colours[n_settings] = {kBlack, kRed, kBlue};
  
  const int n_particles = 1;
  std::string particles[n_particles] = {"proton"};//, "deuteron", "triton", "alpha"};

  const int n_arms = 2;
  std::string armnames[n_arms] = {"SiR", "SiL"};

  for (int i_arm = 0; i_arm < n_arms;  ++i_arm) {
    std::string armname = armnames[i_arm];

    TCanvas* c = new TCanvas();

    TLegend* leg = new TLegend(0.35, 0.5, 0.75, 0.8);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.04);
    leg->SetFillColor(kWhite);

    for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
      std::string setting = settings[i_setting];
      Int_t colour = colours[i_setting];

      std::string infilename = "~/data/results/Al50/unfold_fromMarkHists" + setting + ".root";
      TFile* infile = new TFile(infilename.c_str(), "READ");
      
      for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
	std::string particle = particles[i_particle];

	std::string histname = "ResponseMatrix_" + armname + "/hCorrectedSpectrum";
	std::cout << histname << std::endl;
	TH1D* hist = (TH1D*) infile->Get(histname.c_str());
	hist->SetDirectory(0);
	hist->SetLineColor(colour);
	hist->GetYaxis()->SetTitle("Unfolded Count / 0.5 MeV");
	hist->GetXaxis()->SetTitle("Unfolded Energy [MeV]");
	hist->GetYaxis()->SetRangeUser(0,400e3);
	hist->GetXaxis()->SetRangeUser(0,20);
	hist->Draw("HIST E SAME");

	leg->AddEntry(hist, labels[i_setting].c_str(), "l");
      }
    }
    leg->Draw();

    TLatex* latex = new TLatex();
    std::string text = armname;
    latex->DrawLatexNDC(0.40, 0.80, text.c_str());

  }

}
