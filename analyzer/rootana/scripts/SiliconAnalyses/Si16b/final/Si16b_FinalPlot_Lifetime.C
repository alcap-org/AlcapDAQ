void Si16b_FinalPlot_Lifetime() {

  std::string infilename = "~/data/results/Si16b_passive/lifetime.root";
  std::string inhistname = "Proton_wTimeCut/hTime";
  std::string infitname = "Proton_wTimeCut/muonic_atom_lifetime";
  
  TFile* infile = new TFile(infilename.c_str(), "READ");
  TH1F* hLifetime = (TH1F*) infile->Get(inhistname.c_str());
  TF1* fit = (TF1*) infile->Get(infitname.c_str());

  TCanvas* c_Lifetime = new TCanvas("c_Lifetime", "c_Lifetime");
  c_Lifetime->SetLogy();

  hLifetime->SetMarkerStyle(kFullCircle);
  hLifetime->SetMarkerColor(kBlack);
  hLifetime->SetLineColor(kBlack);
  hLifetime->SetTitle("Si16b Dataset, Right Arm, Protons");
  hLifetime->SetStats(false);
  //  gStyle->SetOptFit(1);
  hLifetime->GetXaxis()->SetRangeUser(0, 10000);
  hLifetime->Draw("P E");

  fit->Draw("SAME");

  TLatex* latex = new TLatex();

  std::stringstream fit_pars;
  fit_pars << std::setprecision(3) << "#tau = " << fit->GetParameter(1) << " #pm " << std::setprecision(1) << fit->GetParError(1) << " ns";
  latex->DrawLatexNDC(0.5, 0.5, fit_pars.str().c_str());
  
  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
}
