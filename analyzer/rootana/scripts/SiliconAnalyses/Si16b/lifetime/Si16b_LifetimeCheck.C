void Si16b_LifetimeCheck() {

  std::string infilename = "~/data/results/Si16b_passive/lifetime.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");
  
  std::string base_indirname = "Proton_wTimeCut";
  const int n_exps = 2;
  std::string exp_strs[n_exps] = {"_SingleExp", "_DoubleExp"};
  const int n_flats = 2;
  std::string flat_strs[n_flats] = {"", "_FlatBkg"};
  Int_t colours[n_flats*n_exps] = {kRed, kBlue, kBlack, kMagenta};

  TCanvas* c_Lifetime = new TCanvas("c_Lifetime", "c_Lifetime");
  c_Lifetime->SetLogy();

  TLegend* leg = new TLegend(0.5,0.55,0.9,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  
  for (int i_exp = 0; i_exp < n_exps; ++i_exp) {
    for (int i_flat = 0; i_flat < n_flats; ++i_flat) {
      std::string indirname = base_indirname + exp_strs[i_exp] + flat_strs[i_flat];
      std::string inhistname = indirname + "/hTime";
      std::string infitname = indirname + "/muonic_atom_lifetime";

      TH1F* hLifetime = (TH1F*) infile->Get(inhistname.c_str());
      TF1* fit = (TF1*) infile->Get(infitname.c_str());
      std::cout << "AE: " << fit << std::endl;
      if (i_exp == 0 && i_flat == 0) {
	hLifetime->SetMarkerStyle(kFullCircle);
	hLifetime->SetMarkerColor(kBlack);
	hLifetime->SetLineColor(kBlack);
	hLifetime->SetTitle("Si16b Dataset, Right Arm, Protons");
	hLifetime->SetStats(false);
	//  gStyle->SetOptFit(1);
	hLifetime->GetXaxis()->SetRangeUser(0, 10000);
	hLifetime->Draw("P E");
      }

      fit->SetLineStyle(kDashed);
      fit->SetLineColor(colours[i_flat + 2*i_exp]);
      fit->Draw("LSAME");
      
      
      std::stringstream leglabel;
      leglabel << exp_strs[i_exp] << " " << flat_strs[i_flat] << ": " << std::setprecision(3) << "#tau = " << fit->GetParameter(1) << " #pm " << std::setprecision(1) << fit->GetParError(1) << " ns";
      if (i_exp == 1) {
	leglabel << std::setprecision(3) << ", #tau = " << fit->GetParameter(3) << " #pm " << std::setprecision(3) << fit->GetParError(1) << " ns";
      }
      std::cout << "AE: " << leglabel.str() << std::endl;
      leg->AddEntry(fit, leglabel.str().c_str(), "l");
    }
  }

  leg->Draw();
}
