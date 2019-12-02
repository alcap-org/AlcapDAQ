void Si16b_FinalPlot_PIDEffs_Example() {

  TFile* file = new TFile("~/data/mc/Si16b/pid-effs.root", "READ");

  double x_min = 0;
  double x_max = 2000;//10000;

  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  Int_t colours[n_particles] = {kRed, kCyan, kMagenta, kSpring};
  
  double energy = 15500;
  double bin_width = 1000;
  double half_bin_width = 0.5*bin_width;
  int layercfg = 0;

  TCanvas* c_example = new TCanvas("c_example", "c_example");
  TCanvas* c_effs = new TCanvas("c_effs", "c_effs");
  TCanvas* c_purities = new TCanvas("c_purities", "c_purities");
  
  TLatex* latex = new TLatex();
  latex->SetTextAlign(11);
  latex->SetTextSize(0.035);
  std::stringstream text;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    Int_t colour = colours[i_particle];

    c_effs->cd();
    std::string effhistname = "eff_" + particle;
    TH1F* hEff = (TH1F*) file->Get(effhistname.c_str());
    hEff->SetTitle("PID Cut Efficiencies");
    hEff->SetLineWidth(2);
    hEff->SetLineColor(colour);
    hEff->SetStats(false);
    hEff->Draw("HIST E SAME");
    double eff = hEff->GetBinContent(hEff->FindBin(energy));
    double eff_err = hEff->GetBinError(hEff->FindBin(energy));
    std::cout << eff << " +/- " << eff_err << std::endl;

    c_purities->cd();
    std::string purityhistname = "purity_" + particle;
    TH1F* hPurity = (TH1F*) file->Get(purityhistname.c_str());
    hPurity->SetTitle("PID Cut Purities");
    hPurity->SetLineWidth(2);
    hPurity->SetLineColor(colour);
    hPurity->SetStats(false);
    hPurity->Draw("HIST E SAME");
    double purity = hPurity->GetBinContent(hPurity->FindBin(energy));
    double purity_err = hPurity->GetBinError(hPurity->FindBin(energy));
    std::cout << purity << " +/- " << purity_err << std::endl;

    c_example->cd();
    std::stringstream histtitle;
    histtitle.str("");
    histtitle << energy-half_bin_width << " keV < E_{1} + E_{2} < " << energy+half_bin_width << " keV";

    std::stringstream datahistname;
    datahistname.str("");
    datahistname << "data_" << particle << "_layercfg" << layercfg << "_E" << energy << "_py";
    TH1F* hDataSlice = (TH1F*) file->Get(datahistname.str().c_str());
    if (!hDataSlice) {
      std::cout << "Couldn't find data histogram " << datahistname.str() << std::endl;
      return;
    }
    hDataSlice->SetTitle(histtitle.str().c_str());
    hDataSlice->SetStats(false);
    hDataSlice->SetLineColor(colour);
    hDataSlice->GetFunction("data_fit")->SetLineColor(colour);
    
    std::stringstream mchistname;
    mchistname.str("");
    mchistname << "mc_" << particle << "_layercfg" << layercfg << "_E" << energy << "_py";
    TH1F* hMCSlice = (TH1F*) file->Get(mchistname.str().c_str());
    if (!hMCSlice) {
      std::cout << "Couldn't find mc histogram " << mchistname.str() << std::endl;
      return;
    }
    hMCSlice->SetTitle(histtitle.str().c_str());
    hMCSlice->SetStats(false);
    hMCSlice->SetLineColor(colour);

    hDataSlice->GetXaxis()->SetRangeUser(x_min, x_max);
    hMCSlice->GetXaxis()->SetRangeUser(x_min, x_max);

    //  hMCSlice->Draw("E");
    hDataSlice->Draw("E SAME");
    
    TF1* full_fit_fn = (TF1*) hDataSlice->GetFunction("data_fit")->Clone("full_fit_fn");
    full_fit_fn->SetLineStyle(kDashed);
    full_fit_fn->SetRange(x_min, x_max);
    full_fit_fn->Draw("LSAME");

    latex->SetTextColor(colour);
    text.str("");
    text << std::setprecision(2) << "#splitline{eff = " << eff << " #pm " << eff_err << "}{purity = " << purity << " #pm" << purity_err << "}";
    latex->DrawLatex(full_fit_fn->GetParameter(1)+100, hDataSlice->GetMaximum(), text.str().c_str());
  }
}
