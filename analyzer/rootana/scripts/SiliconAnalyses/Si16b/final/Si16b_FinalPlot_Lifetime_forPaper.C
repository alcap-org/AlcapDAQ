void Si16b_FinalPlot_Lifetime_forPaper() {

  std::string infilename = "~/data/results/Si16b/plots_newPP_geq1TgtPulse_1.root";
  std::string inhistname = "proton_2sig_SiR_timecut0_10000ns_layerCoinc500ns/hThickTime_TwoLayer_12not3";
  
  TFile* infile = new TFile(infilename.c_str(), "READ");
  TH1F* hLifetime = (TH1F*) infile->Get(inhistname.c_str());
  //  TF1* fit = (TF1*) infile->Get(infitname.c_str());

  TCanvas* c_Lifetime = new TCanvas("c_Lifetime", "c_Lifetime");
  c_Lifetime->SetLogy();

  hLifetime->SetMarkerStyle(kFullCircle);
  hLifetime->SetMarkerColor(kBlack);
  hLifetime->SetLineColor(kBlack);
  hLifetime->SetTitle("Si16b Dataset, Right Arm, Protons");
  hLifetime->SetStats(false);
  gStyle->SetOptFit(1);
  hLifetime->GetXaxis()->SetRangeUser(0, 10000);
  hLifetime->Draw("P E");

  TF1* fit_fn = new TF1("fit_fn", "[0]*TMath::Exp(-x/[1]) + [2]", 0, 10000);// + [2]");
  fit_fn->SetParameters(1000, 700, 0.1);
  hLifetime->Fit(fit_fn, "LR");

  fit_fn->SetLineColor(kRed);
  fit_fn->Draw("LSAME");

  TLatex* latex = new TLatex();

  // std::stringstream fit_pars;
  // fit_pars << std::setprecision(3) << "#tau = " << fit->GetParameter(1) << " #pm " << std::setprecision(1) << fit->GetParError(1) << " ns";
  // latex->DrawLatexNDC(0.5, 0.5, fit_pars.str().c_str());
  
  // latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
}
