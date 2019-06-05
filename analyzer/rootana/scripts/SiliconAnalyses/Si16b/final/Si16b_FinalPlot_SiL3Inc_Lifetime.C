void Si16b_FinalPlot_SiL3Inc_Lifetime() {

  std::string infilename = "~/data/results/Si16b/lifetime.root";
  std::string dirname = "SiL3_10000keVCut_wFlatBkg";
  std::string inhistname = dirname + "/hTime";
  std::string infitname = dirname + "/muonic_atom_lifetime";
  
  TFile* infile = new TFile(infilename.c_str(), "READ");
  TH1F* hLifetime = (TH1F*) infile->Get(inhistname.c_str());
  TF1* fit = (TF1*) infile->Get(infitname.c_str());

  TCanvas* c_Lifetime = new TCanvas("c_Lifetime", "c_Lifetime");
  c_Lifetime->SetLogy();

  hLifetime->SetMarkerStyle(kFullCircle);
  hLifetime->SetMarkerColor(kBlack);
  hLifetime->SetLineColor(kBlack);
  hLifetime->SetTitle("Si16b Dataset, SiL3 Inclusive Analysis, E > 10000 keV (with flat background)");
  hLifetime->SetStats(false);
  //  gStyle->SetOptFit(1);
  hLifetime->GetXaxis()->SetRangeUser(0, 20000);
  hLifetime->Draw("P E");

  fit->Draw("SAME");

  TLatex* latex = new TLatex();
  latex->SetTextSize(0.04);
  
  std::stringstream fit_pars;
  fit_pars << std::fixed << std::setprecision(0) << "#tau = " << fit->GetParameter(1) << " #pm " << std::setprecision(0) << fit->GetParError(1) << " ns";
  latex->DrawLatexNDC(0.5, 0.5, fit_pars.str().c_str());
  
  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
}
