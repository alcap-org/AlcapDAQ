void Si16b_MichelElectrons() {
  std::string filename = "~/data/results/Si16b_passive/subtrees.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  std::string treename = "siBlockTree_SiR";
  TTree* SiRTree = (TTree*) file->Get(treename.c_str());

  double min_energy = 0;
  double max_energy = 2000;
  double energy_width = 50;
  int n_energy_bins = (max_energy - min_energy) / energy_width;

  double min_time = -10000;
  double max_time = 10000;
  double time_width = 50;
  int n_time_bins = (max_time - min_time) / time_width;

  TH2F* hEvst = new TH2F("hEvst_Michel", "", n_time_bins,min_time,max_time, n_energy_bins,min_energy,max_energy);
  SiRTree->Draw("thick_energy:thick_time>>hEvst_Michel", "thin_tpi_id<0 && thick_tpi_id>=0", "COLZ");

  double min_michel_energy = 600;
  double max_michel_energy = 650;
  int min_michel_energy_bin = hEvst->GetYaxis()->FindBin(min_michel_energy);
  int max_michel_energy_bin = hEvst->GetYaxis()->FindBin(max_michel_energy);
  TH1D* hLifetime = hEvst->ProjectionX("hLifetime", min_michel_energy_bin, max_michel_energy_bin);
  hLifetime->Rebin(2);
  hLifetime->GetXaxis()->SetRangeUser(-1000, 10000);
  hLifetime->Draw("HIST E");

  double min_fit_time = 200;
  double max_fit_time = 10000;
  TF1* muonic_atom_lifetime = new TF1("muonic_atom_lifetime", "[0]*TMath::Exp(-x/[1]) + [2]", min_fit_time, max_fit_time);
  muonic_atom_lifetime->SetParameters(1000, 700, 100);
  hLifetime->Fit(muonic_atom_lifetime, "R");
  muonic_atom_lifetime->Draw("LSAME");
}
