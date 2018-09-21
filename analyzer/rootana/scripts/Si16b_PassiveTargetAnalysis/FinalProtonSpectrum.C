void FinalProtonSpectrum() {

  ////////////////////////
  // User parameters
  std::string unfolded_filename = "~/data/results/Si16b_passive/unfolded.root";
  std::string unfolded_histname = "hUnfoldedSpectrum";
  std::string mustop_filename = "~/data/results/Si16b_passive/CountStoppedMuons_GeLoGain.root";
  std::string mustop_treename = "mustops";
  std::string outfilename = "~/data/results/Si16b_passive/final-plots.root";

  TFile* unfolded_file = new TFile(unfolded_filename.c_str(), "READ");
  TH1F* hUnfoldedSpectrum = (TH1F*) unfolded_file->Get(unfolded_filename.c_str());

  TFile* mustop_file = new TFile(mustop_filename.c_str(), "READ");
  TTree* mustop_tree = (TTree*) mustop_file->Get(mustop_treename.c_str());

  double n_stopped_muons = 0;
  double n_stopped_muons_error = 0;
  mustop_tree->SetBranchAddress("n_stopped_muons", n_stopped_muons);
  mustop_tree->SetBranchAddress("n_stopped_muons_error", n_stopped_muons_error);
  mustop_tree->GetEntry(0);

  hUnfoldedSpectrum->Scale(1.0 / n_stopped_muons_error);

  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  hUnfoldedSpectrum->Write();
  outfile->Write();
  outfile->Close();		    
}
