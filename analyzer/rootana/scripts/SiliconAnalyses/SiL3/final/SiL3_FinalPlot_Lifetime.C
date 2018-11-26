void SiL3_FinalPlot_Lifetime() {

  std::string infilename = "~/data/results/SiL3/lifetime.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");
   
  std::string base_indirname = "Target_2000keVCut";
    
  TCanvas* c_Lifetime = new TCanvas("c_Lifetime", "c_Lifetime");
  c_Lifetime->SetLogy();
  
  TLegend* leg = new TLegend(0.5,0.55,0.9,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
    
  std::string indirname = base_indirname;
  std::string inhistname = indirname + "/hTime";
  std::string infitname = indirname + "/muonic_atom_lifetime";
  
  TH1F* hLifetime = (TH1F*) infile->Get(inhistname.c_str());
  TF1* fit = (TF1*) infile->Get(infitname.c_str());
  hLifetime->SetMarkerStyle(kFullCircle);
  hLifetime->SetMarkerColor(kBlack);
  hLifetime->SetLineColor(kBlack);
  //  hLifetime->SetTitle("SiL3 Dataset, Target, E > 5000 keV (with flat bkg)");
  hLifetime->SetStats(false);
  //  gStyle->SetOptFit(1);
  hLifetime->GetXaxis()->SetRangeUser(0, 10000);
  hLifetime->SetMinimum(0.5);
  hLifetime->SetMaximum(2e7);
  hLifetime->GetFunction("muonic_atom_lifetime")->SetBit(TF1::kNotDraw);
  hLifetime->Draw("P E");
  
  fit->Draw("LSAME");
    
    
  std::stringstream leglabel;
  leglabel << std::setprecision(3) << "#tau = " << fit->GetParameter(1) << " #pm " << std::setprecision(2) << fit->GetParError(1) << " ns";
  std::cout << "AE: " << leglabel.str() << std::endl;
  leg->AddEntry(fit, leglabel.str().c_str(), "l");
  
  leg->Draw();
}
