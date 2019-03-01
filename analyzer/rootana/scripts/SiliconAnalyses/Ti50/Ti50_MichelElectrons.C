void Ti50_MichelElectrons() {
  std::string filename = "~/data/results/Ti50/subtrees.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  std::string treename = "siBlockTree_SiL";
  TTree* SiLTree = (TTree*) file->Get(treename.c_str());

  double min_energy = 0;
  double max_energy = 1400;
  double energy_width = 50;
  int n_energy_bins = (max_energy - min_energy) / energy_width;

  double min_time = -1000;
  double max_time = 10000;
  double time_width = 50;
  int n_time_bins = (max_time - min_time) / time_width;

  std::string histtitle = "Ti50 Dataset";
  std::string cutcmd = "thin_tpi_id<0 && thick_tpi_id>=0";// && third_tpi_id>=0";
  TH2F* hE2vsE3 = new TH2F("hE2vsE3_Michel", histtitle.c_str(), n_energy_bins,min_energy,max_energy, n_energy_bins,min_energy,max_energy);
  hE2vsE3->SetYTitle("SiL2 Energy [keV]");
  hE2vsE3->SetXTitle("SiL3 Energy [keV]");
  SiLTree->Draw("thick_energy:third_energy>>hE2vsE3_Michel", cutcmd.c_str(), "COLZ");
  
  TH2F* hE3vst3 = new TH2F("hE3vst3_Michel", histtitle.c_str(), n_time_bins,min_time,max_time, n_energy_bins,min_energy,max_energy);
  hE3vst3->SetYTitle("SiL3 Energy [keV]");
  hE3vst3->SetXTitle("SiL3 Time [ns]");
  SiLTree->Draw("third_energy:third_time>>hE3vst3_Michel", cutcmd.c_str(), "COLZ");

  TH2F* hE2vst2 = new TH2F("hE2vst2_Michel", histtitle.c_str(), n_time_bins,min_time,max_time, n_energy_bins,min_energy,max_energy);
  hE2vst2->SetYTitle("SiL3 Energy [keV]");
  hE2vst2->SetXTitle("SiL3 Time [ns]");
  SiLTree->Draw("thick_energy:thick_time>>hE2vst2_Michel", cutcmd.c_str(), "COLZ");

  double min_michel_energy = 600;
  double max_michel_energy = 650;
  //  double min_michel_energy = 350;
  //  double max_michel_energy = 400;
  int min_michel_energy_bin = hE2vst2->GetYaxis()->FindBin(min_michel_energy);
  int max_michel_energy_bin = hE2vst2->GetYaxis()->FindBin(max_michel_energy);
  TH1D* hLifetime = hE2vst2->ProjectionX("hLifetime", min_michel_energy_bin, max_michel_energy_bin);
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
