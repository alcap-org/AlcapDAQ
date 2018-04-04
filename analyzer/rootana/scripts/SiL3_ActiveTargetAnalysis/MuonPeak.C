void MuonPeak() {

  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run9040.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  TH2F* hTimeEnergy = (TH2F*) file->Get("hTimeEnergy");
  TTree* cuttree = (TTree*) file->Get("cuts");

  double muon_time_cut = 0;
  cuttree->SetBranchAddress("muon_time_cut", &muon_time_cut);
  double min_muon_energy = 0;
  cuttree->SetBranchAddress("min_muon_energy", &min_muon_energy);
  double max_muon_energy = 0;
  cuttree->SetBranchAddress("max_muon_energy", &max_muon_energy);
  cuttree->GetEntry(0);

  
  double min_time_cut = -muon_time_cut;
  double max_time_cut = muon_time_cut;
  int min_time_cut_bin = hTimeEnergy->GetXaxis()->FindBin(min_time_cut);
  int max_time_cut_bin = hTimeEnergy->GetXaxis()->FindBin(max_time_cut);

  TH1D* hMuonCut = hTimeEnergy->ProjectionY("hMuonCut", min_time_cut_bin, max_time_cut_bin);
  hMuonCut->Draw();

  TF1* peak_fit = new TF1("peak_fit", "[0]*TMath::Gaus(x, [1], [2])", min_muon_energy, max_muon_energy);
  peak_fit->SetParameters(hMuonCut->GetMaximum(),4000, 100);
  hMuonCut->Fit(peak_fit, "R");
  
}
