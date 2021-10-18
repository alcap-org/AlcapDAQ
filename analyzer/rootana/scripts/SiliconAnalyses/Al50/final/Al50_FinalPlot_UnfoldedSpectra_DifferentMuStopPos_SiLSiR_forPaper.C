void Al50_FinalPlot_UnfoldedSpectra_DifferentMuStopPos_SiLSiR_forPaper(std::string savedir = "") {

  const int n_settings = 3;
  std::string settings[n_settings] = {"", "_midLowE", "_midHighE"};
  std::string labels[n_settings] = {"nominal stopping depth", "stopping closer to front of target", "stopping closer to back of target"};
  Int_t setting_colours[n_settings] = {kBlack, kRed, kBlue};
  TCanvas* canvases[n_settings] = {};
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    canvases[i_setting] = new TCanvas();
  }

  TLegend* leg_forAll = new TLegend(0.35, 0.60, 0.70, 0.80);
  leg_forAll->SetBorderSize(0);
  leg_forAll->SetTextSize(0.04);
  leg_forAll->SetFillColor(kWhite);

  const int n_particles = 1;
  std::string particles[n_particles] = {"proton"};//, "deuteron", "triton", "alpha"};

  const int n_arms = 2;
  std::string armnames[n_arms] = {"SiR", "SiL"};
  Int_t colours[n_arms] = {kRed, kBlue};

  TCanvas* c_all = new TCanvas();
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = settings[i_setting];
    //      Int_t colour = colours[i_setting];
    canvases[i_setting]->cd();

    TH1F* hAvg = 0;

    TLegend* leg = new TLegend(0.35, 0.5, 0.70, 0.7);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.04);
    leg->SetFillColor(kWhite);

    std::string infilename = "~/data/results/Al50/unfold_fromMarkHists" + setting + ".root";
    TFile* infile = new TFile(infilename.c_str(), "READ");

    for (int i_arm = 0; i_arm < n_arms;  ++i_arm) {
      std::string armname = armnames[i_arm];
      Int_t colour = colours[i_arm];
      //  TCanvas* c = new TCanvas();
            
      for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
	std::string particle = particles[i_particle];

	std::string histname = "ResponseMatrix_" + armname + "/hCorrectedSpectrum";
	std::cout << histname << std::endl;
	TH1D* hist = (TH1D*) infile->Get(histname.c_str());
	hist->SetDirectory(0);
	hist->SetLineColor(colour);
	hist->GetYaxis()->SetTitle("Unfolded Count / 0.5 MeV");
	hist->GetXaxis()->SetTitle("Unfolded Energy [MeV]");
	hist->GetYaxis()->SetRangeUser(0,350e3);
	hist->GetXaxis()->SetRangeUser(0,15);
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
    hAvg->Draw("HIST SAME");
    leg->AddEntry(hAvg, "Avg.", "l");
    leg->Draw();

    TLatex* latex = new TLatex();
    std::string text = labels[i_setting];
    latex->SetTextAlign(22);
    latex->DrawLatexNDC(0.50, 0.80, text.c_str());
    latex->SetTextSize(0.03);
    latex->DrawLatexNDC(0.50, 0.75, "stat. errors only");

    c_all->cd();
    TH1F* hAvg_forAll = (TH1F*) hAvg->Clone("hAvg_forAll");
    hAvg_forAll->SetLineColor(setting_colours[i_setting]);
    hAvg_forAll->GetYaxis()->SetRangeUser(0,250e3);
    hAvg_forAll->GetXaxis()->SetRangeUser(0,15);
    hAvg_forAll->Draw("HIST SAME");
    leg_forAll->AddEntry(hAvg_forAll, labels[i_setting].c_str(), "l");
  }
  c_all->cd();
  leg_forAll->SetHeader("AlCap, Al, Average of Detector Arms");
  leg_forAll->Draw();

  //  TLatex* latex = new TLatex();
  //  std::string text = "SiL - SiR Averages";
  //  latex->SetTextAlign(22);
  //  latex->DrawLatexNDC(0.50, 0.80, text.c_str());
  //  latex->SetTextSize(0.03);

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_Al50Dataset_UnfoldedSpectra_DifferentMuStopPos_SiLSiR_forPaper";
    std::string pdfname = savename + ".pdf";
    c_all->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c_all->SaveAs(pngname.c_str());
  }
}
