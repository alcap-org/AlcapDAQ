void NormalisedProtonSpectrum() {

  ////////////////////////
  // User parameters
  std::string unfolded_filename = "~/data/results/Si16b_passive/unfolded.root";
  std::string unfolded_histname = "hUnfoldedSpectrum";
  std::string mustop_filename = "~/data/results/Si16b_passive/CountStoppedMuons_GeLoGain.root";
  std::string mustop_treename = "mustops";
  std::string outfilename = "~/data/results/Si16b_passive/final-plots.root";

  TFile* unfolded_file = new TFile(unfolded_filename.c_str(), "READ");
  TH1F* hUnfoldedSpectrum = (TH1F*) unfolded_file->Get(unfolded_histname.c_str());

  TFile* mustop_file = new TFile(mustop_filename.c_str(), "READ");
  TTree* mustop_tree = (TTree*) mustop_file->Get(mustop_treename.c_str());

  double n_stopped_muons = 0;
  double n_stopped_muons_error = 0;
  mustop_tree->SetBranchAddress("n_stopped_muons", &n_stopped_muons);
  mustop_tree->SetBranchAddress("n_stopped_muons_error", &n_stopped_muons_error);
  mustop_tree->GetEntry(0);

  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");

  TH1F* hNormalisedSpectrum = (TH1F*) hUnfoldedSpectrum->Clone("hNormalisedSpectrum");
  hNormalisedSpectrum->Scale(1.0 / n_stopped_muons);
  hNormalisedSpectrum->Rebin(2);
  hNormalisedSpectrum->Scale(1.0 / 200); // bin_width

  hNormalisedSpectrum->GetXaxis()->SetRangeUser(0, 13000);
  hNormalisedSpectrum->Draw("HIST E");

  TF1* proton_spec_fit = new TF1("proton_spec_fit", "[0]*(1 - [1]/x)^[2] * TMath::Exp(-x/[3])", 2400, 12000);
  proton_spec_fit->SetParameters(0.05, 1500, 1.1, 5000);
  hNormalisedSpectrum->Fit(proton_spec_fit, "R");
  proton_spec_fit->Draw("LSAME");

  std::cout << proton_spec_fit->Integral(3000, 1000000) << std::endl;
  
  hNormalisedSpectrum->Write();
  outfile->Write();
  //  outfile->Close();		    
}
