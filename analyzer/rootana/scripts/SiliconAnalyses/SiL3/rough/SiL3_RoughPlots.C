void SiL3_RoughPlots() {

  gStyle->SetOptFit(1);

  std::string sil3_filename = "~/data/results/SiL3/plots_wTgtMuon.root";
  TFile* sil3_file = new TFile(sil3_filename.c_str(), "READ");

  TH2F* hFull = (TH2F*) sil3_file->Get("Target/hEnergyTime");

  int rebin_factor = 20;
  double min_time_axis = -20000;
  double max_time_axis = 20000;

  TCanvas* c_muon = new TCanvas("c_muon", "c_muon");
  c_muon->SetLogy();
  double min_muon_energy = 3500;
  double max_muon_energy = 4500;
  int min_muon_energy_bin = hFull->GetYaxis()->FindBin(min_muon_energy);
  int max_muon_energy_bin = hFull->GetYaxis()->FindBin(max_muon_energy)-1;
  TH1F* hMuonTime = (TH1F*) hFull->ProjectionX("hMuonTime", min_muon_energy_bin, max_muon_energy_bin);
  hMuonTime->Sumw2();
  hMuonTime->Rebin(rebin_factor);
  hMuonTime->GetXaxis()->SetRangeUser(min_time_axis,max_time_axis);
  double min_muon_fit_time = 10000;
  double max_muon_fit_time = 20000;
  TF1* muon_fit = new TF1("muon_fit", "[0]*TMath::Exp(-x/[1])", min_muon_fit_time, max_muon_fit_time);
  muon_fit->SetParameters(100,1000);
  hMuonTime->Fit(muon_fit, "LR");
  hMuonTime->Draw("HIST E");
  muon_fit->Draw("LSAME");

  TCanvas* c_flat = new TCanvas("c_flat", "c_flat");
  c_flat->SetLogy();
  double min_flat_energy = 500;
  double max_flat_energy = 1500;
  int min_flat_energy_bin = hFull->GetYaxis()->FindBin(min_flat_energy);
  int max_flat_energy_bin = hFull->GetYaxis()->FindBin(max_flat_energy)-1;
  TH1F* hFlatTime = (TH1F*) hFull->ProjectionX("hFlatTime", min_flat_energy_bin, max_flat_energy_bin);
  hFlatTime->Sumw2();
  hFlatTime->Rebin(rebin_factor);
  hFlatTime->GetXaxis()->SetRangeUser(min_time_axis,max_time_axis);
  double min_flat_fit_time = 10000;
  double max_flat_fit_time = 20000;
  TF1* flat_fit = new TF1("flat_fit", "[0]*x+[1]", min_flat_fit_time, max_flat_fit_time);
  flat_fit->SetParameters(1,100);
  hFlatTime->Fit(flat_fit, "LR");
  hFlatTime->Draw("HIST E");
  flat_fit->Draw("LSAME");


  std::cout << "AE: hFull, Time Bin Width = " << hFull->GetXaxis()->GetBinWidth(1) << std::endl;
  std::cout << "AE: hFull, Energy Bin Width = " << hFull->GetYaxis()->GetBinWidth(1) << std::endl;

  TCanvas* c_full = new TCanvas("c_full", "c_full");
  c_full->SetLogy(1);
  double min_full_energy = 1000;
  double max_full_energy = 30000;
  int min_full_energy_bin = hFull->GetYaxis()->FindBin(min_full_energy);
  int max_full_energy_bin = hFull->GetYaxis()->FindBin(max_full_energy)-1;
  TH1F* hFullTime = (TH1F*) hFull->ProjectionX("hFullTime", min_full_energy_bin, max_full_energy_bin);
  hFullTime->Sumw2();
  hFullTime->Rebin(rebin_factor);
  hFullTime->GetXaxis()->SetRangeUser(min_time_axis,max_time_axis);
  double min_fit_time = 2000;
  double max_fit_time = 3800;
  //  double min_fit_time = 5000;
  //  double max_fit_time = 10000;

  //  std::string equation = "[0]*TMath::Exp(-x/[1]) + [2]*x + [3]";
  //  std::string equation = "[0]*TMath::Exp(-x/[1]) + [2]";
  std::string equation = "[0]*TMath::Exp(-x/[1]) + [2]*TMath::Exp(-x/[3]) + [4]*x + [5]";
  TF1* muonic_atom_fit = new TF1("muonic_atom_fit", equation.c_str(), min_fit_time, max_fit_time);
  muonic_atom_fit->SetParameter(0, 1000);
  muonic_atom_fit->SetParameter(1, 100);
  //  muonic_atom_fit->FixParameter(1, 756);
  muonic_atom_fit->SetParameter(2, muon_fit->GetParameter(0));
  muonic_atom_fit->SetParError(2, muon_fit->GetParError(0));
  //  muonic_atom_fit->FixParameter(2, muon_fit->GetParameter(0));
  muonic_atom_fit->SetParameter(3, muon_fit->GetParameter(1));
  muonic_atom_fit->SetParError(3, muon_fit->GetParError(1));
  muonic_atom_fit->FixParameter(3, muon_fit->GetParameter(1));
  muonic_atom_fit->SetParameter(4, flat_fit->GetParameter(0));
  muonic_atom_fit->SetParError(4, flat_fit->GetParError(0));
  //  muonic_atom_fit->FixParameter(4, flat_fit->GetParameter(0));
  muonic_atom_fit->SetParameter(5, flat_fit->GetParameter(1));
  muonic_atom_fit->SetParError(5, flat_fit->GetParError(1));
  //  muonic_atom_fit->FixParameter(5, flat_fit->GetParameter(1));
  muonic_atom_fit->SetParName(1, "Lifetime");
  hFullTime->Fit(muonic_atom_fit, "R");
  hFullTime->Draw("HIST E");
  muonic_atom_fit->Draw("LSAME");

  
  TF1* full_muonic_atom_fit = new TF1("full_muonic_atom_fit", equation.c_str(), min_fit_time, max_muon_fit_time);
  full_muonic_atom_fit->SetParameter(0, muonic_atom_fit->GetParameter(0));
  full_muonic_atom_fit->SetParError(0, muonic_atom_fit->GetParError(0));
  full_muonic_atom_fit->SetParameter(1, muonic_atom_fit->GetParameter(1));
  full_muonic_atom_fit->SetParError(1, muonic_atom_fit->GetParError(1));
  full_muonic_atom_fit->SetParameter(2, muonic_atom_fit->GetParameter(2));
  full_muonic_atom_fit->SetParError(2, muonic_atom_fit->GetParError(2));
  full_muonic_atom_fit->SetParameter(3, muonic_atom_fit->GetParameter(3));
  full_muonic_atom_fit->SetParError(3, muonic_atom_fit->GetParError(3));
  full_muonic_atom_fit->SetParameter(4, muonic_atom_fit->GetParameter(4));
  full_muonic_atom_fit->SetParError(4, muonic_atom_fit->GetParError(4));
  full_muonic_atom_fit->SetParameter(5, muonic_atom_fit->GetParameter(5));
  full_muonic_atom_fit->SetParError(5, muonic_atom_fit->GetParError(5));
  full_muonic_atom_fit->SetParName(1, "Lifetime");
  full_muonic_atom_fit->SetLineStyle(kDashed);
  full_muonic_atom_fit->Draw("LSAME");


  TLatex* latex = new TLatex();
  latex->SetTextSize(0.03);
  latex->DrawLatexNDC(0.15, 0.6, equation.c_str());
  
}
